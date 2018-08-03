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

#include <iostream>
#include <thread>

#include <ItcLogger/Logger.hpp>
#include <Client/Bus.hpp>
#include <Client/DataNode.hpp>
#include <Utility/Buffer.hpp>
#include <Common/BufferOverlay.hpp>

#include <sim_hardware_model_factory.hpp>
#include <sim_i_hardware_model.hpp>
#include <sim_config.hpp>

namespace Nos3
{
    class SimTerminal;
    REGISTER_HARDWARE_MODEL(SimTerminal,"SimTerminal");

    ItcLogger::Logger *sim_logger;

    class SimTerminal : public SimIHardwareModel
    {
    public:
        // Constructors
        SimTerminal(const boost::property_tree::ptree& config) : SimIHardwareModel(config),
            _current_sim_commanded_name(config.get("simulator.hardware-model.sim-commanded", "time")),
            _current_in_mode((config.get("simulator.hardware-model.input-mode", "").compare("HEX") == 0) ? HEX : ASCII),
            _current_out_mode((config.get("simulator.hardware-model.output-mode", "").compare("HEX") == 0) ? HEX : ASCII)
        {
            _connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");
        }

        /// @name Mutating public worker methods
        //@{
        /// \brief Runs the server, creating the NOS Engine bus and the transports for the simulator and simulator client to connect to.
        void run(void)
        {
            try
            {
                handle_input(); // when handle_input returns... it is time to quit
            }
            catch(...)
            {
                Nos3::sim_logger->error("SimTerminal::run:  Exception caught!");
            }
        }
        //@}

    private:
        // private types
        enum SimTerminalMode {HEX, ASCII};

        // private helper methods
        void command_callback(const NosEngine::Common::Message& msg) // override method in base class
        {
            write_message_to_cout(msg);
        }

        void write_message_to_cout(const NosEngine::Common::Message& msg)
        {
            NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
            for (unsigned int i = 0; i < dbf.len; i++) {
                if (_current_out_mode == HEX) {
                    std::cout << " 0x" << convert_hexhexchar_to_asciihexchars(dbf.data[i]);
                } else {
                    std::cout << dbf.data[i];
                }
            }

            std::cout << std::endl;
        }

        void handle_input(void)
        {
            std::string input, in_upper;
            std::cout << "This is the simulator terminal program.  Type 'HELP' for help." << std::endl << std::endl;
            print_prompt();
            while(std::getline(std::cin, input)) // keep looping and getting the next command line
            {
                in_upper = input;
                boost::to_upper(in_upper);

                if (in_upper.compare(0, 4, "HELP") == 0) 
                {
                    std::cout << "This is help for the simulator terminal program." << std::endl;
                    std::cout << "  The prompt shows the <simulator terminal node name@simulator bus name> and <simulator node being commanded> " << std::endl;
                    std::cout << "  Commands:" << std::endl;
                    std::cout << "    HELP - Displays this help" << std::endl;
                    std::cout << "    QUIT - Exits the program" << std::endl;
                    std::cout << "    SET SIMNODE <sim node> - Sets the simulator node being commanded to '<sim node>'" << std::endl;
                    std::cout << "    SET SIMBUS <sim bus> - Sets the simulator bus for the simulator node being commanded to '<sim bus>'" << std::endl;
                    std::cout << "    SET TERMNODE <term node> - Sets the name of this terminal's node to '<term node>'" << std::endl;
                    std::cout << "    SET <ASCII|HEX> <IN|OUT> - Sets the terminal mode to ASCII mode or HEX mode; optionally IN or OUT only" << std::endl;
                    std::cout << "    All other commands are interpreted as commands to the currently set simulator node" << std::endl;
                } 
                else if (in_upper.compare(0, 12, "SET SIMNODE ") == 0) 
                {
                    _current_sim_commanded_name = input.substr(12, input.size() - 12);
                } 
                else if (in_upper.compare(0, 11, "SET SIMBUS ") == 0) 
                {
                        _command_bus_name = input.substr(11, input.size() - 11);
                        _command_bus.reset(new NosEngine::Client::Bus(_hub, _connection_string,
                            _command_bus_name));
                        _command_node = _command_bus->get_or_create_data_node(_command_node_name);
                        _command_node->set_message_received_callback(std::bind(&SimIHardwareModel::command_callback, this, std::placeholders::_1));
                } 
                else if (in_upper.compare(0, 13, "SET TERMNODE ") == 0) 
                {
                        _command_node_name = input.substr(13, input.size() - 13);
                        _command_bus.reset(new NosEngine::Client::Bus(_hub, _connection_string,
                            _command_bus_name));
                        _command_node = _command_bus->get_or_create_data_node(_command_node_name);
                        _command_node->set_message_received_callback(std::bind(&SimIHardwareModel::command_callback, this, std::placeholders::_1));
                } 
                else if (in_upper.compare(0, 9, "SET ASCII") == 0) 
                {
                    if ((input.size() == 9) || (in_upper.compare(0, 12, "SET ASCII IN") == 0)) _current_in_mode = ASCII;
                    if ((input.size() == 9) || (in_upper.compare(0, 13, "SET ASCII OUT") == 0)) _current_out_mode = ASCII;
                } 
                else if (in_upper.compare(0, 7, "SET HEX") == 0) 
                {
                    if ((input.size() == 7) || (in_upper.compare(0, 10, "SET HEX IN") == 0)) _current_in_mode = HEX;
                    if ((input.size() == 7) || (in_upper.compare(0, 11, "SET HEX OUT") == 0)) _current_out_mode = HEX;
                } 
                else if (in_upper.compare(0, 4, "QUIT") == 0) 
                {
                    break;
                } 
                else 
                {
                    std::string message = input;
                    if (_current_in_mode == HEX) 
                    {
                        message = convert_asciihex_to_hexhex(input);
                    }
                    if (_command_node != nullptr) 
                    {
                        NosEngine::Common::Message reply =
                            _command_node->send_request_message(_current_sim_commanded_name, message.size(), const_cast<char*>(message.c_str()));
                        write_message_to_cout(reply);
                    } 
                    else 
                    {
                        std::cout << "Error!  Not connected as a node to a bus!" << std::endl;
                    }
                }

                print_prompt();
            }

            std::cout << "SimTerminal is quitting!" << std::endl;
        }

