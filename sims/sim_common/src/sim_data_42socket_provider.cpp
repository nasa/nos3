/* Copyright (C) 2015 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

   This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
   limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
   for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
   any warranty that the software will be error free.

   In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
   arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
   contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
   documentation or services provided hereunder

   ITC Team
   NASA IV&V
   ivv-itc@lists.nasa.gov
*/

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
//#include <fcntl.h>

#include <ItcLogger/Logger.hpp>

#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    //REGISTER_DATA_PROVIDER(SimData42SocketProvider,"42SOCKET");

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors / Destructors
     *************************************************************************/

    SimData42SocketProvider::SimData42SocketProvider(const boost::property_tree::ptree& config)
        : SimIDataProvider(config), _socket_client_thread(NULL), _not_terminating(true),
          _max_connection_attempts(config.get("simulator.hardware-model.data-provider.max-connection-attempts", 5)),
          _retry_wait_seconds(config.get("simulator.hardware-model.data-provider.retry-wait-seconds", 5)),
          _init_time(config.get("common.absolute-start-time", 552110400.0))
    {
        _ECEF.resize(3);
        _ECI.resize(3);
        _ECI_vel.resize(3);
        _svn.resize(3);
        _bvn.resize(3);
        _hvn.resize(3);
        _qbn.resize(4);
    }

    SimData42SocketProvider::~SimData42SocketProvider(void)
    {
        _not_terminating = false;
        if (_socket_client_thread != NULL) {
            _socket_client_thread->join();
            delete _socket_client_thread;
        }
        close(_socket_fd); // close the socket
        close(_cmd_socket_fd); // close the 42 cmd socket
    }

    /*************************************************************************
     * Non-mutating public worker methods
     *************************************************************************/

    /*************************************************************************
     * Protected mutating worker methods
     *************************************************************************/

    void SimData42SocketProvider::connect_reader_thread_as_42_socket_client(std::string server_host, uint16_t server_port)
    {
        // http://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c/8257728#8257728
        int length = (int)((ceil(log10(server_port))+1)*sizeof(char)); 
        char port_string[length];
        snprintf(port_string, length, "%ud", server_port);

        // http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
        struct addrinfo hints, *servinfo, *p;
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        for (int i = 0; i < _max_connection_attempts + 1; i++) 
        {
            if ((rv = getaddrinfo(server_host.c_str(), port_string, &hints, &servinfo)) != 0) 
            {
                sim_logger->error("SimData42SocketProvider::connect_reader_thread_as_42_socket_client:  Error getting address: %s", gai_strerror(rv));
                return;
            }

            // loop through all the results and connect to the first we can
            for(p = servinfo; p != NULL; p = p->ai_next) 
            {
                if ((_socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
                {
                    sim_logger->warning("SimData42SocketProvider::connect_reader_thread_as_42_socket_client:  Continuing, but could not create socket: %s", strerror(errno));
                    continue;
                }
                if (connect(_socket_fd, p->ai_addr, p->ai_addrlen) == -1) 
                {
                    close(_socket_fd);
                    sim_logger->warning("SimData42SocketProvider::connect_reader_thread_as_42_socket_client:  Continuing, but could not connect socket: %s", strerror(errno));
                    continue;
                }
                break;
            }

            if (p != NULL) 
            {
                break;
            } else 
            {
                if (i == _max_connection_attempts) 
                {
                    sim_logger->error("SimData42SocketProvider::connect_reader_thread_as_42_socket_client:  Maximum number of connection attempts readched.  Failed to connect!");
                    return;
                } 
                else 
                {
                    sim_logger->warning("SimData42SocketProvider::connect_reader_thread_as_42_socket_client:  Warning... failed to connect... retrying in %d seconds.",
                                        _retry_wait_seconds);
                    sleep(_retry_wait_seconds);
                }
            }
        }

        sim_logger->debug("SimData42SocketProvider::connect_reader_thread_as_42_socket_client:  Successfully connected to 42, starting reader thread!");

        _socket_client_thread = new std::thread(std::bind(&SimData42SocketProvider::socket_reader, this), NULL); // And spawn thread to read from socket

        return; // connected and going!!! don't forget to disconnect when you are done!!
    }

    void SimData42SocketProvider::connect_cmd_thread_as_42_socket_client(std::string server_host, uint16_t server_port)
    {   
        int length = (int)((ceil(log10(server_port))+1)*sizeof(char)); 
        char port_string[length];
        snprintf(port_string, length, "%ud", server_port);

        struct addrinfo hints, *servinfo, *p;
        int rv;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        for (int i = 0; i < _max_connection_attempts + 1; i++) 
        {
            if ((rv = getaddrinfo(server_host.c_str(), port_string, &hints, &servinfo)) != 0) 
            {
                sim_logger->error("SimData42SocketProvider::connect_cmd_thread_as_42_socket_client:  Error getting address: %s", gai_strerror(rv));
                return;
            }

            // loop through all the results and connect to the first we can
            for(p = servinfo; p != NULL; p = p->ai_next) 
            {
                if ((_cmd_socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
                {
                    sim_logger->warning("SimData42SocketProvider::connect_cmd_thread_as_42_socket_client:  Continuing, but could not create socket: %s", strerror(errno));
                    continue;
                }
                if (connect(_cmd_socket_fd, p->ai_addr, p->ai_addrlen) == -1) 
                {
                    close(_cmd_socket_fd);
                    sim_logger->warning("SimData42SocketProvider::connect_cmd_thread_as_42_socket_client:  Continuing, but could not connect socket: %s", strerror(errno));
                    continue;
                }
                break;
            }

            if (p != NULL) 
            {
                break;
            } 
            else 
            {
                if (i == _max_connection_attempts) 
                {
                    sim_logger->error("SimData42SocketProvider::connect_cmd_thread_as_42_socket_client:  Maximum number of connection attempts readched.  Failed to connect!");
                    return;
                } 
                else 
                {
                    sim_logger->warning("SimData42SocketProvider::connect_cmd_thread_as_42_socket_client:  Warning... failed to connect... retrying in %d seconds.",
                                        _retry_wait_seconds);
                    sleep(_retry_wait_seconds);
                }
            }
        }

        sim_logger->debug("SimData42SocketProvider::connect_cmd_thread_as_42_socket_client:  Successfully connected to 42 via port %d, ready to send commands!", server_port);
        return;
    }

    /*************************************************************************
     * Private helper methods
     *************************************************************************/

    void SimData42SocketProvider::socket_reader(void)
    {
        while (_not_terminating) 
        {
            read_socket_data();

            {
                std::lock_guard<std::mutex> lock(_data_point_mutex);
                Sim42DataPoint dp(_abs_time, _gps_week, _gps_sec_week, _gps_frac_sec, _ECEF, _ECI, _ECI_vel, _svn, _bvn, _hvn, _qbn);
                _data_point = dp;
                // Lock is released when scope ends
            }

            //sim_logger->debug("SimData42SocketProvider::socket_reader:  Data Point=%s\n", _data_point.to_formatted_string().c_str());
        }
    }

    void SimData42SocketProvider::read_socket_data(void)
    {
        long Done = 0;
        char line[512] = "Blank";
        char *LineIsValid;
        long IntVal1,IntVal2,IntVal3,IntVal4;
        double DblVal1,DblVal2,DblVal3,DblVal4,DblVal5,DblVal6,DblVal7,DblVal8,DblVal9;
        char MnemString[80];

        while(!Done) 
        {
            LineIsValid = rgets(line, 511, _socket_fd);
            sim_logger->trace("SimData42SocketProvider::read_socket_data:  Line=%s", line);
            if (LineIsValid == NULL) 
            {
                Done = 1;
            }
            if (sscanf(line, "%s %ld-%ld-%ld:%ld:%lf", MnemString, &IntVal1, &IntVal2, &IntVal3, &IntVal4, &DblVal1) == 6) 
            {
                if (!strcmp(MnemString, "TIME")) 
                {
                    long Year = IntVal1;
                    long doy = IntVal2;
                    long Hour = IntVal3;
                    long Minute = IntVal4;
                    long Second = DblVal1;
                    long Day, Month;
                    DOY2MD(Year, doy, &Month, &Day);
                    _abs_time = DateToAbsTime(Year, Month, Day, Hour, Minute, Second);
                    long GpsRollover;
                    double GpsSecond;
                    JDToGpsTime(AbsTimeToJD(_abs_time), &GpsRollover, &_gps_week, &GpsSecond);
                    _gps_sec_week = (long)GpsSecond;
                    _gps_frac_sec = GpsSecond - (double)_gps_sec_week;
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found TIME.  Line=%s, Abs Time=%f, GPS Time=%ld/%ld/%f",
                        line, _abs_time, _gps_week, _gps_sec_week, _gps_frac_sec);
                }
            }
            if (sscanf(line, "%s %ld", MnemString, &IntVal1) == 2) 
            {
                if (!strcmp(MnemString, "SC")) 
                {
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found SC.  Line=%s", line);
                   // SC #
                   // TODO - Be careful... 42 can output state for multiple spacecraft... for now I am just assuming 1!
                }
            }
            if (sscanf(line, "%s %lf %lf %lf", MnemString, &DblVal1, &DblVal2, &DblVal3) == 4) 
            {
                sim_logger->trace("SimData42SocketProvider::read_socket_data:    Found mnemonic + 3 floats.  Line=%s", line);
                if (!strcmp(MnemString, "POSITION")) 
                {
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found POSITION.  Line=%s, ECI=%lf/%lf/%lf",
                        line, DblVal1, DblVal2, DblVal3);
                    _ECI[0] = DblVal1;
                    _ECI[1] = DblVal2;
                    _ECI[2] = DblVal3;
                }
                if (!strcmp(MnemString, "POSITION_W")) 
                {
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found POSITION_W.  Line=%s, ECI=%lf/%lf/%lf",
                        line, DblVal1, DblVal2, DblVal3);
                    _ECEF[0] = DblVal1;
                    _ECEF[1] = DblVal2;
                    _ECEF[2] = DblVal3;
                }
                if (!strcmp(MnemString, "VELOCITY")) 
                {
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found VELOCITY.  Line=%s, Velocity=%lf/%lf/%lf",
                        line, DblVal1, DblVal2, DblVal3);
                    _ECI_vel[0] = DblVal1;
                    _ECI_vel[1] = DblVal2;
                    _ECI_vel[2] = DblVal3;
                }
                if (!strcmp(MnemString, "SUNVEC")) 
                {
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found SUNVEC.  Line=%s, Sun Vector=%lf/%lf/%lf",
                        line, DblVal1, DblVal2, DblVal3);
                    _svn[0] = DblVal1;
                    _svn[1] = DblVal2;
                    _svn[2] = DblVal3;
                }
                if (!strcmp(MnemString, "MAGVEC")) 
                {
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found MAGVEC.  Line=%s, Magnetic Vector=%lf/%lf/%lf",
                        line, DblVal1, DblVal2, DblVal3);
                    _bvn[0] = DblVal1;
                    _bvn[1] = DblVal2;
                    _bvn[2] = DblVal3;
                }
                if (!strcmp(MnemString, "ANGVEL")) 
                {
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found ANGVEL.  Line=%s, Ang Velocity=%lf/%lf/%lf",
                        line, DblVal1, DblVal2, DblVal3);
                    _hvn[0] = DblVal1;
                    _hvn[1] = DblVal2;
                    _hvn[2] = DblVal3;
                }
            }
            if (sscanf(line, "%s %lf %lf %lf %lf", MnemString, &DblVal1, &DblVal2, &DblVal3, &DblVal4) == 5) 
            {
                sim_logger->trace("SimData42SocketProvider::read_socket_data:    Found mnemonic + 4 floats.  Line=%s", line);
                if (!strcmp(MnemString, "QBN")) 
                {
                    sim_logger->trace("SimData42SocketProvider::read_socket_data:      Found QBN.  Line=%s, Quaternion=%lf/%lf/%lf/%lf",
                        line, DblVal1, DblVal2, DblVal3, DblVal4);
                    _qbn[0] = DblVal1;
                    _qbn[1] = DblVal2;
                    _qbn[2] = DblVal3;
                    _qbn[3] = DblVal4;
                }
            }
            if (!strncmp(line,"[EOF]",5)) Done = 1;
        }
    }

    char SimData42SocketProvider::rgetc(int fd)
    {
        char buf;
        if (read(fd, &buf, 1) != 1)
            return EOF;
        return buf;
    }

    char* SimData42SocketProvider::rgets(char *s, int n, int fd) /* K&R, 2nd, p. 165 */
    {
        register int c;
        register char *cs;

        cs = s;
        while (--n > 0 && (c = rgetc(fd)) != EOF) // ssize_t read(int fd, void *buf, size_t count);
            if ((*cs++ = c) == '\n')
                break;
        *cs = '\0';
        if (cs != s) *(cs-1) = '\0'; // Hack off the \n
        return (c == EOF && cs == s) ? NULL : s;
    }

    //
    // Note: The following "cmd_*" functions assume communications with SC 0, Body 0, and use the primary vector
    //

    void SimData42SocketProvider::cmd_qrn(double q1, double q2, double q3, double q4)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf SC[%d] qrn = [%lf %lf %lf %lf]", 
                 _abs_time - _init_time, 0, q1, q2, q3, q4
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_qrl(double q1, double q2, double q3, double q4)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf SC[%d] qrl = [%lf %lf %lf %lf]", 
                 _abs_time - _init_time, 0, q1, q2, q3, q4
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_angles_wrt_frame(double ang1, double ang2, double ang3, long rotSeq, char frame)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf SC[%d] Cmd Angles = [%lf %lf %lf] Seq = %ld wrt %c Frame", 
                 _abs_time - _init_time, 0, ang1, ang2, ang3, rotSeq, frame
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_angles(double ang1, double ang2, double ang3)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf SC[%d].G[%d] Cmd Angles = [%lf %lf %lf]", 
                 _abs_time - _init_time, 0, 0, ang1, ang2, ang3
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_vector_ra_dec(double vecR0, double vecR1, double vecR2, double ra, double dec)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Point SC[%d].B[%d] %s Vector [%lf %lf %lf] at RA = %lf Dec = %lf", 
                 _abs_time - _init_time, 0, 0, "Primary", vecR0, vecR1, vecR2, ra, dec
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_vector_world_lng_lat_alt(double vecR0, double vecR1, double vecR2, int world, double lng, double lat, double alt)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Point SC[%d].B[%d] %s Vector [%lf %lf %lf] at World[%d] Lng = %lf Lat = %lf Alt = %lf", 
                 _abs_time - _init_time, 0, 0, "Primary", vecR0, vecR1, vecR2, world, lng, lat, alt
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_vector_world(double vecR0, double vecR1, double vecR2, int world)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Point SC[%d].B[%d] %s Vector [%lf %lf %lf] at World[%d]", 
                 _abs_time - _init_time, 0, 0, "Primary", vecR0, vecR1, vecR2, world
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_vector_ground_station(double vecR0, double vecR1, double vecR2, int groundStation)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Point SC[%d].B[%d] %s Vector [%lf %lf %lf] at GroundStation[%d]", 
                 _abs_time - _init_time, 0, 0, "Primary", vecR0, vecR1, vecR2, groundStation
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_vector_sc_point(double vecR0, double vecR1, double vecR2, long sc, long body, double vec0, double vec1, double vec2)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Point SC[%d].B[%d] %s Vector [%lf %lf %lf] at SC[%ld].B[%ld] point [%lf %lf %lf]", 
                 _abs_time - _init_time, 0, 0, "Primary", vecR0, vecR1, vecR2, sc, body, vec0, vec1, vec2
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_vector_sc(double vecR0, double vecR1, double vecR2, long sc)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Point SC[%d].B[%d] %s Vector [%lf %lf %lf] at SC[%ld]", 
                 _abs_time - _init_time, 0, 0, "Primary", vecR0, vecR1, vecR2, sc
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_vector_point_at(double vecR0, double vecR1, double vecR2, const char* target)
    {   // Assumes that target has a NULL terminator
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Point SC[%d].B[%d] %s Vector [%lf %lf %lf] at %s", 
                 _abs_time - _init_time, 0, 0, "Primary", vecR0, vecR1, vecR2, target
                );
        sim_logger->debug("SimData42SocketProvider::cmd_vector_point_at: %s", line);
        sim_logger->debug("sizeof(line) = %d", sizeof(line));
        write(_cmd_socket_fd, line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_align(double vecR0, double vecR1, double vecR2, long sc, long body, double vec0, double vec1, double vec2)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Align SC[%d].B[%d] %s Vector [%lf %lf %lf] with SC[%ld].B[%ld] vector [%lf %lf %lf]", 
                 _abs_time - _init_time, 0, 0, "Primary", vecR0, vecR1, vecR2, sc, body, vec0, vec1, vec2
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    void SimData42SocketProvider::cmd_align_c_frame(double vecR0, double vecR1, double vecR2, char frameChar, double vec0, double vec1, double vec2)
    {
        char line[512];
        snprintf(line, sizeof(line), 
                 "%lf Align SC[%d].B[%d] %s Vector [%lf %lf %lf] with %c-frame Vector [%lf %lf %lf]", 
                 _abs_time - _init_time, 0, 0, "Primary\0", vecR0, vecR1, vecR2, frameChar, vec0, vec1, vec2
                );
        write(_cmd_socket_fd, &line, sizeof(line));
    }

    /**********************************************************************/
    /*  Find Month, Day, given Day of Year                                */
    /*  Ref. Jean Meeus, 'Astronomical Algorithms', QB51.3.E43M42, 1991.  */

    void SimData42SocketProvider::DOY2MD(long Year, long DayOfYear, long *Month, long *Day)
    {
          long K;

          if (Year % 4 == 0) 
          {
             K = 1;
          }
          else 
          {
             K = 2;
          }

          if (DayOfYear < 32) 
          {
             *Month = 1;
          }
          else 
          {
             *Month = (long) (9.0*(K+DayOfYear)/275.0+0.98);
          }

          *Day = DayOfYear - 275*(*Month)/9 + K*(((*Month)+9)/12) + 30;

    }

    /**********************************************************************/
    /*  Convert Year, Month, Day, Hour, Minute and Second to              */
    /*  "Absolute Time", i.e. seconds elapsed since J2000 epoch.          */
    /*  J2000 = 2451545.0 TT  =  01 Jan 2000 12:00:00.00 TT               */
    /*  Year, Month, Day assumed in Gregorian calendar. (Not true < 1582) */
    /*  Ref. Jean Meeus, 'Astronomical Algorithms', QB51.3.E43M42, 1991.  */

    double SimData42SocketProvider::DateToAbsTime(long Year, long Month, long Day, long Hour,
       long Minute, double Second)
    {
          long A,B;
          double Days;

          if (Month < 3) 
          {
             Year--;
             Month+=12;
          }

          A = Year/100;
          B = 2 - A + A/4;

          /* Days since J2000 Epoch (01 Jan 2000 12:00:00.0) */
          Days = floor(365.25*(Year+4716))
                      + floor(30.6001*(Month+1))
                      + Day + B - 1524.5 - 2451545.0;

          /* Add fractional day */
          return(86400.0*Days + 3600.0*((double) Hour)
             + 60.0*((double) Minute) + Second);
    }

    /**********************************************************************/
    /* AbsTime is elapsed seconds since J2000 epoch                       */
    double SimData42SocketProvider::AbsTimeToJD(double AbsTime)
    {
          return(AbsTime/86400.0 + 2451545.0);
    }

    /**********************************************************************/
    /* GPS Epoch is 6 Jan 1980 00:00:00.0 which is JD = 2444244.5         */
    /* GPS Time is expressed in weeks and seconds                         */
    /* GPS Time rolls over every 1024 weeks                               */
    void SimData42SocketProvider::JDToGpsTime(double JD, long *GpsRollover, long *GpsWeek, double *GpsSecond)
    {
          double DaysSinceEpoch, DaysSinceRollover, DaysSinceWeek;

          DaysSinceEpoch = JD - 2444244.5;
          *GpsRollover = (long) (DaysSinceEpoch/7168.0);
          DaysSinceRollover = DaysSinceEpoch - 7168.0*((double) *GpsRollover);
          *GpsWeek = (long) (DaysSinceRollover/7.0);
          DaysSinceWeek = DaysSinceRollover - 7.0*((double) *GpsWeek);
          *GpsSecond = DaysSinceWeek*86400.0;
    }

}
