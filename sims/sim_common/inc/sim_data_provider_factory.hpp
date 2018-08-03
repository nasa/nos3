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

#ifndef NOS3_SIMDATAPROVIDERFACTORY_HPP
#define NOS3_SIMDATAPROVIDERFACTORY_HPP

#include <map>

#include <sim_i_data_provider_maker.hpp>
#include <sim_config.hpp>

namespace Nos3
{
	/// Abstract-Factory Pattern Implementation
	class SimDataProviderFactory
	{
	public:
		/// Factory is implemented as a Singleton
		static SimDataProviderFactory& Instance();

		/// Adds data provider maker with given key
		void RegisterMaker(const std::string& key, SimIDataProviderMaker * maker);

		/// Creates data provider for the given key from sim config
		SimIDataProvider * Create(const std::string& key, const boost::property_tree::ptree& config) const;

	private:
		SimDataProviderFactory() {}

		// Disable copying and assignment
		SimDataProviderFactory(const SimDataProviderFactory& other);
		SimDataProviderFactory& operator=(const SimDataProviderFactory& other);

		/// Maps keys to makers
		/// Note: using either the map or string makes our code not binary-compatible
		/// Memory layout and implementation of these classes will change from compiler to compiler
		std::map<std::string, SimIDataProviderMaker*> _makers;
	};
}

#endif
