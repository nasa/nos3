/* Copyright (C) 2015 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#ifndef NOS3_GPSSIMDATAPOINT_HPP
#define NOS3_GPSSIMDATAPOINT_HPP

#include <cstdint>
#include <string>

#include <sim_i_data_point.hpp>

namespace Nos3
{

    /** \brief Class to contain a file entry of GPS simulation data for a specific time.
     *
     *  The GPS data is actually for a specific orbit location and attitude for the spacecraft also.
     *
     *  Notes on symbols/abbreviations:
     *  - v is for vector
     *  - n is the standard symbol used for inertial reference frame (independent of spacecraft attitude),
     *    b is the standard symbol used for body reference frame (depends on spacecraft attitude)
     *  - x, y, z... are for the x, y, z components
     *
     *  !!! UNITS !!!:
     *    absolute time is in seconds, with an epoch of ??
     *    inertial reference frame has center at ??, x pointing in ?? direction, y pointing in ?? direction, z pointing in ?? direction
     *    body reference frame has center at ?? (mag center?  spacecraft center?), x pointing in ?? direction, y pointing in ?? direction, z pointing in ?? direction
     *
     *  TBD:  Specify units of GPS data.  Specify exactly what
     *    inertial reference frame we are talking about.  Specify exactly how the body reference frame is set up, e.g. do
     *    we need to worry about a rotation matrix to reference the magnetometer body reference frame to the cubesat body
     *    reference frame.
     */
    class GPSSimDataPoint : public SimIDataPoint
    {
    public:
        /// @name Constructors
        //@{
        /** \brief Default constructor
         *
         *  Just zeroes out all the data
         *
         */
        GPSSimDataPoint();
        GPSSimDataPoint(double abs_time, int16_t gps_week, int32_t gps_sec_week, double gps_frac_sec,
            const std::vector<double>& ECEF, const std::vector<double>& ECI,
            const std::vector<std::vector<double>>& DCM, const std::vector<double>& velocity);
        //@}

        /// @name Accessors
        //@{
        /// \brief Returns the absolute time of the GPS simulation data point
        /// @return     The absolute time of the data point
        double      get_abs_time(void) const {return _abs_time;};
        /// \brief Returns a block formatted string representation of the GPS simulation data point
        /// @return     A block formatted string representation of the GPS simulation data point
        std::string to_formatted_string(void) const;
        /// \brief Returns one long single string representation of the GPS simulation data point
        /// @return     A long single string representation of the GPS simulation data point
        std::string to_string(void) const;

        int16_t get_gps_week(void) const {return _gps_week;}
        int32_t get_gps_sec_week(void) const {return _gps_sec_week;}
        double get_gps_frac_sec(void) const {return _gps_frac_sec;}
        double get_ECEF_x(void) const {return _ECEF[0];}
        double get_ECEF_y(void) const {return _ECEF[1];}
        double get_ECEF_z(void) const {return _ECEF[2];}
        double get_velocity_x(void) const {return _ECI_vel[0];}
        double get_velocity_y(void) const {return _ECI_vel[1];}
        double get_velocity_z(void) const {return _ECI_vel[2];}
        //@}
    private:
        // Private data
        double _abs_time;
        int16_t _gps_week; // Unambiguous GPS Week
        int32_t _gps_sec_week; // Integer seconds elapsed since the start of the GPS week
        double _gps_frac_sec; // Fractions of a second beyond the integer seconds_of_week
        std::vector<double> _ECEF, _ECI, _ECI_vel;
        std::vector<std::vector<double>> _DCM;

    };

}

#endif

