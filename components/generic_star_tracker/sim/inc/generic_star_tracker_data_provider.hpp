#ifndef NOS3_GENERIC_STAR_TRACKERDATAPROVIDER_HPP
#define NOS3_GENERIC_STAR_TRACKERDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <generic_star_tracker_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class Generic_star_trackerDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        Generic_star_trackerDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~Generic_star_trackerDataProvider(void) {};
        Generic_star_trackerDataProvider& operator=(const Generic_star_trackerDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
