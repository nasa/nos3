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

#ifndef NOS3_SIMHARDWAREMODELMAKER_HPP
#define NOS3_SIMHARDWAREMODELMAKER_HPP

#include <sim_i_hardware_model_maker.hpp>
#include <sim_hardware_model_factory.hpp>
#include <sim_config.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
	/// Helper template to simplify the process of generating data provider maker
	template<typename T>
	class SimHardwareModelMaker : public SimIHardwareModelMaker
	{
	public:
		/// When created, the hardware model maker will automaticly register itself with the factory
		/// Note - you are discouraged from using SimHardwareModelMaker outside REGISTER_DATA_PROVIDER macro
		/// For example, creating SimHardwareModelMaker on the stack will end up badly
		SimHardwareModelMaker(const std::string& key)
		{
			SimHardwareModelFactory::Instance().RegisterMaker(key, this);
		}

		virtual SimIHardwareModel * Create(const boost::property_tree::ptree& config) const
		{
			// Create instance of T using constructor from ptree
			// Assumes T has a constructor that accepts ptree
			return new T(config);
		}
	};

}

#endif
