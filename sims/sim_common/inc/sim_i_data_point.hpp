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

#ifndef NOS3_SIMDATAPOINT_HPP
#define NOS3_SIMDATAPOINT_HPP

#include <cstdint>
#include <string>

namespace Nos3
{

    /** \brief Class to contain a point of simulation data for a specific time.
     */
    class SimIDataPoint
    {
    public:
        /// @name Constructors
        //@{
        /** \brief Default constructor
         */
        SimIDataPoint() {};
        /** \brief Default destructor
         */
        virtual ~SimIDataPoint() {};
        //@}

        /// @name Accessors
        //@{
        /// \brief Returns a string representation of the simulation data point
        /// @return     A string representation of the simulation data point
        virtual std::string to_string(void) const = 0;
        //@}
    private:
        // Private data
    };

}

#endif

