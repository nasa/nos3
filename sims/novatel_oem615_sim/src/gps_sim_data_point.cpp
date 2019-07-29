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

#include <iomanip>
#include <limits>

//#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <gps_sim_data_point.hpp>

namespace Nos3
{

    /*************************************************************************
     * Constructors
     *************************************************************************/

    GPSSimDataPoint::GPSSimDataPoint()
    {
        _ECEF.resize(3);
        _ECI.resize(3);
        _ECI_vel.resize(3);
    }

    GPSSimDataPoint::GPSSimDataPoint(double abs_time, int16_t gps_week, int32_t gps_sec_week, double gps_frac_sec,
        const std::vector<double>& ECEF, const std::vector<double>& ECI,
        const std::vector<double>& ECI_vel) :
            _abs_time(abs_time), _gps_week(gps_week), _gps_sec_week(gps_sec_week), _gps_frac_sec(gps_frac_sec),
            _ECEF(ECEF), _ECI(ECI), _ECI_vel(ECI_vel)
    {
    }

    /*************************************************************************
     * Accessors
     *************************************************************************/

    std::string GPSSimDataPoint::to_formatted_string(void) const
    {
        std::stringstream ss;

        ss << std::fixed << std::setprecision(4) << std::setfill(' ');
        ss << "GPS Data Point: " << std::endl;
        ss << "  Absolute Time                    : " << std::setw(15) << _abs_time << std::endl;
        ss << "  GPS Week/Second/Fractional Second: "
           << std::setw(6) << _gps_week << "/"
           << std::setw(7) << _gps_sec_week << "/"
           << std::setw(7) << _gps_frac_sec << std::endl;
        ss << std::setprecision(2);
        ss << "  ECEF        : "
           << std::setw(12) << _ECEF[0] << ","
           << std::setw(12) << _ECEF[1] << ","
           << std::setw(12) << _ECEF[2] << std::endl;
        ss << "  ECI Velocity: "
           << std::setw(12) << _ECI_vel[0] << ","
           << std::setw(12) << _ECI_vel[1] << ","
           << std::setw(12) << _ECI_vel[2] << std::endl;
        ss << "  ECI         : "
           << std::setw(12) << _ECI[0] << ","
           << std::setw(12) << _ECI[1] << ","
           << std::setw(12) << _ECI[2] << std::endl;

        return ss.str();
    }

    std::string GPSSimDataPoint::to_string(void) const
    {
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "GPS Data Point: ";
        ss << std::setprecision(std::numeric_limits<double>::digits10); // Full double precision
        ss << " AbsTime: " << _abs_time;
        ss << std::setprecision(std::numeric_limits<int32_t>::digits10); // Full int32_t precision
        ss << " GPS Time: "
           << _gps_week << "/"
           << _gps_sec_week << "/";
        ss << std::setprecision(std::numeric_limits<double>::digits10); // Full double precision
        ss << _gps_frac_sec ;
        ss << " ECEF: "
           << _ECEF[0] << ","
           << _ECEF[1] << ","
           << _ECEF[2] ;
        ss << " ECI Velocity"
           << _ECI_vel[0] << ","
           << _ECI_vel[1] << ","
           << _ECI_vel[2] ;
        ss << " ECI "
           << _ECI[0] << ","
           << _ECI[1] << ","
           << _ECI[2] ;

        return ss.str();
    }

}
