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

#ifndef NOS3_SIMCOORDINATETRANSFORMATIONS_HPP
#define NOS3_SIMCOORDINATETRANSFORMATIONS_HPP

#define SIM_LOGGER                "nos3.sim"

#include <cmath>

namespace Nos3
{

    /// \brief Class to provide some well known coordinate and time transformations
    class SimCoordinateTransformations
    {
    public:

        class SimConstants
        {
        public:
            SimConstants()
                :PI(4.0 * atan(1.0)),
                R_plus(6378136.3), // meters, JGM-3 model, Vallado, section 3.2, p. 140
                e_plus(0.081819221456) // Vallado, section 3.2, p. 140
            {
            }
            const double PI;
            const double R_plus;
            const double e_plus;

        };

        /// @name Constructors
        //@{
        //@}

        /// @name Static Methods
        //@{
        static void AbsTime2YMDHMS(double abs_time, long& year, long& month, long& day,
                                 long& hour, long& minute, double& second);
        static double AbsTimeToJD(double abs_time);
        static void JD2YMDHMS(double jd, long& year, long& month, long& day,
                                 long& hour, long& minute, double& second);

        static void ECEF2LLA(double x, double y, double z, double& latitude, double& longitude, double& altitude);

       //@}
    private:
        // Private helper methods

        // Private data
        static SimConstants SIM_CONSTANTS;

    };

}

#endif

