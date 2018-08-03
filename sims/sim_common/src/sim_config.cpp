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
#include <dlfcn.h>

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/filesystem.hpp>
#include <boost/exception/diagnostic_information.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <ItcLogger/Logger.hpp>

#include <sim_hardware_model_factory.hpp>
#include <sim_i_hardware_model.hpp>

#include <sim_config.hpp>

namespace Nos3
{

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/

    SimConfig::SimConfig(int argc, char *argv[])
    {
        parse_options(argc, argv);

        std::string log_config_file = _config.get("nos3-configuration.common.log-config-file", "sim_log_config.xml");
        if(boost::filesystem::exists(log_config_file)) // key should exist, but specify default just in case
        {
            ItcLogger::Logger::configure(log_config_file.c_str());
        }

        sim_logger = ItcLogger::Logger::get(SIM_LOGGER);
        //sim_logger->debug("SimConfig::SimConfig:  Constructing simulator configuration with %d arguments.", argc - 1);
        sim_logger->debug("SimConfig::SimConfig:  sim_logger is NOW valid.");
        //sim_logger->debug("SimConfig::SimConfig:  Configuration values:\n%s", to_string().c_str());
    }

    /*************************************************************************
     * Accessors
     *************************************************************************/

    void SimConfig::run_simulator(std::string simulator_name) const
    {
        sim_logger->debug("SimConfig::run_simulator:  SimConfig is created, logger is valid, and run_simulator is starting.");

        boost::property_tree::ptree config = get_config_for_simulator(simulator_name);
        if (config.get("simulator.active", false)) 
        {
            std::string model_type = config.get("simulator.hardware-model.type", "");

            // Create an instance of the simulator hardware model and run it
            std::unique_ptr<SimIHardwareModel> hardware_model(SimHardwareModelFactory::Instance().Create(model_type, config));
            hardware_model->run();
        } 
        else 
        {
            sim_logger->warning("SimConfig::run_simulator:  Simulator %s is not active.  Not running.", simulator_name.c_str());
        }
    }

    boost::property_tree::ptree SimConfig::get_config_for_simulator(std::string simulator_name) const
    {
        boost::property_tree::ptree sim_config;

        if (boost::optional<const boost::property_tree::ptree &> common = _config.get_child_optional("nos3-configuration.common"))
        {
            sim_config.add_child("common", common.get());
        }

        if (_config.get_child_optional("nos3-configuration.simulators")) 
        {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, _config.get_child("nos3-configuration.simulators")) 
            {
                // v.first is the name of the child.
                // v.second is the child tree.
                if (simulator_name.compare(v.second.get("name", "")) == 0) 
                {
                    sim_config.add_child("simulator", v.second);
                    std::string library = v.second.get("library", "");
                    if (library.compare("") == 0) 
                    {
                        library = "lib" + simulator_name + "_sim.so"; // guess a name
                    }
                    sim_logger->info("SimConfig::get_config_for_simulator:  Loading plug-in library %s", library.c_str());
                    if (dlopen(library.c_str(), RTLD_LAZY | RTLD_LOCAL) == NULL)
                    {   // Try to load the library so any plug-ins get registered
                        sim_logger->warning("SimConfig::get_config_for_simulator:  WARNING, did NOT load plug-in library %s.  Error: %s", library.c_str(), dlerror());
                    }
                    break;
                }
            }
        }

        if (sim_logger->is_level_enabled(ItcLogger::LOGGER_DEBUG)) 
        {
            sim_logger->debug("SimConfig::get_config_for_simulator:  Configuration for simulator %s is:\n", simulator_name.c_str());
            std::ostringstream oss;
            #if BOOST_VERSION / 100 % 1000 < 56
                write_xml(oss, sim_config, boost::property_tree::xml_writer_make_settings<char>(' ', 4));
            #else
                write_xml(oss, sim_config, boost::property_tree::xml_writer_make_settings<std::string>(' ', 4));
            #endif
            sim_logger->debug("\n%s", oss.str().c_str());
        }

        return sim_config;
    }

    boost::property_tree::ptree SimConfig::get_config(void) const
    {
        return _config;
    }

    std::string SimConfig::to_string(void) const
    {
        std::ostringstream oss;

        oss << "config-filename=" << _config_filename << std::endl;
        #if BOOST_VERSION / 100 % 1000 < 56
            write_xml(oss, _config, boost::property_tree::xml_writer_make_settings<char>(' ', 4));
        #else
            write_xml(oss, _config, boost::property_tree::xml_writer_make_settings<std::string>(' ', 4));
        #endif

        return oss.str();
    }

    /*************************************************************************
     * Private helper methods
     *************************************************************************/

    void SimConfig::parse_options(int argc, char *argv[])
    {
        try
        {
            // Generic options that can be on the command line
            boost::program_options::options_description generic("Generic options");
            generic.add_options()
            ("version,v", "print version string")
            ("help", "produce help message")
            ("config-file,f",
             boost::program_options::value<std::string>(&_config_filename)->
             default_value("nos3-simulator.xml"))
            ;

            // Options that can be on the command line or (more likely) in a configuration file
            boost::program_options::options_description config("Configuration");
            // Logging
            std::string cmd_line_log_config_filename;
            config.add_options()
            ("log-config-file,l",
             boost::program_options::value<std::string>(&cmd_line_log_config_filename)->
             default_value("")->
             composing(), "specify log configuration file name");

            // Ok, the option descriptions are created... now go get the options!
            boost::program_options::variables_map vm;
            boost::program_options::store(boost::program_options::parse_command_line(argc, argv, generic.add(config)), vm);
            boost::program_options::notify(vm);

            // Ok, that's all the options that can be specified on the command line... now go get any others (and all but the first one if they are
            // not specified on the command line) from a config file

            if(boost::filesystem::exists(_config_filename))
            {
                boost::property_tree::xml_parser::read_xml(_config_filename, _config, boost::property_tree::xml_parser::trim_whitespace);
            }

            // Set the common.log-config-file value in the ptree... order:  command line, config file, default
            std::string cfg_file_log_config_filename = _config.get("nos3-configuration.common.log-config-file", "");
            if (cmd_line_log_config_filename.compare("") != 0) 
            { // Use command line if specified
                _config.put("nos3-configuration.common.log-config-file", cmd_line_log_config_filename);
            } 
            else if (cfg_file_log_config_filename.compare("") == 0) 
            { // Use default if command line and config file not specified
                _config.put("nos3-configuration.common.log-config-file", "sim_log_config.xml");
            }

        }
        catch(boost::exception const &e)
        {
            std::cerr << "SimConfig::parse_options:  Error during option parsing prior to logger availability.  Error:  " <<
                      boost::diagnostic_information(e) << std::endl;
        }
        catch(std::exception e)
        {
            std::cerr << "SimConfig::parse_options:  Error during option parsing prior to logger availability.  Error:  " <<
                      e.what() << std::endl;
        }
        catch(...)
        {
            std::cerr << "SimConfig::parse_options:  Exception of unknown type." << std::endl;
        }
    }

}
