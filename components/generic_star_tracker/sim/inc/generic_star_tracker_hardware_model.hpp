#ifndef NOS3_GENERIC_STAR_TRACKERHARDWAREMODEL_HPP
#define NOS3_GENERIC_STAR_TRACKERHARDWAREMODEL_HPP

/*
** Includes
*/
#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>

#include <Client/Bus.hpp>
#include <Uart/Client/Uart.hpp> /* TODO: Change if your protocol bus is different (e.g. SPI, I2C, etc.) */

#include <sim_i_data_provider.hpp>
#include <generic_star_tracker_data_point.hpp>
#include <sim_i_hardware_model.hpp>


/*
** Defines
*/
#define GENERIC_STAR_TRACKER_SIM_SUCCESS 0
#define GENERIC_STAR_TRACKER_SIM_ERROR   1


/*
** Namespace
*/
namespace Nos3
{
    /* Standard for a hardware model */
    class Generic_star_trackerHardwareModel : public SimIHardwareModel
    {
    public:
        /* Constructor and destructor */
        Generic_star_trackerHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_star_trackerHardwareModel(void);

    private:
        /* Private helper methods */
        void create_generic_star_tracker_hk(std::vector<uint8_t>& out_data); 
        void create_generic_star_tracker_data(std::vector<uint8_t>& out_data); 
        void uart_read_callback(const uint8_t *buf, size_t len); /* Handle data the hardware receives from its protocol bus */
        void command_callback(NosEngine::Common::Message msg); /* Handle backdoor commands and time tick to the simulator */

        /* Private data members */
        std::unique_ptr<NosEngine::Uart::Uart>              _uart_connection; /* TODO: Change if your protocol bus is different (e.g. SPI, I2C, etc.) */
        std::unique_ptr<NosEngine::Client::Bus>             _time_bus; /* Standard */

        SimIDataProvider*                                   _generic_star_tracker_dp; /* Only needed if the sim has a data provider */

        /* Internal state data */
        std::uint8_t                                        _enabled;
        std::uint32_t                                       _count;
        std::uint32_t                                       _config;
        std::uint32_t                                       _status;
    };
}

#endif
