/* Copyright (C) 2009 - 2015 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

/* Standard Includes */
#include <thread>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <ItcLogger/Logger.hpp>

#include <Client/Bus.hpp>

#include <sim_hardware_model_factory.hpp>
#include <sim_i_hardware_model.hpp>
#include <sim_config.hpp>

namespace Nos3
{
    class TimeDriver;
    REGISTER_HARDWARE_MODEL(TimeDriver,"TimeDriver");

    ItcLogger::Logger *sim_logger;

    /** \brief Class to drive NOS time.
     *
     */
    class TimeDriver : public SimIHardwareModel
    {
    public:
        /*************************************************************************
         * Constructors / destructors
         *************************************************************************/

        TimeDriver(const boost::property_tree::ptree& config) : SimIHardwareModel(config),
            _active(config.get("simulator.active", true)),
            _real_microseconds_per_tick(config.get("simulator.hardware-model.real-microseconds-per-tick", 1000000)),
            _time_uri(config.get("common.nos-connection-string", "tcp://127.0.0.1:12001")),
            _time_bus_name("command"),
            _time_counter(0)
        {
            std::string type = config.get("simulator.hardware-model.type", "");

            if ((type.compare("") == 0) /* no type found in the config ok */ &&
                (type.compare("time-simulator") != 0) /* otherwise this has to be the type or die */) 
            {
                std::ostringstream oss;
                oss << "TimeDriver::TimeDriver:  Exception!  Incorrect simulator type "
                    << type << "; expected 'time-simulator'.";
                throw new std::runtime_error(oss.str());
            }

            if (_active) 
            {
                sim_logger->debug("TimeDriver::TimeDriver: Creating time sender\n");
                std::string node_name = "TimeDriver";

                // Use config data if it exists
                if (config.get_child_optional("simulator.hardware-model.connections")) 
                {
                    BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) 
                    {
                        std::ostringstream oss;
                        write_xml(oss, v.second);
                        sim_logger->trace("TimeDriver::TimeDriver - simulator.hardware-model.connections.connection subtree:\n%s", oss.str().c_str());

                        // v.first is the name of the child.
                        // v.second is the child tree.
                        if (v.second.get("type", "").compare("time") == 0) {
                            _time_bus_name = v.second.get("bus-name", _time_bus_name);
                            node_name = v.second.get("node-name", node_name);
                            break;
                        }
                    }
                }

                // Create the bus and node with defaults even if there is no config data... this node is the only reason for the time driver to exist
                _time_bus.reset(new NosEngine::Client::Bus(_hub, _time_uri, _time_bus_name));
                _time_bus->enable_set_time();

                sim_logger->debug("TimeDriver::TimeDriver: Time sender created!\n");
            }
        }

        /*************************************************************************
         * Mutating public worker methods
         *************************************************************************/

        void run(void)
        {
            if (_active) 
            {
                while (1) 
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(_real_microseconds_per_tick));

                    if(_time_bus->is_connected())
                    {
                        sim_logger->info("TimeDriver::send_tick_to_nos_engine: tick = %d, absolute time %f\n",
                            _time_counter, _absolute_start_time + (double(_time_counter * _sim_microseconds_per_tick)) / 1000000.0);

                        _time_bus->set_time(_time_counter++);
                    }
                    else
                    {
                        sim_logger->info("time bus disconnected... reconnecting");
                        _time_bus.reset(new NosEngine::Client::Bus(_hub, _time_uri, _time_bus_name));
                        _time_bus->enable_set_time();
                    }
                }
            } 
            else 
            {
                sim_logger->info("TimeDriver::run:  Time driver is not active");
            }
        }

    private:
        // Private data
        const bool                                     _active;
        const int64_t                                  _real_microseconds_per_tick;

        std::string                                    _time_uri;
        std::string                                    _time_bus_name;
        unsigned int                                   _time_counter;
        std::unique_ptr<NosEngine::Client::Bus>        _time_bus;
    };
}

int main(int argc, char *argv[])
{
    std::string simulator_name = "time"; // this is the ONLY time driver specific line!

    // Determine the configuration and run the simulator
    Nos3::SimConfig sc(argc, argv);
    Nos3::sim_logger->info("main:  %s simulator starting", simulator_name.c_str());
    sc.run_simulator(simulator_name);
    Nos3::sim_logger->info("main:  %s simulator terminating", simulator_name.c_str());
}
