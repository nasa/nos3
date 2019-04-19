#include <bus_connections.hpp>
#include <sstream>
#include <cstring>

namespace Nos3 {

    void BusConnection::set_target(std::string target){
        _target = target;
    }

    I2CConnection::I2CConnection(int master_address, std::string connection_string, std::string bus_name){
        //set_target(target);
        //std::cout << "Master address: " << master_address << std::endl;
        //std::cout << "Connection string: " << connection_string << std::endl;
        _i2c.reset(new NosEngine::I2C::I2CMaster(master_address, connection_string, bus_name));
    }

    I2CConnection::~I2CConnection() {
        Nos3::sim_logger->debug("I2CConnection: deleting old i2c Handle");
        NosEngine::I2C::I2CMaster* old = _i2c.release();
        delete old;
    }

    void I2CConnection::write(const char* buf, size_t len){
        try{
            int address = stoi(_target);
            _i2c->i2c_write(address, reinterpret_cast<const uint8_t*>(buf), len);
            std::cout << "Wrote " << len << " bytes to I2C address " << address << std::endl;
        }catch(std::invalid_argument e){
            std::stringstream ss;
            ss << "Error: \"" << _target << "\" is not a valid I2C address. To select an address, use SET SIMNODE.";
            throw std::runtime_error(ss.str());
        }
    }

    void I2CConnection::read(char* buf, size_t len){
        if(len <= 0){
            throw std::runtime_error("Error: Length must be greater than zero.");
        }
        try {
            int address = stoi(_target);
            printf("Result: %d\n", _i2c->i2c_read(address, reinterpret_cast<uint8_t*>(buf), len));
        }catch (std::invalid_argument e){
            std::stringstream ss;
            ss << "Error: \"" << _target << "\" is not a valid I2C address. To select an address, use SET SIMNODE.";
            throw std::runtime_error(ss.str());
        }
    }

    void I2CConnection::transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen){
        if(rlen <= 0){
            throw std::runtime_error("Error: Length must be greater than zero.");
        }
        try {
            int address = stoi(_target);
            _i2c->i2c_transaction(address, reinterpret_cast<const uint8_t*>(wbuf), wlen, reinterpret_cast<uint8_t*>(rbuf), rlen);
        }catch (std::invalid_argument e){
            std::stringstream ss;
            ss << "Error: \"" << _target << "\" is not a valid I2C address. To select an address, use SET SIMNODE.";
            throw std::runtime_error(ss.str());
        }
    }

    SPIConnection::SPIConnection(std::string connection_string, std::string bus_name){
        _spi.reset(new NosEngine::Spi::SpiMaster(connection_string, bus_name));
    }
    
    SPIConnection::~SPIConnection() {
        Nos3::sim_logger->debug("SPIConnection: deleting old spi Handle");
        NosEngine::Spi::SpiMaster* old = _spi.release();
        delete old;
    }

    void SPIConnection::write(const char* buf, size_t len){
        try {
            int select = stoi(_target);
            _spi->select_chip(select);
            _spi->spi_write(reinterpret_cast<const uint8_t*>(buf), len);
            _spi->unselect_chip();
            std::cout << "Wrote " << len << " bytes to SPI device " << select << std::endl;
        }catch (std::invalid_argument e){
            std::stringstream ss;
            ss << "Error: \"" << _target << "\" is not a valid select line. Must be a number.";
            throw std::runtime_error(ss.str());
        }
    }

    void SPIConnection::read(char* buf, size_t len){
        try {
            int select = stoi(_target);
            _spi->select_chip(select);
            _spi->spi_read(reinterpret_cast<uint8_t*>(buf), len);
            _spi->unselect_chip();
        }catch (std::invalid_argument e){
            std::stringstream ss;
            ss << "Error: \"" << _target << "\" is not a valid select line. Must be a number.";
            throw std::runtime_error(ss.str());
        }
    }

    void SPIConnection::transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen){
        try {
            int select = stoi(_target);
            _spi->select_chip(select);
            _spi->spi_transaction(reinterpret_cast<const uint8_t*>(wbuf), wlen, reinterpret_cast<uint8_t*>(rbuf), rlen);
            _spi->unselect_chip();
        }catch (std::invalid_argument e){
            std::stringstream ss;
            ss << "Error: \"" << _target << "\" is not a valid select line. Must be a number.";
            throw std::runtime_error(ss.str());
        }
    }

    UartConnection::UartConnection(SimTerminal* terminal, std::string node_name, std::string connection_string, std::string bus_name){
        _uart.reset(new NosEngine::Uart::Uart(node_name, connection_string, bus_name));
        _terminal = terminal;
        _uart->set_read_callback([this, bus_name](const uint8_t* const buf, size_t len, void* user){
            std::cout << std::endl << "Received a UART message on bus " << bus_name << ": " << std::endl;
            _terminal->write_message_to_cout(reinterpret_cast<const char*>(buf), len);
        });
    }

    UartConnection::~UartConnection() {
        Nos3::sim_logger->debug("UartConnection: deleting old uart Handle");
        NosEngine::Uart::Uart* old = _uart.release();
        delete old;
    }

    void UartConnection::write(const char* buf, size_t len){
        try {
            int port = stoi(_target);
            _uart->open(port);
            _uart->write(reinterpret_cast<const uint8_t*>(buf), len);
            _uart->close();
        }catch (std::invalid_argument e){
            std::stringstream ss;
            ss << "Error: \"" << _target << "\" is not a valid UART port. Must be a number.";
            throw std::runtime_error(ss.str());
        }
    }

    void UartConnection::read(char* buf, size_t len){
        if(_uart->available() > 0){
            throw std::runtime_error("I haven't implemented this yet.");
        }else{
            throw std::runtime_error("There are no bytes available to read.");
        }
    }

    void UartConnection::transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen){
        throw std::runtime_error("Error: Cannot perform transactions on UART bus.");
    }

    BaseConnection::BaseConnection(SimTerminal* terminal, std::string node_name, std::string connection_string, std::string bus_name){
        _bus.reset(new NosEngine::Client::Bus(connection_string, bus_name));
        _node = _bus->get_or_create_data_node(node_name);
        _terminal = terminal;
        _node->set_message_received_callback([this](NosEngine::Common::Message message) {
            std::cout << std::endl << "Received a message from " << message.source << ": " << std::endl; 
            _terminal->write_message_to_cout(message);
        });
        std::cout << "Connected to standard bus." << std::endl;
    }

    BaseConnection::~BaseConnection() {
        Nos3::sim_logger->debug("BaseConnection: deleting old base connection Handle");
        NosEngine::Client::Bus* old = _bus.release();
        delete old;
    }

    void BaseConnection::write(const char* buf, size_t len){
        _node->send_message(_target, len, buf);
    }

    void BaseConnection::read(char* buf, size_t len){
        throw std::runtime_error("Error: Cannot read from a normal bus.");
    }

    void BaseConnection::transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen){
        try{
            NosEngine::Common::Message msg = _node->send_request_message(_target, wlen, wbuf, 5000);
            NosEngine::Common::DataBufferOverlay dbf(msg.buffer);
            if(dbf.len < rlen){
                std::memcpy(rbuf, dbf.data, dbf.len);
                std::memset(rbuf + dbf.len, 0, rlen - dbf.len);
            }else{
                std::memcpy(rbuf, dbf.data, rlen);
            }
        }catch(...){
            throw std::runtime_error("Error while sending request message. The transaction may have timed out before a response was received.");
        }
        
    }
}