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

#ifndef NOS3_SIMULATOR_TERMINAL_HPP
#define NOS3_SIMULATOR_TERMINAL_HPP

#include <iostream>
#include <thread>
#include <memory>
#include <stdexcept>

#include <ItcLogger/Logger.hpp>
#include <Client/Bus.hpp>
#include <Client/DataNode.hpp>
#include <Utility/Buffer.hpp>
#include <Common/BufferOverlay.hpp>
#include <I2C/Client/I2CMaster.hpp>
#include <Spi/Client/SpiMaster.hpp>
#include <Uart/Client/Uart.hpp>

#include <sim_hardware_model_factory.hpp>
#include <sim_i_hardware_model.hpp>
#include <sim_config.hpp>

#include <bus_connections.hpp>

namespace Nos3
{
    class SimTerminal : public SimIHardwareModel
    {
    public:
        // Constructors
        SimTerminal(const boost::property_tree::ptree& config);

        void run(void);
        void write_message_to_cout(const char* buf, size_t len);
        void write_message_to_cout(const NosEngine::Common::Message& msg);
        void print_prompt(void);

    private:
        // private types
        enum SimTerminalMode {HEX, ASCII};

        // private helper methods
        void command_callback(const NosEngine::Common::Message& msg);
        bool process_command(std::string input);
        void handle_input(void);
        void reset_bus_connection();
        
        std::string mode_as_string(void);
        std::string convert_hexhexchar_to_asciihexchars(uint8_t in);
        char convert_hexhexnibble_to_asciihexchar(uint8_t in);
        std::string convert_asciihex_to_hexhex(std::string in);
        uint8_t convert_asciihexcharpair_to_hexhexchar(char in1, char in2);
        uint8_t convert_asciihexchar_to_hexhexchar(char in);

        // Private data
        std::string _connection_string;
        std::string _current_sim_commanded_name;
        std::string _command_bus_name;
        std::unique_ptr<class BusConnection> _bus_connection;
        enum SimTerminalMode _current_in_mode;
        enum SimTerminalMode _current_out_mode;
    };
}

#endif