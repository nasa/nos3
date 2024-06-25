#include <newsim_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(NewsimDataProvider,"NEWSIM_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    NewsimDataProvider::NewsimDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("NewsimDataProvider::NewsimDataProvider:  Constructor executed");
        _request_count = 0;
    }

    boost::shared_ptr<SimIDataPoint> NewsimDataProvider::get_data_point(void) const
    {
        sim_logger->trace("NewsimDataProvider::get_data_point:  Executed");

        /* Prepare the provider data */
        _request_count++;

        /* Request a data point */
        SimIDataPoint *dp = new NewsimDataPoint(_request_count);

        /* Return the data point */
        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
