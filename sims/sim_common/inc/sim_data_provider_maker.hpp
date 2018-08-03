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

#ifndef NOS3_SIMDATAPROVIDERMAKER_HPP
#define NOS3_SIMDATAPROVIDERMAKER_HPP

#include <sim_i_data_provider_maker.hpp>
#include <sim_data_provider_factory.hpp>
#include <sim_config.hpp>

namespace Nos3
{
	/// Helper template to simplify the process of generating data provider maker
	template<typename T>
	class SimDataProviderMaker : public SimIDataProviderMaker
	{
	public:
		/// When created, the data provider maker will automaticly register itself with the factory
		/// Note - you are discouraged from using SimDataProviderMaker outside REGISTER_DATA_PROVIDER macro
		/// For example, creating SimDataProviderMaker on the stack will end up badly
		SimDataProviderMaker(const std::string& key)
		{
			SimDataProviderFactory::Instance().RegisterMaker(key, this);
		}

		virtual SimIDataProvider * Create(const boost::property_tree::ptree& config) const
		{
			// Create instance of T using constructor from ptree
			// Assumes T has a constructor that accepts ptree
			return new T(config);
		}
	};

}

#endif
