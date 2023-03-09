#include <generic_mag_hardware_model.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(Generic_magHardwareModel,"GENERIC_MAG");

    extern ItcLogger::Logger *sim_logger;

    const std::string Generic_magHardwareModel::_generic_mag_stream_name = "generic_mag_stream";

    Generic_magHardwareModel::Generic_magHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _stream_counter(0)
    {
        std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001"); // Get the NOS engine connection string, needed for the busses
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  NOS Engine connection string: %s.", connection_string.c_str());

        /* vvv 1. Get a data provider */
        /* !!! If your sim does not *need* a data provider, delete this block. */
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "GENERIC_MAG_PROVIDER");
        _generic_mag_dp = SimDataProviderFactory::Instance().Create(dp_name, config);
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  Data provider %s created.", dp_name.c_str());
        /* ^^^ 1. Get a data provider */

        /* vvv 2. Get on the computer bus */
        /* !!! This block is fine for UART.  If you use a different bus type, change this, but most of the structure will be similar. !!! */
        std::string bus_name = "usart_0"; // Initialize to default in case value not found in config file
        int node_port = 0;                // Initialize to default in case value not found in config file
        if (config.get_child_optional("simulator.hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) // Loop through the connections for *this* hw model
            {
                if (v.second.get("type", "").compare("usart") == 0) // v.second is the child tree (v.first is the name of the child)
                {
                    bus_name = v.second.get("bus-name", bus_name);
                    node_port = v.second.get("node-port", node_port);
                    break; // Found it... don't need to go through any more items
                }
            }
        }
        _uart_connection.reset(new NosEngine::Uart::Uart(_hub, config.get("simulator.name", "generic_mag_sim"), connection_string, bus_name));
        _uart_connection->open(node_port);
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  Now on UART bus name %s, port %d.", bus_name.c_str(), node_port);
        /* ^^^ 2. Get on the computer bus */
        /* vvv !!! User tip:  You should implement a read callback if you need to handle unsolicited byte messages on your bus and provide byte responses. !!! */
        _uart_connection->set_read_callback(std::bind(&Generic_magHardwareModel::uart_read_callback, this, std::placeholders::_1, std::placeholders::_2));

        /* vvv 3. Streaming data */
        /* !!! If your sim does not *stream* data, delete this entire block. */
        /* vvv !!! Add streaming data functions !!! USER TIP:  Add names and functions to stream data based on what your hardware can stream here */
        _streaming_data_function_map.insert(std::map<std::string, streaming_data_func>::value_type(_generic_mag_stream_name, &Generic_magHardwareModel::create_generic_mag_data));
        /* ^^^ !!! Add streaming data functions !!! USER TIP:  Add names and functions to stream data based on what your hardware can stream here */

        /* Which streaming data functions are initially enabled should be set in the config file... which will be processed here. !!! DO NOT CHANGE BELOW.  */
        if (config.get_child_optional("simulator.hardware-model.default-streams")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.default-streams")) // Loop through the default streams for *this* hw model
            {
                std::string stream_name = v.second.get("name", "");
                double initial_stream_time = v.second.get("initial-stream-time", 1.0); // Delta from start time to begin streaming
                std::uint32_t stream_period_ms = v.second.get("stream-period-ms", 1); // Time in milliseconds between streamed messages

                if ((_streaming_data_function_map.find(stream_name) != _streaming_data_function_map.end()) &&
                    (stream_period_ms > 0)) {
                    _periodic_streams.insert(
                        std::map<std::string, boost::tuple<double, double>>::value_type(
                            stream_name, boost::tuple<double, double>(_absolute_start_time + initial_stream_time, ((double)stream_period_ms)/1000.0)));

                    sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  Created default stream name %s starting at %f (start time + %f) with stream period %d milliseconds.", 
                        stream_name.c_str(), _absolute_start_time + initial_stream_time, initial_stream_time, stream_period_ms);
                } else {
                    sim_logger->error("Generic_magHardwareModel::Generic_magHardwareModel:  Invalid stream name %s or stream period (must be > 0) %d.", 
                        stream_name.c_str(), stream_period_ms);
                }
            }
        }

        std::string time_bus_name = "command"; // Initialize to default in case value not found in config file
        if (config.get_child_optional("hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections")) // Loop through the connections for *this* hw model
            {
                if (v.second.get("type", "").compare("time") == 0) // v.second is the child tree (v.first is the name of the child)
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    break; // Found it... don't need to go through any more items
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        _time_bus->add_time_tick_callback(std::bind(&Generic_magHardwareModel::send_streaming_data, this, std::placeholders::_1));
        sim_logger->info("Generic_magHardwareModel::Generic_magHardwareModel:  Now on time bus %s, executing callback to stream data.", time_bus_name.c_str());
        /* ^^^ 3. Streaming data */
    }

    // vvv Pretty standard... only change me if a different bus type is used and/or the data provider is not needed
    Generic_magHardwareModel::~Generic_magHardwareModel(void)
    {        
        // 1. Close the uart
        _uart_connection->close();

        // 2. Clean up the data provider we got
        delete _generic_mag_dp;
        _generic_mag_dp = nullptr;

        // 3. Don't need to clean up the time node, the bus will do it
    }

    // vvv Automagically set up by the base class to be called
    void Generic_magHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        // Here's how to get the data out of the message
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("Generic_magHardwareModel::command_callback:  Received command: %s.", dbf.data);

        // Do something with the data
        std::string command = dbf.data;
        std::string response = "Generic_magHardwareModel::command_callback:  INVALID COMMAND! (Try STOP GENERIC_MAG)";
        boost::to_upper(command);
        if (command.compare("STOP GENERIC_MAG") == 0) 
        {
            _keep_running = false;
            response = "Generic_magHardwareModel::command_callback:  STOPPING GENERIC_MAG";
        }
        // !!! USER TIP: Add anything additional to do with received data here

        // Here's how to send a reply
        _command_node->send_reply_message_async(msg, response.size(), response.c_str());
    }

    // vvv !!! Do not change me
    void Generic_magHardwareModel::send_streaming_data(NosEngine::Common::SimTime time)
    {
        const boost::shared_ptr<Generic_magDataPoint> data_point =
            boost::dynamic_pointer_cast<Generic_magDataPoint>(_generic_mag_dp->get_data_point());

        std::vector<uint8_t> data;

        double abs_time = _absolute_start_time + (double(time * _sim_microseconds_per_tick)) / 1000000.0;

        for (std::map<std::string, boost::tuple<double, double>>::iterator it = _periodic_streams.begin(); it != _periodic_streams.end(); it++) {
            boost::tuple<double, double> value = it->second;
            double prev_time = boost::tuples::get<0>(value);
            double period = boost::tuples::get<1>(value);
            double next_time = prev_time + period - (_sim_microseconds_per_tick / 1000000.0) / 2; // within half a tick time period
            if (next_time < abs_time) { // Time to send more data
                it->second = boost::tuple<double, double>(abs_time, period);
                std::map<std::string, streaming_data_func>::iterator search = _streaming_data_function_map.find(it->first);
                if (search != _streaming_data_function_map.end()) {
                    streaming_data_func f = search->second;
                    (this->*f)(*data_point, data);
                    sim_logger->debug("send_streaming_data:  Data point:  %s\n", data_point->to_string().c_str());
                    sim_logger->debug("send_streaming_data:  Writing data to UART:  %s\n", uint8_vector_to_hex_string(data).c_str());
                    _uart_connection->write(&data[0], data.size());
                }
            }
        }
    }

    // USER TIP:  This is your custom function to create some kind of data to send... you can have 1 or more of these functions... 
    // they can be called in response to a request, or periodically if streaming
    void Generic_magHardwareModel::create_generic_mag_data(const Generic_magDataPoint& data_point, std::vector<uint8_t>& out_data)
    {
        out_data.resize(14, 0x00);
        // Streaming data header - 0xDEAD
        out_data[0] = 0xDE;
        out_data[1] = 0xAD;
        // Set Payload - Counter
        _stream_counter++;
        out_data[2] = (_stream_counter >> 24) & 0x000000FF; 
        out_data[3] = (_stream_counter >> 16) & 0x000000FF; 
        out_data[4] = (_stream_counter >>  8) & 0x000000FF; 
        out_data[5] = _stream_counter & 0x000000FF;
        // Set Payload - Data

        // floating point numbers are **extremely** problematic (https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html),
        // and most hardware transmits some type of unsigned integer (e.g. from an ADC) anyway, 
        // so that's what this generic_mag is going to do... so scale each of the x, y, z (which are in the range [-1.0, 1.0]) by 32767
        // and add 32768 so that the result fits in a 16 bit unsigned integer... finally, we are going to model the hardware as sending
        // the bytes big endian (most significant byte first)
        // ... this is a good example of the type of thinking you need to do in the hardware model to make its byte interface behave
        // **just like** the real thing... most of the time you will have to **undo** (invert) the calculations the hardware spec says
        // to do to convert from raw units to engineering units

        // Another point how does your hardware behave if the dynamic/environmental data is not valid? 
        // ... you can check this value and make a decision:  data_point.is_generic_mag_data_valid()
        // ... in this case we are going to pretend that the hardware just pushes forward with whatever
        // it has and the computer on the other end has to deal with detecting invalid data

        uint16_t x   = (uint16_t)(data_point.get_generic_mag_data_x()*32767.0 + 32768.0);
        out_data[6]  = (x >> 8) & 0x00FF;
        out_data[7]  =  x       & 0x00FF;
        uint16_t y   = (uint16_t)(data_point.get_generic_mag_data_y()*32767.0 + 32768.0);
        out_data[8]  = (y >> 8) & 0x00FF;
        out_data[9]  =  y       & 0x00FF;
        uint16_t z   = (uint16_t)(data_point.get_generic_mag_data_z()*32767.0 + 32768.0);
        out_data[10] = (z >> 8) & 0x00FF;
        out_data[11] =  z       & 0x00FF;

        // Streaming data trailer - 0xBEEF
        out_data[12] = 0xBE;
        out_data[13] = 0xEF;
    }


    // USER TIP:  This is your custom function to do something when you receive unsolicited data from the UART
    void Generic_magHardwareModel::uart_read_callback(const uint8_t *buf, size_t len)
    {
        // Retrieve data and log received data in man readable format
        boost::shared_ptr<Generic_magDataPoint> data_point;
        std::vector<uint8_t> in_data(buf, buf + len);
        sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str());
        std::vector<uint8_t> out_data = in_data; // Initialize to just echo back what came in

        // Check if message is incorrect size
        if (in_data.size() != 13)
        {
            sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  Invalid command size of %d received!", in_data.size());
            return;
        }

        // Check header - 0xDEAD
        if ((in_data[0] != 0xDE) || (in_data[1] !=0xAD))
        {
            sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  Header incorrect!");
            return;
        }

        // Check trailer - 0xBEEF
        if ((in_data[11] != 0xBE) || (in_data[12] !=0xEF))
        {
            sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  Trailer incorrect!");
            return;
        }

        // Process command type
        switch (in_data[6])
        {
            case 1:
                sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  Send data command received!");
                data_point = boost::dynamic_pointer_cast<Generic_magDataPoint>(_generic_mag_dp->get_data_point());
                sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  Data point:  %s", data_point->to_string().c_str());
                create_generic_mag_data(*data_point, out_data); // Command not echoed back... actual generic_mag data is sent
                break;

            case 2:
                sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  Configuration command received!");
                if ((in_data[2] == _generic_mag_stream_name[0]) && 
                    (in_data[3] == _generic_mag_stream_name[1]) && 
                    (in_data[4] == _generic_mag_stream_name[2]) && 
                    (in_data[5] == _generic_mag_stream_name[3])) { 
                    // ... this is a good example of the type of thinking you need to do in the hardware model to make its byte interface behave
                    // **just like** the real thing... understand exactly what order the bytes come over the wire, what type they represent, and
                    // how to put them back together in the correct way to the correct type:
                    uint32_t millisecond_stream_delay = ((uint32_t)in_data[7] << 24) +
                                                        ((uint32_t)in_data[8] << 16) +
                                                        ((uint32_t)in_data[9] << 8 ) +
                                                        ((uint32_t)in_data[10]);
                    std::map<std::string, boost::tuple<double, double>>::iterator it = _periodic_streams.find(_generic_mag_stream_name);
                    if ((it != _periodic_streams.end()) &&
                        (millisecond_stream_delay > 0)) {
                        boost::get<1>(it->second) = ((double)millisecond_stream_delay)/1000.0;
                        sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  New millisecond stream delay for %s of %u", 
                            _generic_mag_stream_name.c_str(), millisecond_stream_delay);
                    } else {
                        sim_logger->error("Generic_magHardwareModel::uart_read_callback:  Stream %s was not set to be executed periodically or delay %u was not > 0",
                            _generic_mag_stream_name.c_str(), millisecond_stream_delay);
                        // zero out the response data... to indicate invalid request
                        in_data[ 7] = 0;
                        in_data[ 8] = 0;
                        in_data[ 9] = 0;
                        in_data[10] = 0;
                    }
                } else {
                    sim_logger->error("Generic_magHardwareModel::uart_read_callback:  Requested stream %c%c%c%c does not match prefix of %s", 
                        in_data[3], in_data[4], in_data[5], in_data[6], _generic_mag_stream_name.c_str());
                    // zero out the response data... to indicate invalid request
                    in_data[ 7] = 0;
                    in_data[ 8] = 0;
                    in_data[ 9] = 0;
                    in_data[10] = 0;
                }
                out_data = in_data; // Echo back what was actually configured
                break;

            case 3:
                sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  Other command received!");
                break;
            
            default:
                sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  Unused command received!");
                break;
        }

        // Log reply data in man readable format and ship the message bytes off
        sim_logger->debug("Generic_magHardwareModel::uart_read_callback:  REPLY %s",
            SimIHardwareModel::uint8_vector_to_hex_string(out_data).c_str());
        _uart_connection->write(&out_data[0], out_data.size());
    }
}
