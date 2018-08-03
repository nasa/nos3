/* Copyright (C) 2016 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <bar_data_provider.hpp>
#include <bar_data_point.hpp>

#include <ItcLogger/Logger.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(BarDataProvider,"BARPROVIDER");

    extern ItcLogger::Logger *sim_logger;

    BarDataProvider::BarDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("BarDataProvider::BarDataProvider:  Constructor executed");

        std::ostringstream oss;
        write_xml(oss, config);
        sim_logger->info("BarDataProvider::BarDataProvider:  "
            "configuration:\n%s", oss.str().c_str());

        sim_logger->trace("BarDataProvider::BarDataProvider:  Constructor exiting");
    }

    BarDataProvider::~BarDataProvider(void)
    {
        sim_logger->trace("BarDataProvider::~BarDataProvider:  Destructor executed");
    }

    boost::shared_ptr<SimIDataPoint> BarDataProvider::get_data_point(void) const
    {
        sim_logger->info("BarDataProvider::get_data_point:  Executed");

        BarDataPoint *msdp = new BarDataPoint();
        return boost::shared_ptr<SimIDataPoint>(msdp);
    }
}

