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

#ifndef NOS3_SIMIHARDWAREMODEL_HPP
#define NOS3_SIMIHARDWAREMODEL_HPP

#include <cstdint>
#include <vector>
#include <iomanip>

#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#include <ItcLogger/Logger.hpp>
#include <Common/BufferOverlay.hpp>
#include <Common/DataBufferOverlay.hpp>
#include <Common/Message.hpp>
#include <Client/Bus.hpp>
#include <Client/DataNode.hpp>

#include <sim_hardware_model_maker.hpp>
#define REGISTER_HARDWARE_MODEL(T,K) static Nos3::SimHardwareModelMaker<T> maker(K) // T = type, K = key
#include <sim_i_data_provider.hpp>
#include <sim_config.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    /** \brief Interface for a hardware model.
     *
     */
    class SimIHardwareModel
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  provider_key The name of the data provider
        /// @param  sc  The configuration for the simulation
        SimIHardwareModel(const boost::property_tree::ptree& config) :
            _absolute_start_time(config.get("common.absolute-start-time", 552110400.0)),
            _sim_microseconds_per_tick(config.get("common.sim-microseconds-per-tick", 1000000)),
            _command_bus(nullptr),
            _command_node(nullptr)
        {
            if (config.get_child_optional("simulator.hardware-model.connections")) 
            {
                BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) 
                {
                    // v.first is the name of the child.
                    // v.second is the child tree.
                    if (v.second.get("type", "").compare("command") == 0) 
                    {
                        // Set up the command node for this hardware model
                        _command_bus_name = v.second.get("bus-name", "command");
                        _command_node_name = v.second.get("node-name", "SimIHardwareModel");
                        _command_bus.reset(new NosEngine::Client::Bus(_hub, config.get("common.nos-connection-string", "tcp://127.0.0.1:12001"),
                            _command_bus_name));
                        _command_node = _command_bus->get_or_create_data_node(_command_node_name);
                        _command_node->set_message_received_callback(std::bind(&SimIHardwareModel::command_callback, this, std::placeholders::_1));
                        sim_logger->debug("SimIHardwareModel::SimIHardwareModel:  Command node %s now active on command bus %s.",
                            _command_node_name.c_str(), _command_bus_name.c_str());
                        break;
                    }
                }
            }
        }

        /// \brief Destructor.
        virtual ~SimIHardwareModel()
        {
            _command_bus.reset();
        }
        //@}

        /// @name Mutating public worker methods
        //@{

        /** \brief Method to run the hardware model simulation.
         */
        virtual void run(void) = 0;

        /** \brief Method to determine what to do with a command to the simulator received on the command bus.  The default is to do nothing.
         *
         * @param       msg         The NOS Engine message sent with the command.
         */
        virtual void command_callback(NosEngine::Common::Message msg)
        {
            // default is no command handling... override me!!
            NosEngine::Common::DataBufferOverlay dbf(const_cast<NosEngine::Utility::Buffer&>(msg.buffer));
            sim_logger->debug("SimIHardwareModel::command_callback:  Received command: %s.  Doing nothing and returning UNIMPLEMENTED!", dbf.data);
            _command_node->send_reply_message(msg, 14, "UNIMPLEMENTED!");
        }
        //@}

        /// @name Non-mutating public worker methods
        //@{
        /** \brief Method to convert a vector of uint8_t to an ASCII hex string.
         *
         * @param       v   The buffer (vector) of bytes to be converted.
         * @return          The string with the converted bytes.
         */
        static std::string  uint8_vector_to_hex_string(const std::vector<uint8_t> & v)
        {
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            std::vector<uint8_t>::const_iterator it;

            for (it = v.begin(); it != v.end(); it++) 
            {
                ss << " 0x" << std::setw(2) << static_cast<unsigned>(*it);
            }

            return ss.str();
        };
        //@}
    protected:
        // Protected data
        const double                                 _absolute_start_time;
        const int64_t                                _sim_microseconds_per_tick;
        NosEngine::Transport::TransportHub           _hub;
        std::string                                  _command_bus_name;
        std::string                                  _command_node_name;
        std::unique_ptr<NosEngine::Client::Bus>      _command_bus;
        NosEngine::Client::DataNode*                 _command_node;
    };
}

#endif
