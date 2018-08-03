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

#include <cmath>

#include <ItcLogger/Logger.hpp>

#include <sim_coordinate_transformations.hpp>

namespace Nos3
{

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/
    SimCoordinateTransformations::SimConstants SimCoordinateTransformations::SIM_CONSTANTS;

    /*************************************************************************
     * Static Methods
     *************************************************************************/
    void SimCoordinateTransformations::AbsTime2YMDHMS(double abs_time, long& year, long& month, long& day,
                                 long& hour, long& minute, double& second)
    {
        JD2YMDHMS(AbsTimeToJD(abs_time), year, month, day, hour, minute, second);
    }

    /**********************************************************************/
    /* AbsTime is elapsed seconds since J2000 epoch                       */
    double SimCoordinateTransformations::AbsTimeToJD(double abs_time)
    {
          return(abs_time/86400.0 + 2451545.0);
    }

    /**********************************************************************/
    /*   Convert Julian Day to Year, Month, Day, Hour, Minute, and Second */
    /*   Ref. Jean Meeus, 'Astronomical Algorithms', QB51.3.E43M42, 1991. */

    void SimCoordinateTransformations::JD2YMDHMS(double jd, long& year, long& month, long& day,
                                 long& hour, long& minute, double& second)
    {
          double Z,F,A,B,C,D,E,alpha;
            double FD;

          Z= floor(jd+0.5);
          F=(jd+0.5)-Z;

          if (Z < 2299161.0) 
          {
             A = Z;
          }
          else 
          {
             alpha = floor((Z-1867216.25)/36524.25);
             A = Z+1.0+alpha - floor(alpha/4.0);
          }

          B = A + 1524.0;
          C = floor((B-122.1)/365.25);
          D = floor(365.25*C);
          E = floor((B-D)/30.6001);

          FD = B - D - floor(30.6001*E) + F;
          day = (long) FD;

          if (E < 14.0) 
          {
             month = (long) (E - 1.0);
             year = (long) (C - 4716.0);
          }
          else 
          {
             month = (long) (E - 13.0);
             year = (long) (C - 4715.0);
          }

            FD = FD - floor(FD);
            FD = FD * 24.0;
            hour = (long) FD;
            FD = FD - floor(FD);
            FD = FD * 60.0;
            minute = (long) FD;
            FD = FD - floor(FD);
            second = FD * 60.0;

    }

    /* Fundamentals of Astrodynamics and Applications, 3rd edition, David A. Vallado,
     * Space Technology Library, Microcosm Press / Springer, Hawthorne, CA / New York, NY, 2007.
     * Section 3.4, Algorithm 12, p. 179 */
    void SimCoordinateTransformations::ECEF2LLA(double x, double y, double z, double& phi_gd, double& lambda, double& h_ellp)
    {
        sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Inputs: x = %12.4f, y = %12.4f, z = %12.4f",
            x, y, z);
        double r_I = x / SIM_CONSTANTS.R_plus;
        double r_J = y / SIM_CONSTANTS.R_plus;
        double r_K_sat = z / SIM_CONSTANTS.R_plus;

        sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Converted: r_I = %12.4f, r_J = %12.4f, r_K_sat = %12.4f",
            r_I, r_J, r_K_sat);

        double r_delta_sat, sin_alpha, cos_alpha, r, delta, r_delta, r_K, phi_gd_old, sin_phi_gd, C_plus, tan_phi_gd;
        double tolerance = 0.000000001;

        r = sqrt(r_I * r_I + r_J * r_J + r_K_sat * r_K_sat);
        r_delta_sat = sqrt(r_I * r_I + r_J * r_J);
        sin_alpha = r_J / r_delta_sat;
        cos_alpha = r_I / r_delta_sat;

        if (sin_alpha >= 0) 
        { // 1st or 2nd quadrant, 0 <= alpha <= PI
            lambda = acos(cos_alpha); // Result of acos is between 0 and PI
        } 
        else 
        { // 3rd or 4th quadrant, 0 -PI < alpha < PI
            lambda = asin(sin_alpha); // Result of asin is between -PI/2 and 0, so this is only correct if we are in 4th quadrant
            if (cos_alpha < 0) 
            { // 3rd quadrant, so we need to fix the result
                lambda = -1 * SIM_CONSTANTS.PI - lambda;
            }
        }
        lambda = lambda * 180.0 / SIM_CONSTANTS.PI; // convert to degrees for output

        delta = asin(r_K_sat / r);
        sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Fixed Computations: r_delta_sat = %12.4f (%12.4f m), alpha = %12.8f, delta = %12.8f",
            r_delta_sat, r_delta_sat * SIM_CONSTANTS.R_plus, lambda, delta * 180.0 / SIM_CONSTANTS.PI);


        phi_gd = delta;
        r_delta = r_delta_sat;
        r_K = r_K_sat;

        do {
            sin_phi_gd = sin(phi_gd);

            C_plus = 1 / sqrt(1 - SIM_CONSTANTS.e_plus * SIM_CONSTANTS.e_plus * sin_phi_gd * sin_phi_gd);
            tan_phi_gd = (r_K + C_plus * SIM_CONSTANTS.e_plus * SIM_CONSTANTS.e_plus * sin_phi_gd) / r_delta;

            phi_gd_old = phi_gd;
            phi_gd = atan(tan_phi_gd);

            sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Iteration: C_plus = %12.8f, phi_gd = %12.8f (phi_gd_old = %12.8f), phi_gd - phi_gd_old = %12.8f",
                C_plus, phi_gd * 180.0 / SIM_CONSTANTS.PI, phi_gd_old * 180.0 / SIM_CONSTANTS.PI, phi_gd - phi_gd_old);
        } while ((phi_gd - phi_gd_old) > tolerance);

        h_ellp = (r_delta / cos(phi_gd) - C_plus) * SIM_CONSTANTS.R_plus;

        phi_gd = phi_gd * 180.0 / SIM_CONSTANTS.PI; // convert to degrees for output

        sim_logger->trace("SimCoordinateTransformations::ECEF2LLA:  Outputs: lambda = %12.8f, phi_gd = %12.8f, h_ellp = %12.4f",
            lambda, phi_gd, h_ellp);
    }


    /*************************************************************************
     * Private helper methods
     *************************************************************************/


}
