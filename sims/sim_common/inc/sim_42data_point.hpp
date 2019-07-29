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

#ifndef NOS3_SIM42DATAPOINT_HPP
#define NOS3_SIM42DATAPOINT_HPP

#include <cstdint>
#include <string>

#include <sim_i_data_point.hpp>

namespace Nos3
{

    /** \brief Class to contain an entry of 42 simulation data.
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
    class Sim42DataPoint : public SimIDataPoint
    {
    public:
        /// @name Constructors
        //@{
        /** \brief Default constructor
         *
         *  Just zeroes out all the data
         *
         */
        Sim42DataPoint();
        Sim42DataPoint(double abs_time, int32_t gps_week, int32_t gps_sec_week, double gps_frac_sec,
                const std::vector<double>& ECEF, const std::vector<double>& ECI, const std::vector<double>& velocity,
                const std::vector<double>& svn,  const std::vector<double>& bvn, const std::vector<double>& hvn,
                const std::vector<double>& qbn);
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

        int32_t get_gps_week(void) const {return _gps_week;}
        int32_t get_gps_sec_week(void) const {return _gps_sec_week;}
        double get_gps_frac_sec(void) const {return _gps_frac_sec;}
        std::vector<double> get_ECEF(void) const {return _ECEF;}
        double get_ECEF_x(void) const {return _ECEF[0];}
        double get_ECEF_y(void) const {return _ECEF[1];}
        double get_ECEF_z(void) const {return _ECEF[2];}
        std::vector<double> get_ECEF_vel(void) const {return _ECEF_vel;}
        std::vector<double> get_ECEF_velocity(void) const {return _ECEF_vel;}
        std::vector<double> get_ECI(void) const {return _ECI;}
        double get_ECI_x(void) const {return _ECI[0];}
        double get_ECI_y(void) const {return _ECI[1];}
        double get_ECI_z(void) const {return _ECI[2];}
        std::vector<double> get_ECI_vel(void) const {return _ECI_vel;}
        std::vector<double> get_ECI_velocity(void) const {return _ECI_vel;}
        std::vector<double> get_svn(void) const {return _svn;}
        std::vector<double> get_bvn(void) const {return _bvn;}
        std::vector<double> get_hvn(void) const {return _hvn;}
        std::vector<double> get_qbn(void) const {return _qbn;}
        //@}
    private:
        // Private data
        double _abs_time;
        int32_t _gps_week; // Unambiguous GPS Week
        int32_t _gps_sec_week; // Integer seconds elapsed since the start of the GPS week
        double _gps_frac_sec; // Fractions of a second beyond the integer seconds_of_week
        std::vector<double> _ECEF, _ECEF_vel, _ECI, _ECI_vel, _svn, _bvn, _hvn; // 3 elements each
        std::vector<double> _qbn; // 4 element quaternion body/inertial
    };

}

#endif

