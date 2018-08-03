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

#ifndef NOS3_SIMIDATAPROVIDERMAKER_HPP
#define NOS3_SIMIDATAPROVIDERMAKER_HPP

#include <sim_config.hpp>

namespace Nos3
{
	class SimIDataProvider;

	// This class is a public parent of all data provider makers
	// It represents a function to be invoked when creating a data provider
	class SimIDataProviderMaker
	{
	public:
		/// Accepts ptree to pass into data provider constructor
		/// Returns data provider object
		virtual SimIDataProvider * Create(const boost::property_tree::ptree& config) const = 0;

		// Every C++ interface should define a public virtual destructor
		// Why? http://stackoverflow.com/questions/270917/why-should-i-declare-a-virtual-destructor-for-an-abstract-class-in-c
		virtual ~SimIDataProviderMaker() {}
	};
}

#endif
