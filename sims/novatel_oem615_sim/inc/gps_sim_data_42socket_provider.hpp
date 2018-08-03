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

#ifndef NOS3_GPSSIMDATA42SOCKETPROVIDER_HPP
#define NOS3_GPSSIMDATA42SOCKETPROVIDER_HPP

//#include <vector>

//#include <Client/Bus.hpp>

#include <sim_data_42socket_provider.hpp>

namespace Nos3
{
    /** \brief Class for a provider of GPS simulation data that provides data from a socket connection to 42.
     *
     */
    class GPSSimData42SocketProvider : public SimData42SocketProvider
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  sc  The configuration for the simulation
        GPSSimData42SocketProvider(const boost::property_tree::ptree& config);
        //@}

        /// @name Non-mutating public worker methods
        //@{
        /** \brief Method to retrieve GPS data.
         *
         * @returns                     A data point of GPS data.
         */
      virtual boost::shared_ptr<SimIDataPoint> get_data_point(void) const;
        //@}
    private:
        // Private helper methods
    };
}

#endif
