#ifndef NOS3_GENERIC_MAGDATAPOINT_HPP
#define NOS3_GENERIC_MAGDATAPOINT_HPP

#include <boost/shared_ptr.hpp>

#include <sim_42data_point.hpp>

namespace Nos3
{
    // vvv This is pretty standard for a data point (if one is needed to transfer data between a data provider and a hardware model for your sim)
    class Generic_magDataPoint : public SimIDataPoint
    {
    public:
        // Constructor
        Generic_magDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp);

        // Accessors
        std::string to_string(void) const;
        // vvv These provide the hardware model a way to get the specific data out of the data point that it will need to send
        // out bytes over its peripheral bus
        double      get_generic_mag_data_x(void) const {return _generic_mag_data[0];}
        double      get_generic_mag_data_y(void) const {return _generic_mag_data[1];}
        double      get_generic_mag_data_z(void) const {return _generic_mag_data[2];}
        bool        is_generic_mag_data_valid(void) const {return _generic_mag_data_is_valid;}
    
    private:
        // Disallow the big 4
        Generic_magDataPoint(void) {};
        Generic_magDataPoint(const Generic_magDataPoint&) {};
        Generic_magDataPoint& operator=(const Generic_magDataPoint&) {};
        ~Generic_magDataPoint(void) {};

        // vvv This would be the specific data you need to get from the data provider to the hardware model so that
        // the hardware model can send out bytes over its peripheral bus... but you only get to this data through the accessors above
        mutable bool   _generic_mag_data_is_valid;
        mutable double _generic_mag_data[3];
    };
}

#endif
