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

#ifndef NOS3_GPSSIMHARDWAREMODELCOMMON_HPP
#define NOS3_GPSSIMHARDWAREMODELCOMMON_HPP

#include <cstdint>
#include <vector>

#include <sim_data_provider_factory.hpp>
#include <sim_i_hardware_model.hpp>

namespace Nos3
{
    /** \brief Class for things common to multiple GPS simulation hardware models, like the data provider
     *
     */
    class GPSSimHardwareModelCommon : public SimIHardwareModel
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  config  The configuration for the simulation
        GPSSimHardwareModelCommon(const boost::property_tree::ptree& config) : SimIHardwareModel(config)
        {
            std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GPS42SOCKET");
            _sim_data_provider = SimDataProviderFactory::Instance().Create(dp_name, config);
        }

        /// \brief Destructor.
        virtual ~GPSSimHardwareModelCommon()
        {
            delete _sim_data_provider;
            _sim_data_provider = nullptr;
        }
        //@}

        void run(void)
        {
            // Spin so the callbacks remain valid
            // TODO - Is this the best thing to do?
            while(1)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

    protected:
        // protected data
        SimIDataProvider* _sim_data_provider;
    };
}

#endif
