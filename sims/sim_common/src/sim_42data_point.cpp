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

#include <iomanip>
#include <limits>

//#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <ItcLogger/Logger.hpp>

#include <sim_42data_point.hpp>

namespace Nos3
{

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/

    Sim42DataPoint::Sim42DataPoint()
    {
        _ECEF.resize(3);
        _ECEF_vel.resize(3);
        _ECI.resize(3);
        _ECI_vel.resize(3);
        _svn.resize(3);
        _bvn.resize(3);
        _hvn.resize(3);
        _DCM.resize(3);
        _DCM[0].resize(3);
        _DCM[1].resize(3);
        _DCM[2].resize(3);
        _cbn.resize(3);
        _cbn[0].resize(3);
        _cbn[1].resize(3);
        _cbn[2].resize(3);
        _qbn.resize(4);
    }

    Sim42DataPoint::Sim42DataPoint(double abs_time, int32_t gps_week, int32_t gps_sec_week, double gps_frac_sec,
            const std::vector<double>& ECEF, const std::vector<double>& ECI, const std::vector<double>& ECI_vel,
            const std::vector<double>& svn, const std::vector<double>& bvn, const std::vector<double>& hvn, long eclipse,
            const std::vector<std::vector<double>>& DCM, const std::vector<std::vector<double>>& cbn, const std::vector<double>& qbn) :
            _abs_time(abs_time), _gps_week(gps_week), _gps_sec_week(gps_sec_week), _gps_frac_sec(gps_frac_sec),
            _ECEF(ECEF), _ECI(ECI), _ECI_vel(ECI_vel), _svn(svn), _bvn(bvn), _hvn(hvn), _eclipse(eclipse), _DCM(DCM), _cbn(cbn), _qbn(qbn)
    {
        sim_logger->trace("Sim42DataPoint::Sim42DataPoint:  Constructed data point with:  "
            "Abs Time:%lf, GPS Time:%d/%d/%lf, ECEF:%lf/%lf/%lf, ECI:%lf/%lf/%lf, ECI Velocity:%lf/%lf/%lf, "
            "Sun Vector:%lf/%lf/%lf, Mag Field Vector:%lf/%lf/%lf, SC Angular Momentum:%lf/%lf/%lf, Eclipse(1=yes,0=no):%ld, "
            "DCM:%lf/%lf/%lf////%lf/%lf/%lf////%lf/%lf/%lf, cbn:%lf/%lf/%lf////%lf/%lf/%lf////%lf/%lf/%lf, Body-Inertial Quaternion:%lf/%lf/%lf/%lf",
            _abs_time, _gps_week, _gps_sec_week, _gps_frac_sec,
            _ECEF[0], _ECEF[1], _ECEF[2], _ECI[0], _ECI[1], _ECI[2], _ECI_vel[0], _ECI_vel[1], _ECI_vel[2],
            _svn[0], _svn[1], _svn[2], _bvn[0], _bvn[1], _bvn[2], _hvn[0], _hvn[1], _hvn[2], _eclipse,
            _DCM[0][0], _DCM[0][1], _DCM[0][2], _DCM[1][0], _DCM[1][1], _DCM[1][2], _DCM[2][0], _DCM[2][1], _DCM[2][2],
            _cbn[0][0], _cbn[0][1], _cbn[0][2], _cbn[1][0], _cbn[1][1], _cbn[1][2], _cbn[2][0], _cbn[2][1], _cbn[2][2],
            _qbn[0], _qbn[1], _qbn[2], _qbn[3]);
    }

    /*************************************************************************
     * Accessors
     *************************************************************************/