        void print_prompt(void)
        {
            std::cout << "SimTerminal:<" << _command_node_name << "@" << _command_bus_name
                << ">:Node:<" << _current_sim_commanded_name << ">:Mode:<" << mode_as_string() << "> $ ";
        }

        std::string mode_as_string(void)
        {
            std::string mode;
            if (_current_in_mode == ASCII) mode.append("IN=ASCII:");
            if (_current_in_mode == HEX) mode.append("IN=HEX:");
            if (_current_out_mode == ASCII) mode.append("OUT=ASCII");
            if (_current_out_mode == HEX) mode.append("OUT=HEX");
            return mode;
        }

        std::string convert_hexhexchar_to_asciihexchars(uint8_t in)
        {
            std::string out;
            uint8_t inupper = (in & 0xF0) >> 4;
            uint8_t inlower = in & 0x0F;
            out.push_back(convert_hexhexnibble_to_asciihexchar(inupper));
            out.push_back(convert_hexhexnibble_to_asciihexchar(inlower));
            return out;
        }

        char convert_hexhexnibble_to_asciihexchar(uint8_t in)
        {
            char out = '.';
            if ((0x0 <= in) && (in <= 0x9)) out = in - 0x0 + '0';
            if ((0xA <= in) && (in <= 0xF)) out = in - 0xA + 'A';
            return out;
        }

        std::string convert_asciihex_to_hexhex(std::string in)
        {
            std::string out;
            in.push_back('0'); // in case there are an odd number of characters, tack a 0 on the end
            for (size_t i = 0; i < in.size() - 1; i += 2) {
                out.push_back(convert_asciihexcharpair_to_hexhexchar(in[i], in[i+1]));
            }
            return out;
        }

        uint8_t convert_asciihexcharpair_to_hexhexchar(char in1, char in2)
        {
            uint8_t outupper = convert_asciihexchar_to_hexhexchar(in1);
            uint8_t outlower = convert_asciihexchar_to_hexhexchar(in2);
            uint8_t out = ((outupper << 4) + outlower);
            return out;
        }

        uint8_t convert_asciihexchar_to_hexhexchar(char in)
        {
            uint8_t out = 0;
            if (('0' <= in) && (in <= '9')) out = in - '0';
            if (('A' <= in) && (in <= 'F')) out = in - 'A' + 10;
            if (('a' <= in) && (in <= 'f')) out = in - 'a' + 10;
            return out;
        }

        // Private data
        std::string _connection_string;
        std::string _current_sim_commanded_name;
        enum SimTerminalMode _current_in_mode;
        enum SimTerminalMode _current_out_mode;
    };
}

int
main(int argc, char *argv[])
{
    std::string simulator_name = "terminal"; // this is the ONLY terminal specific line!

    // Determine the configuration and run the simulator
    Nos3::SimConfig sc(argc, argv);
    Nos3::sim_logger->info("main:  %s simulator starting", simulator_name.c_str());
    sc.run_simulator(simulator_name);
    Nos3::sim_logger->info("main:  %s simulator terminating", simulator_name.c_str());
}
