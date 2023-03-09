#ifndef NOS3_GENERIC_MAGHARDWAREMODEL_HPP
#define NOS3_GENERIC_MAGHARDWAREMODEL_HPP

#include <map>

#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>

#include <Client/Bus.hpp>
#include <Uart/Client/Uart.hpp>

#include <sim_i_data_provider.hpp>
#include <generic_mag_data_point.hpp>
#include <sim_i_hardware_model.hpp>

namespace Nos3
{
    // vvv This is pretty standard for a hardware model
    class Generic_magHardwareModel : public SimIHardwareModel
    {
    public:
        // Constructors / destructor
        Generic_magHardwareModel(const boost::property_tree::ptree& config);
        ~Generic_magHardwareModel(void);

    private:
        // Private helper methods
        void uart_read_callback(const uint8_t *buf, size_t len); // This guy handles unsolicited bytes the hardware receives from its peripheral bus
        void send_streaming_data(NosEngine::Common::SimTime time); // This guy provides an example of how to send unsolicited streaming data
        void create_generic_mag_data(const Generic_magDataPoint& data_point, std::vector<uint8_t>& out_data); // This guy creates data to send from a data point
        void command_callback(NosEngine::Common::Message msg);  // This guy handles out of band commands to the sim on the command bus

        // Private data members
        std::unique_ptr<NosEngine::Uart::Uart>              _uart_connection; // Change me if your peripheral bus is different (e.g. SPI, I2C, etc.)
        std::unique_ptr<NosEngine::Client::Bus>             _time_bus; // Very standard

        SimIDataProvider*                                   _generic_mag_dp; // I'm only needed if the sim actually has/needs a data provider

        // vvv Standard maps needed to set up streaming
        typedef void (Generic_magHardwareModel::*streaming_data_func)(const Generic_magDataPoint&, std::vector<uint8_t>&); // Convenience pointer to function typedef
        std::map<std::string, streaming_data_func>          _streaming_data_function_map; // stream name, function to call to generate data for that stream
        std::map<std::string, boost::tuple<double, double>> _periodic_streams; // stream name, (last absolute time function was called, period (seconds) to call function)

        // vvv Internal state data... change me as appropriate for your hardware model
        std::uint32_t                                       _stream_counter; // Used in this example to keep some internal state to report during streaming
        static const std::string                            _generic_mag_stream_name; // Used in this example to validate commands sent over UART to this hardware model sim
    };
}

#endif
