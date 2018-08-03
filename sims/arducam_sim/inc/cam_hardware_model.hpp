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

#ifndef NOS3_CAMHARDWAREMODEL_HPP
#define NOS3_CAMHARDWAREMODEL_HPP

#include <sim_i_hardware_model.hpp>
#include <Client/Bus.hpp>

// Protocols
#include <I2C/Client/I2CSlave.hpp>
#include <Spi/Client/SpiSlave.hpp>

#include <atomic>
#include <fstream>

namespace Nos3
{
    class CamHardwareModel : public SimIHardwareModel
    {
    public:
        CamHardwareModel(const boost::property_tree::ptree& config);
        ~CamHardwareModel(void);
        void run(void);
        std::uint8_t determine_i2c_response_for_request(const std::vector<uint8_t>& in_data); 
        std::uint16_t determine_spi_response_for_request(const std::vector<uint8_t>& in_data); 
        void command_callback(NosEngine::Common::Message msg);
    private:
        std::atomic<bool>                       _keep_running;
        SimIDataProvider*                       _sdp;
        std::unique_ptr<NosEngine::Client::Bus> _time_bus;
        class I2CSlaveConnection*               _i2c_slave_connection;
        class SpiSlaveConnection*               _spi_slave_connection;
        std::uint8_t                            spi_register[69]; // 0x45
        std::ifstream                           fin;
        std::uint32_t                           fifo_length;
    };

    class I2CSlaveConnection : public NosEngine::I2C::I2CSlave
    {
    public:
        I2CSlaveConnection(CamHardwareModel* hm, int bus_address, std::string connection_string, std::string bus_name);
        size_t i2c_read(uint8_t *rbuf, size_t rlen);
        size_t i2c_write(const uint8_t *wbuf, size_t wlen);
    private:
        CamHardwareModel* _hardware_model;
        std::uint8_t _i2c_out_data;
    };

    class SpiSlaveConnection : public NosEngine::Spi::SpiSlave
    {
    public:
        SpiSlaveConnection(CamHardwareModel* hm, int chip_select, std::string connection_string, std::string bus_name);
        size_t spi_read(uint8_t *rbuf, size_t rlen);
        size_t spi_write(const uint8_t *wbuf, size_t wlen);
    private:
        CamHardwareModel* _hardware_model;
        std::uint16_t _spi_out_data;        
    };
}

#endif
