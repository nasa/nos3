#include <generic_star_tracker_data_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(Generic_star_trackerDataProvider,"GENERIC_STAR_TRACKER_PROVIDER");

    extern ItcLogger::Logger *sim_logger;

    Generic_star_trackerDataProvider::Generic_star_trackerDataProvider(const boost::property_tree::ptree& config) : SimIDataProvider(config)
    {
        sim_logger->trace("Generic_star_trackerDataProvider::Generic_star_trackerDataProvider:  Constructor executed");
        _request_count = 0;
    }

    boost::shared_ptr<SimIDataPoint> Generic_star_trackerDataProvider::get_data_point(void) const
    {
        sim_logger->trace("Generic_star_trackerDataProvider::get_data_point:  Executed");

        /* Prepare the provider data */
        _request_count++;

        /* Request a data point */
        SimIDataPoint *dp = new Generic_star_trackerDataPoint(_request_count);

        /* Return the data point */
        return boost::shared_ptr<SimIDataPoint>(dp);
    }
}
