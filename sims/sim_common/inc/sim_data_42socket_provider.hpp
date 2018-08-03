/* Copyright (C) 2015 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef NOS3_SIMDATA42SOCKETPROVIDER_HPP
#define NOS3_SIMDATA42SOCKETPROVIDER_HPP

#include <thread>
#include <mutex>

#include <sim_i_data_provider.hpp>
#include <sim_42data_point.hpp>

namespace Nos3
{
    /** \brief Class for a provider of simulation data that provides data from a 42 socket connection.
     *
     *  This class can concretely retrieve data from a 42 socket... but is still virtual because it is up
     *  to a derived class to determine what 42 data it should be a provider of... the get_data_point()
     *  method is still pure virtual.  Now it does need its derived class to perform
     *  connect_reader_thread_as_42_socket_client(), otherwise the 42 data point will never be
     *  set with any valid data... this allows the derived class to specify the endpoint
     *  information, but places all the shared code for reading 42 data in this class.
     */
    class SimData42SocketProvider : public SimIDataProvider
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  sc  The configuration for the simulation
        SimData42SocketProvider(const boost::property_tree::ptree& config);
        ~SimData42SocketProvider(void);
        //@}

        /// @name Non-mutating public worker methods
        //@{
        /** \brief Method to retrieve simulation data.
         *
         * @returns                     A data point of simulation data.
         */
        virtual boost::shared_ptr<SimIDataPoint> get_data_point(void) const
        {
            boost::shared_ptr<Sim42DataPoint> dp;
            {
                std::lock_guard<std::mutex> lock(_data_point_mutex);
                dp = boost::shared_ptr<Sim42DataPoint>(new Sim42DataPoint(_data_point));
                // Lock is released when scope ends
            }
            return dp;
        }
        //@}

    protected:
        /// @name Mutating protected worker methods
        //@{
        /** \brief Method to connect to a 42 socket and start reading data
         *
         * @param       server_host        The host name or IP address of the 42 server.
         * @param       server_port        The port number of the 42 server.
         */
        void connect_reader_thread_as_42_socket_client(std::string server_host, uint16_t server_port);

        /** \brief Method to connect to a 42 socket to send commands to 42 "FSW"
         *
         * @param       server_host        The host name or IP address of the 42 server.
         * @param       server_port        The port number of the 42 server.
         */
        void connect_cmd_thread_as_42_socket_client(std::string server_host, uint16_t server_port);
        //@}

    private:
        // Private helper methods
        void socket_reader(void);
        void read_socket_data(void);
        static char rgetc(int fd);
        static char *rgets(char *s, int n, int fd);
        static void DOY2MD(long Year, long DayOfYear, long *Month, long *Day);
        static double DateToAbsTime(long Year, long Month, long Day, long Hour, long Minute, double Second);
        static double AbsTimeToJD(double AbsTime);
        static void JDToGpsTime(double JD, long *GpsRollover, long *GpsWeek, double *GpsSecond);
        
        void cmd_qrn(double q1, double q2, double q3, double q4);
        void cmd_qrl(double q1, double q2, double q3, double q4);
        void cmd_angles_wrt_frame(double ang1, double ang2, double ang3, long rotSeq, double frame);
        void cmd_angles(double ang1, double ang2, double ang3);
        void cmd_vector_ra_dec(double vecR0, double vecR1, double vecR2, double ra, double dec);
        void cmd_vector_world_lng_lat_alt(double vecR0, double vecR1, double vecR2, double world, double lng, double lat, double alt);
        void cmd_vector_world(double vecR0, double vecR1, double vecR2, double world);
        void cmd_vector_ground_station(double vecR0, double vecR1, double vecR2, double groundStation);
        void cmd_vector_sc_point(double vecR0, double vecR1, double vecR2, long sc, long body, double vec0, double vec1, double vec2);
        void cmd_vector_sc(double vecR0, double vecR1, double vecR2, long sc);
        void cmd_vector_point_at(double vecR0, double vecR1, double vecR2, const char* target);
        void cmd_align(double vecR0, double vecR1, double vecR2, long sc, long body, double vec0, double vec1, double vec2);
        void cmd_align_c_frame(double vecR0, double vecR1, double vecR2, char frameChar, double vec0, double vec1, double vec2);

        // Private data
        // ... connection data
        std::string _server_host;
        std::string _server_port;
        int _max_connection_attempts;
        int _retry_wait_seconds;
        int _socket_fd;
        int _cmd_socket_fd;
        double _init_time;

        // ... reader thread / thread state data
        std::thread *_socket_client_thread;
        bool _not_terminating; // Used to signal the thread when we are terminating so it quits reading the socket

        // ... data read from the socket
        double _abs_time, _gps_frac_sec;
        long _gps_week, _gps_sec_week;
        std::vector<double> _ECEF, _ECI, _ECI_vel, _svn, _bvn, _hvn, _qbn;
        long _eclipse;
        std::vector<std::vector<double>> _DCM, _cbn;

        // ... a data point of data read from the socket
        Sim42DataPoint _data_point;
        mutable std::mutex _data_point_mutex;  // protects _data_point

    };
}

#endif
