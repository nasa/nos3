#ifndef NOS3_BUS_CONNECTIONS_HPP
#define NOS3_BUS_CONNECTIONS_HPP

#include <memory>
#include <iostream>
#include <stdexcept>

#include <ItcLogger/Logger.hpp>
#include <Client/Bus.hpp>
#include <Client/DataNode.hpp>
#include <Utility/Buffer.hpp>
#include <Common/BufferOverlay.hpp>
#include <Common/DataBufferOverlay.hpp>
#include <Common/Message.hpp>
#include <I2C/Client/I2CMaster.hpp>
#include <Spi/Client/SpiMaster.hpp>
#include <Uart/Client/Uart.hpp>

#include <simulator_terminal.hpp>

namespace Nos3 {

    //TODO: Add transactions (Which will be send_request_message on the base node)

    class BusConnection {
    public:
        virtual ~BusConnection(void){};
        virtual void write(const char* buf, size_t len) = 0;
        virtual void read(char* buf, size_t len) = 0;
        virtual void transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen) = 0;
        void set_target(std::string target);
    protected:
        std::string _target;
    };

    class I2CConnection : public BusConnection {
    public:
        I2CConnection(int master_address, std::string connection_string, std::string bus_name);
        ~I2CConnection();
        void write(const char* buf, size_t len);
        void read(char* buf, size_t len);
        void transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen);
    private:
        std::unique_ptr<NosEngine::I2C::I2CMaster> _i2c;
    };

    class SPIConnection : public BusConnection {
    public:
        SPIConnection(std::string connection_string, std::string bus_name);
        ~SPIConnection();
        void write(const char* buf, size_t len);
        void read(char* buf, size_t len);
        void transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen);
    private:
        std::unique_ptr<NosEngine::Spi::SpiMaster> _spi;
    };

    class UartConnection : public BusConnection {
    public:
        UartConnection(class SimTerminal* terminal, std::string node_name, std::string connection_string, std::string bus_name);
        ~UartConnection();
        void write(const char* buf, size_t len);
        void read(char* buf, size_t len);
        void transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen);
    private:
        std::unique_ptr<NosEngine::Uart::Uart> _uart;
        class SimTerminal* _terminal;
    };

    class BaseConnection : public BusConnection {
    public:
        BaseConnection(class SimTerminal* terminal, std::string node_name, std::string connection_string, std::string bus_name);
        ~BaseConnection();
        void write(const char* buf, size_t len);
        void read(char* buf, size_t len);
        void transact(const char* wbuf, size_t wlen, char* rbuf, size_t rlen);
    private:
        std::unique_ptr<NosEngine::Client::Bus> _bus;
        NosEngine::Client::DataNode* _node;
        class SimTerminal* _terminal;
    };

}

#endif