    std::string Sim42DataPoint::to_formatted_string(void) const
    {
        std::stringstream ss;

        ss << std::fixed << std::setprecision(4) << std::setfill(' ');
        ss << "42 Data Point: " << std::endl;
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
        ss << "  ECI         : "
           << std::setw(12) << _ECI[0] << ","
           << std::setw(12) << _ECI[1] << ","
           << std::setw(12) << _ECI[2] << std::endl;
        ss << "  ECI Velocity: "
           << std::setw(12) << _ECI_vel[0] << ","
           << std::setw(12) << _ECI_vel[1] << ","
           << std::setw(12) << _ECI_vel[2] << std::endl;
        ss << "  SVN         : "
           << std::setw(12) << _svn[0] << ","
           << std::setw(12) << _svn[1] << ","
           << std::setw(12) << _svn[2] << std::endl;
        ss << "  BVN         : "
           << std::setw(12) << _bvn[0] << ","
           << std::setw(12) << _bvn[1] << ","
           << std::setw(12) << _bvn[2] << std::endl;
        ss << "  HVN         : "
           << std::setw(12) << _hvn[0] << ","
           << std::setw(12) << _hvn[1] << ","
           << std::setw(12) << _hvn[2] << std::endl;
        ss << " Eclipse      : " << std::setw(12) << _eclipse << std::endl;
        ss << std::setprecision(4);
        ss << "  DCM         : "
           << std::setw(12) << _DCM[0][0] << ","
           << std::setw(12) << _DCM[0][1] << ","
           << std::setw(12) << _DCM[0][2] << std::endl;
        ss << "            "
           << std::setw(12) << _DCM[1][0] << ","
           << std::setw(12) << _DCM[1][1] << ","
           << std::setw(12) << _DCM[1][2] << std::endl;
        ss << "            "
           << std::setw(12) << _DCM[2][0] << ","
           << std::setw(12) << _DCM[2][1] << ","
           << std::setw(12) << _DCM[2][2] << std::endl;
        ss << std::setprecision(4);
        ss << "  cbn         : "
           << std::setw(12) << _cbn[0][0] << ","
           << std::setw(12) << _cbn[0][1] << ","
           << std::setw(12) << _cbn[0][2] << std::endl;
        ss << "            "
           << std::setw(12) << _cbn[1][0] << ","
           << std::setw(12) << _cbn[1][1] << ","
           << std::setw(12) << _cbn[1][2] << std::endl;
        ss << "            "
           << std::setw(12) << _cbn[2][0] << ","
           << std::setw(12) << _cbn[2][1] << ","
           << std::setw(12) << _cbn[2][2] << std::endl;
        ss << std::setprecision(2);
        ss << "  qbn         : "
           << std::setw(12) << _qbn[0] << ","
           << std::setw(12) << _qbn[1] << ","
           << std::setw(12) << _qbn[2] << ","
           << std::setw(12) << _qbn[3] << std::endl;

        return ss.str();
    }

    std::string Sim42DataPoint::to_string(void) const
    {
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "42 Data Point: ";
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
        ss << " ECI "
           << _ECI[0] << ","
           << _ECI[1] << ","
           << _ECI[2] ;
        ss << " ECI Velocity"
           << _ECI_vel[0] << ","
           << _ECI_vel[1] << ","
           << _ECI_vel[2] ;
        ss << " SVN: "
           << _svn[0] << ","
           << _svn[1] << ","
           << _svn[2] ;
        ss << " BVN: "
           << _bvn[0] << ","
           << _bvn[1] << ","
           << _bvn[2] ;
        ss << " HVN: "
           << _hvn[0] << ","
           << _hvn[1] << ","
           << _hvn[2] ;
        ss << " Eclipse: " << _eclipse;
        ss << " DCM ("
           << _DCM[0][0] << ","
           << _DCM[0][1] << ","
           << _DCM[0][2] << "), ("
           << _DCM[1][0] << ","
           << _DCM[1][1] << ","
           << _DCM[1][2] << "), ("
           << _DCM[2][0] << ","
           << _DCM[2][1] << ","
           << _DCM[2][2] << ")";
        ss << " cbn ("
           << _cbn[0][0] << ","
           << _cbn[0][1] << ","
           << _cbn[0][2] << "), ("
           << _cbn[1][0] << ","
           << _cbn[1][1] << ","
           << _cbn[1][2] << "), ("
           << _cbn[2][0] << ","
           << _cbn[2][1] << ","
           << _cbn[2][2] << ")";
        ss << " qbn: "
           << _qbn[0] << ","
           << _qbn[1] << ","
           << _qbn[2] << ","
           << _qbn[3] ;

        return ss.str();
    }

}
