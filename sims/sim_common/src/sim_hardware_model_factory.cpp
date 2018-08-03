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

// See:  http://www.codeproject.com/Articles/751869/Abstract-Factory-Step-by-Step-Implementation-in-Cp

#include <ItcLogger/Logger.hpp>

#include <sim_hardware_model_factory.hpp>
#include <sim_i_hardware_model_maker.hpp>
#include <sim_i_hardware_model.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

	SimHardwareModelFactory& SimHardwareModelFactory::Instance()
	{
		// So called Meyers Singleton implementation,
		// In C++ 11 it is in fact thread-safe
		// In older versions you should ensure thread-safety here
		static SimHardwareModelFactory factory;
		return factory;
	}

	void SimHardwareModelFactory::RegisterMaker(const std::string& key, SimIHardwareModelMaker* maker)
	{
		// Validate uniquness and add to the map
		if (_makers.find(key) != _makers.end())
		{
            if (sim_logger != nullptr) 
			{
                sim_logger->warning("SimHardwareModelFactory::RegisterMaker:  Ignoring key.  Multiple hardware models for given key:  %s",
                    key.c_str());
            }
		} 
		else 
		{
            _makers[key] = maker;
            if (sim_logger != nullptr) 
			{
                sim_logger->info("SimHardwareModelFactory::RegisterMaker:  Registered hardware model for key %s", key.c_str());
            }
		}
	}

	SimIHardwareModel* SimHardwareModelFactory::Create(const std::string& key, const boost::property_tree::ptree& config) const
	{
		// Look up the maker by nodes name
		std::map<std::string, SimIHardwareModelMaker*>::const_iterator i = _makers.find(key);
		if (i == _makers.end())
		{
            sim_logger->fatal("SimHardwareModelFactory::Create:  Unrecognized hardware model key:  %s", key.c_str());
			throw new std::runtime_error("SimHardwareModelFactory::Create:  Unrecognized hardware model key:  " + key);
		}
		sim_logger->info("SimHardwareModelFactory::Create:  Creating hardware model for key %s", key.c_str());
		SimIHardwareModelMaker* maker = i->second;
		// Invoke create polymorphically
		return maker->Create(config);
	}
}
