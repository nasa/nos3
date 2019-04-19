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

#include <simulator_terminal.hpp>

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

#include <boost/algorithm/string/find.hpp>

#include <bus_connections.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(SimTerminal,"SimTerminal");

    ItcLogger::Logger *sim_logger;


    // Constructors
    SimTerminal::SimTerminal(const boost::property_tree::ptree& config) : SimIHardwareModel(config),
        _current_sim_commanded_name(config.get("simulator.hardware-model.sim-commanded", "time")),
        _command_bus_name(config.get("simulator.hardware-model.start-bus", "command")),
        _current_in_mode((config.get("simulator.hardware-model.input-mode", "").compare("HEX") == 0) ? HEX : ASCII),
        _current_out_mode((config.get("simulator.hardware-model.output-mode", "").compare("HEX") == 0) ? HEX : ASCII)
    {
        _connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");
        _command_node_name = config.get("simulator.hardware-model.term-node-name", "terminal");
        if (config.get_child_optional("simulator.hardware-model.startup-commands")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.startup-commands")) 
            {
                process_command(v.second.data());
            }
        }

        reset_bus_connection();
    }

    /// @name Mutating public worker methods
    //@{
    /// \brief Runs the server, creating the NOS Engine bus and the transports for the simulator and simulator client to connect to.
    void SimTerminal::run(void)
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

    void SimTerminal::write_message_to_cout(const char* buf, size_t len){
        for (unsigned int i = 0; i < len; i++) {
            if (_current_out_mode == HEX) {
                std::cout << " 0x" << convert_hexhexchar_to_asciihexchars(buf[i]);
            } else {
                std::cout << buf[i];
            }
        }

        std::cout << std::endl;
    }

    void SimTerminal::write_message_to_cout(const NosEngine::Common::Message& msg)
    {
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        write_message_to_cout(dbf.data, dbf.len);
    }

    void SimTerminal::reset_bus_connection(){
         // Figure out where _old_connection lives and what data structure it belongs to
         if (_command_bus_name.find("i2c") != std::string::npos){
            int master_address;
            try{
                master_address = stoi(_command_node_name);
            }catch(std::invalid_argument e){
                master_address = 127;
                _command_node_name = "127";
                std::cout << "\"" << _command_node_name << "\" is not a valid I2C address for the terminal. Defaulting to 127." << std::endl;
            }
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
            
            I2CConnection* i2c = new I2CConnection(master_address, _connection_string, _command_bus_name);
            _bus_connection.reset(i2c);
        }else if(_command_bus_name.find("spi") != std::string::npos){
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
           _bus_connection.reset(new SPIConnection(_connection_string, _command_bus_name));
        }else if(_command_bus_name.find("uart") != std::string::npos || _command_bus_name.find("usart") != std::string::npos){
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
            _bus_connection.reset(new UartConnection(this, _command_node_name, _connection_string, _command_bus_name));
        }else{
            // if old connection exists, free it
            if (_bus_connection.get() != nullptr) {
                BusConnection* old = _bus_connection.release();
                delete old;
                // Nos3::sim_logger->debug("reset_bus_connection: deleting old bus connection");
            }
            _bus_connection.reset(new BaseConnection(this, _command_node_name, _connection_string, _command_bus_name));
        }
        _bus_connection->set_target(_current_sim_commanded_name);
    }

    bool SimTerminal::process_command(std::string input){
        std::string in_upper = input;
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
            std::cout << "    WRITE <data> - Writes <data> to the current node. Interprets <data> as ascii or hex depending on input setting." << std::endl;
            std::cout << "    READ <length> - Reads the given number of bytes from the current node. Only works on SPI and I2C buses." << std::endl;
            std::cout << "    TRANSACT <read length> <data> - Performs a transaction. Sends the given data, and expects a return value of the given length." << std::endl;
            std::cout << "             Interprets everything after the first space after <read length> as data to be written." << std::endl;
        } 
        else if (in_upper.compare(0, 12, "SET SIMNODE ") == 0) 
        {
            _current_sim_commanded_name = input.substr(12, input.size() - 12);
            _bus_connection->set_target(_current_sim_commanded_name);
        } 
        else if (in_upper.compare(0, 11, "SET SIMBUS ") == 0) 
        {

            std::string new_command_bus_name = input.substr(11, input.size() - 11);
            if(new_command_bus_name.compare(_command_bus_name) != 0){
                _command_bus_name = new_command_bus_name;
                reset_bus_connection();
            }else{
                std::cout << "Already on bus " << _command_bus_name << std::endl;
            }
        } 
        else if (in_upper.compare(0, 13, "SET TERMNODE ") == 0) 
        {
            _command_node_name = input.substr(13, input.length());
            reset_bus_connection();
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
            return true;
        }
        else if (in_upper.compare(0, 6, "WRITE ") == 0)
        {
            if(_bus_connection.get() == nullptr){
                std::cout << "Connection has not been instantiated. Connect to a bus with SET SIMBUS." << std::endl;
                return false;
            }
            std::string buf = input.substr(6, input.length() - 6);
            if(_current_in_mode == HEX){
                buf = convert_asciihex_to_hexhex(buf);
            }
            int wlen = buf.length();

            try{
                _bus_connection->write(buf.c_str(), wlen);
            }catch (std::runtime_error e){
                std::cout << e.what() << std::endl;
            }
            
        }
        else if (in_upper.compare(0, 5, "READ ") == 0)
        {
            if(_bus_connection.get() == nullptr){
                std::cout << "Connection has not been instantiated. Connect to a bus with SET SIMBUS." << std::endl;
                return false;
            }
            char buf[255];
            int len;
            std::string len_string = input.substr(5, input.length());
            try{
                len = stoi(len_string);
            }catch (std::invalid_argument e){
                len = 0;
            }

            try {
                _bus_connection->read(buf, len);
                write_message_to_cout(buf, len);
            }catch (std::runtime_error e){
                std::cout << e.what() << std::endl;
            }
            
        }
        else if (in_upper.compare(0, 9, "TRANSACT ") == 0)
        {
            if(_bus_connection.get() == nullptr){
                std::cout << "Connection has not been instantiated. Connect to a bus with SET SIMBUS." << std::endl;
                return false;
            }
            char rbuf[255];
            int rlen;
            int numberStart = 9;
            int dataStart;

            boost::iterator_range<std::string::iterator> r = boost::find_nth(input, " ", 1);
            dataStart = std::distance(input.begin(), r.begin()) + 1;

            try {
                rlen = stoi(input.substr(numberStart, dataStart - numberStart));
            }catch (std::invalid_argument){
                std::cout << "\"" << input.substr(numberStart, dataStart - numberStart) << "\" is not a valid number." << std::endl;
                return false;
            }

            //std::cout << "rlen: " << rlen << ", Data: " << input.substr(dataStart, input.length() - dataStart) << std::endl;

            std::string wbuf = input.substr(dataStart, input.length() - dataStart);
            if(_current_in_mode == HEX){
                wbuf = convert_asciihex_to_hexhex(wbuf);
            }
            int wlen = wbuf.length();
            try {
                _bus_connection->transact(wbuf.c_str(), wlen, rbuf, rlen);
                write_message_to_cout(rbuf, rlen);
            }catch (std::runtime_error e){
                std::cout << e.what() << std::endl;
            }
        }
        else if (input.length() > 0)
        {
            std::cout << "Unrecognized command. Type \"HELP\" for help." << std::endl;
        }
        return false;
    }

    void SimTerminal::handle_input(void)
    {
        std::string input, in_upper;
        std::cout << "This is the simulator terminal program.  Type 'HELP' for help." << std::endl << std::endl;
        print_prompt();
        while(std::getline(std::cin, input)) // keep looping and getting the next command line
        {
            bool result = process_command(input);
            if(result){
                break;
            }
            print_prompt();
        }

        std::cout << "SimTerminal is quitting!" << std::endl;
    }

    void SimTerminal::print_prompt(void)
    {
        std::cout << "SimTerminal:<" << _command_node_name << "@" << _command_bus_name
            << ">:Node:<" << _current_sim_commanded_name << ">:Mode:<" << mode_as_string() << "> $ ";
    }

    std::string SimTerminal::mode_as_string(void)
    {
        std::string mode;
        if (_current_in_mode == ASCII) mode.append("IN=ASCII:");
        if (_current_in_mode == HEX) mode.append("IN=HEX:");
        if (_current_out_mode == ASCII) mode.append("OUT=ASCII");
        if (_current_out_mode == HEX) mode.append("OUT=HEX");
        return mode;
    }

    std::string SimTerminal::convert_hexhexchar_to_asciihexchars(uint8_t in)
    {
        std::string out;
        uint8_t inupper = (in & 0xF0) >> 4;
        uint8_t inlower = in & 0x0F;
        out.push_back(convert_hexhexnibble_to_asciihexchar(inupper));
        out.push_back(convert_hexhexnibble_to_asciihexchar(inlower));
        return out;
    }

    char SimTerminal::convert_hexhexnibble_to_asciihexchar(uint8_t in)
    {
        char out = '.';
        if ((0x0 <= in) && (in <= 0x9)) out = in - 0x0 + '0';
        if ((0xA <= in) && (in <= 0xF)) out = in - 0xA + 'A';
        return out;
    }

    std::string SimTerminal::convert_asciihex_to_hexhex(std::string in)
    {
        std::string out;
        in.push_back('0'); // in case there are an odd number of characters, tack a 0 on the end
        for (size_t i = 0; i < in.size() - 1; i += 2) {
            out.push_back(convert_asciihexcharpair_to_hexhexchar(in[i], in[i+1]));
        }
        return out;
    }

    uint8_t SimTerminal::convert_asciihexcharpair_to_hexhexchar(char in1, char in2)
    {
        uint8_t outupper = convert_asciihexchar_to_hexhexchar(in1);
        uint8_t outlower = convert_asciihexchar_to_hexhexchar(in2);
        uint8_t out = ((outupper << 4) + outlower);
        return out;
    }

    uint8_t SimTerminal::convert_asciihexchar_to_hexhexchar(char in)
    {
        uint8_t out = 0;
        if (('0' <= in) && (in <= '9')) out = in - '0';
        if (('A' <= in) && (in <= 'F')) out = in - 'A' + 10;
        if (('a' <= in) && (in <= 'f')) out = in - 'a' + 10;
        return out;
    }

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
