/* Copyright (C) 2016 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <cam_hardware_model.hpp>
#include <cam_data_provider.hpp>

#include <ItcLogger/Logger.hpp>

#include <boost/property_tree/xml_parser.hpp>
#include <sys/stat.h>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(CamHardwareModel,"ARDUCAM_OV5640");

    extern ItcLogger::Logger *sim_logger;

    CamHardwareModel::CamHardwareModel(const boost::property_tree::ptree& config) : SimIHardwareModel(config), _keep_running(true)
    {
        sim_logger->trace("CamHardwareModel::CamHardwareModel:  Constructor executing");

        // Write out the config data passed
        //std::ostringstream oss;
        //write_xml(oss, config);
        //sim_logger->info("CamHardwareModel::CamHardwareModel:  "
        //    "configuration:\n%s", oss.str().c_str());

        // Time node
        std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");

        std::string time_bus_name = "command";        
        if (config.get_child_optional("hardware-model.connections")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections")) 
            {
                if (v.second.get("type", "").compare("time") == 0) 
                {
                    time_bus_name = v.second.get("bus-name", "command");
                    break;
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));

        // Initialize Register
        memset(spi_register, 0, sizeof(spi_register));

        // Connect to Science I2C Bus
        std::string i2c_bus_name = "i2c_2";
        int i2c_bus_address = 60; // 0x3C
        _i2c_slave_connection = new I2CSlaveConnection(this, i2c_bus_address, connection_string, i2c_bus_name);

        // Connect to SPI Bus
        std::string spi_bus_name = "spi_0";
        int chip_select = 0;
        _spi_slave_connection = new SpiSlaveConnection(this, chip_select, connection_string, spi_bus_name);

        // Here's how to get a data provider
        std::string dp_name = config.get("simulator.hardware-model.data-provider.type", "CAMPROVIDER");
        _sdp = SimDataProviderFactory::Instance().Create(dp_name, config);

        sim_logger->trace("CamHardwareModel::CamHardwareModel:  Time node, UART node, data provider created; constructor exiting");
    }

    CamHardwareModel::~CamHardwareModel(void)
    {
        sim_logger->trace("CamHardwareModel::CamHardwareModel:  Destructor executing");
        delete _sdp; // Clean up the data provider we got
        _time_bus.reset(); // Must reset the time bus so the unique pointer does not try to delete the hub.  Do not destroy the time node, the bus will do it
        
        // Clean up I2C
        delete _i2c_slave_connection;
        _i2c_slave_connection = nullptr;
        
        // Clean up SPI
        delete _spi_slave_connection;
        _spi_slave_connection = nullptr;

        // Clean up File Pointer
        if(fin.is_open())
        {
            fin.close();
        }
    }

    void CamHardwareModel::run(void)
    {
        int i = 0;
        boost::shared_ptr<SimIDataPoint> dp;
        while(_keep_running) 
        {
            sim_logger->info("CamHardwareModel::run:  Loop count %d, time %f", i++,
                _absolute_start_time + (double(_time_bus->get_time() * _sim_microseconds_per_tick)) / 1000000.0);
            dp = _sdp->get_data_point();
            sleep(5);
        }
    }

    std::uint8_t CamHardwareModel::determine_i2c_response_for_request(const std::vector<uint8_t>& in_data)
    {
        // Initialize local variables
        std::uint8_t out_data = 0x00;

        // Which register?
        switch (in_data[1])
        {             
            case 0x0A:
                out_data = 0x56;
                break;
            
            case 0x0B:
                out_data = 0x40;
                break;

            default:
                break;
        }

        return out_data;
    }

    std::uint16_t CamHardwareModel::determine_spi_response_for_request(const std::vector<uint8_t>& in_data)
    {
        // Initialize local variables
        std::uint16_t out_data = 0x0000;
        std::uint8_t reg = (in_data[0] & 0x7F);
        struct stat st;

        // Check write bit
        if ((in_data[0] & 0x80) == 0x80)
        {
            // Process register
            switch (reg)
            {   
                case 0x01: // Capture Control
                    // Number of frames to be captures
                    break;

                case 0x02: // Start capture
                    if(!fin.is_open())
                    {
                        fin.open("cam.bin", std::ios::binary | std::ios::in);
                        sim_logger->debug("Opening cam.bin");
                    }
                    fin.clear();
                    fin.seekg(0, std::ios::beg);
                    // Determine file size
                    if (stat("cam.bin", &st) != 0)
                    {
                        fifo_length = 0;
                        sim_logger->error("CamHardwareModel::determine_spi_response_for_request: ERROR - get fifo length failure!");
                    }
                    else
                    {
                        fifo_length = st.st_size;
                    };

                    break;

                case 0x03: // Sensor Interface Timing
                    break;

                case 0x04: // FIFO Control
                    break;

                case 0x05: // GPIO Direction
                    break;
                
                case 0x06: // GPIO Write
                    break;

                default:
                    break;
            }
            spi_register[reg] = in_data[1];            
        }

        // Set out data
        switch (reg)
        {
            case 0x3C:  // Burst FIFO Read
                sim_logger->error("CamHardwareModel::determine_spi_response_for_request: ERROR - burst FIFO read not supported!");
                break;

            case 0x3D:  // Single FIFO Read
                out_data = spi_register[reg] << 8;
                if (!fin.eof())
                {
                    fin.read(reinterpret_cast<char*>(&spi_register[reg]), 1);
                }
                break;

            case 0x40:  // ArduChip Version
                out_data = out_data | 0x4000; // 0x40 for 2MP Model
                break;

            case 0x41:  // Capture Done Flag
                out_data = out_data | 0x0800;
                break;

            // Reserved
            case 0x3B:
            case 0x3E:
            case 0x3F:
                sim_logger->error("ERROR - attempted access of reserved register!");
                break;

            case 0x42:  // Camera write FIFO size [7:0]
                out_data = fifo_length >> 8;
                break;
            case 0x43:  // Camera write FIFO size [15:8]
                out_data = fifo_length;
                break;
            case 0x44:  // Camera write FIFO size [18:16]
                out_data = fifo_length << 8;
                break;

            // TODO: Unimplemented
            case 0x02:
            
            case 0x45:  // GPIO Read Register

            default:
                out_data = (spi_register[reg] << 8) | spi_register[reg];
                break;
        }

        return out_data;
    }

    void CamHardwareModel::command_callback(NosEngine::Common::Message msg)
    {
        // Here's how to get the data out of the message
        NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
        sim_logger->info("CamHardwareModel::command_callback:  Received command: %s.", dbf.data);

        // Do something with the data
        std::string command = dbf.data;
        std::string response = "CamHardwareModel::command_callback:  INVALID COMMAND! (Try STOP CAMSIM)";
        boost::to_upper(command);
        if (command.compare("STOP CAMSIM") == 0) 
        {
            _keep_running = false;
            response = "CamHardwareModel::command_callback:  STOPPING CAMSIM";
        }

        // Here's how to send a reply
        _command_node->send_reply_message(msg, response.size(), response.c_str());
    }

    I2CSlaveConnection::I2CSlaveConnection(CamHardwareModel* hm,
        int bus_address, std::string connection_string, std::string bus_name)
        : NosEngine::I2C::I2CSlave(bus_address, connection_string, bus_name)
    {
        _hardware_model = hm;
    }

    size_t I2CSlaveConnection::i2c_read(uint8_t *rbuf, size_t rlen)
    {
        size_t num_read;
        sim_logger->debug("i2c_read: 0x%02x", _i2c_out_data); // log data
        if(rlen <= 1)
        {
            rbuf[0] = _i2c_out_data;
            num_read = 1;
        }
        return num_read;
    }

    size_t I2CSlaveConnection::i2c_write(const uint8_t *wbuf, size_t wlen)
    {
        std::vector<uint8_t> in_data(wbuf, wbuf + wlen);
        sim_logger->debug("i2c_write: %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str()); // log data
        _i2c_out_data = _hardware_model->determine_i2c_response_for_request(in_data);
        return wlen;
    }

    SpiSlaveConnection::SpiSlaveConnection(CamHardwareModel* hm,
        int chip_select, std::string connection_string, std::string bus_name)
        : NosEngine::Spi::SpiSlave(chip_select, connection_string, bus_name)
    {
        _hardware_model = hm;
    }

    size_t SpiSlaveConnection::spi_read(uint8_t *rbuf, size_t rlen)
    {     
        sim_logger->debug("spi_read: 0x%04x", _spi_out_data); // log data
        //sim_logger->debug("spi_read: rlen = 0x%02x", rlen);
        
        if(rlen <= 2)
        {
            rbuf[0] = (_spi_out_data & 0x00FF);
            rbuf[1] = (_spi_out_data & 0xFF00) >> 8;
        }

        //sim_logger->debug("spi_read: rbuf[0] = 0x%02x", rbuf[0]);
        //sim_logger->debug("spi_read: rbuf[1] = 0x%02x", rbuf[1]);
        return rlen;
    }

    size_t SpiSlaveConnection::spi_write(const uint8_t *wbuf, size_t wlen)
    {
        std::vector<uint8_t> in_data(wbuf, wbuf + wlen);
        sim_logger->debug("spi_write: %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str()); // log data
        _spi_out_data = _hardware_model->determine_spi_response_for_request(in_data);
        return wlen;
    }
}
