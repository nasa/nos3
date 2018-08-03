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

#include <sim_data_provider_factory.hpp>
#include <sim_i_data_provider_maker.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

	SimDataProviderFactory& SimDataProviderFactory::Instance()
	{
		// So called Meyers Singleton implementation,
		// In C++ 11 it is in fact thread-safe
		// In older versions you should ensure thread-safety here
		static SimDataProviderFactory factory;
		return factory;
	}

	void SimDataProviderFactory::RegisterMaker(const std::string& key, SimIDataProviderMaker* maker)
	{
		// Validate uniquness and add to the map
		if (_makers.find(key) != _makers.end())
		{
            if (sim_logger != nullptr) 
			{
                sim_logger->warning("SimDataProviderFactory::RegisterMaker:  Ignoring key.  Multiple data providers for given key:  %s",
                    key.c_str());
            }
		} 
		else 
		{
            _makers[key] = maker;
            if (sim_logger != nullptr) 
			{
                sim_logger->info("SimDataProviderFactory::RegisterMaker:  Registered data provider for key %s", key.c_str());
            }
		}
	}

    SimIDataProvider* SimDataProviderFactory::Create(const std::string& key, const boost::property_tree::ptree& config) const
	{
		// Look up the maker by nodes name
		std::map<std::string, SimIDataProviderMaker*>::const_iterator i = _makers.find(key);
		if (i == _makers.end())
		{
            sim_logger->fatal("SimDataProviderFactory::Create:  Unrecognized data provider key:  %s", key.c_str());
			throw new std::runtime_error("SimDataProviderFactory::Create:  Unrecognized data provider key:  " + key);
		}
		sim_logger->info("SimDataProviderFactory::Create:  Creating data provider for key %s", key.c_str());
		SimIDataProviderMaker* maker = i->second;
		// Invoke create polymorphically
		return maker->Create(config);
	}
}
