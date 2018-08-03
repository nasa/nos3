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

#include <cam_data_provider.hpp>
#include <cam_data_point.hpp>

#include <ItcLogger/Logger.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(CamDataProvider,"CAMPROVIDER");

    extern ItcLogger::Logger *sim_logger;

    CamDataProvider::CamDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("CamDataProvider::CamDataProvider:  Constructor executed");

        //std::ostringstream oss;
        //write_xml(oss, config);
        //sim_logger->info("CamDataProvider::CamDataProvider:  "
        //    "configuration:\n%s", oss.str().c_str());

        sim_logger->trace("CamDataProvider::CamDataProvider:  Constructor exiting");
    }

    CamDataProvider::~CamDataProvider(void)
    {
        sim_logger->trace("CamDataProvider::~CamDataProvider:  Destructor executed");
    }

    boost::shared_ptr<SimIDataPoint> CamDataProvider::get_data_point(void) const
    {
        sim_logger->info("CamDataProvider::get_data_point:  Executed");

        CamDataPoint *msdp = new CamDataPoint();
        return boost::shared_ptr<SimIDataPoint>(msdp);
    }
}

