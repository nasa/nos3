#ifndef NOS3_NEWSIMDATAPROVIDER_HPP
#define NOS3_NEWSIMDATAPROVIDER_HPP

#include <boost/property_tree/xml_parser.hpp>
#include <ItcLogger/Logger.hpp>
#include <newsim_data_point.hpp>
#include <sim_i_data_provider.hpp>

namespace Nos3
{
    class NewsimDataProvider : public SimIDataProvider
    {
    public:
        /* Constructors */
        NewsimDataProvider(const boost::property_tree::ptree& config);

        /* Accessors */
        boost::shared_ptr<SimIDataPoint> get_data_point(void) const;

    private:
        /* Disallow these */
        ~NewsimDataProvider(void) {};
        NewsimDataProvider& operator=(const NewsimDataProvider&) {return *this;};

        mutable double _request_count;
    };
}

#endif
