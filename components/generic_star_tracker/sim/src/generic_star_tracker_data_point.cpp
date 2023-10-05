#include <ItcLogger/Logger.hpp>
#include <generic_star_tracker_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_star_trackerDataPoint::Generic_star_trackerDataPoint(double count)
    {
        sim_logger->trace("Generic_star_trackerDataPoint::Generic_star_trackerDataPoint:  Defined Constructor executed");

        /* Do calculations based on provided data */
        _generic_star_tracker_data_is_valid = true;
        _generic_star_tracker_data[0] = count * 0.001;
        _generic_star_tracker_data[1] = count * 0.002;
        _generic_star_tracker_data[2] = count * 0.003;
    }

    Generic_star_trackerDataPoint::Generic_star_trackerDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp) : _dp(*dp), _sc(spacecraft)
    {
        sim_logger->trace("Generic_star_trackerDataPoint::Generic_star_trackerDataPoint:  42 Constructor executed");

        /* Initialize data */
        _generic_star_tracker_data_is_valid = false;
        _generic_star_tracker_data[0] = _generic_star_tracker_data[1] = _generic_star_tracker_data[2] = 0.0;
    }
    
    void Generic_star_trackerDataPoint::do_parsing(void) const
    {
        try {
            /*
            ** Declare 42 telemetry string prefix
            ** 42 variables defined in `42/Include/42types.h`
            ** 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
            */
            std::string key;
            key.append("SC[").append(std::to_string(_sc)).append("].svb"); // SC[N].svb

            /* Parse 42 telemetry */
            std::string values = _dp.get_value_for_key(key);

            std::vector<double> data;
            parse_double_vector(values, data);

            _generic_star_tracker_data[0] = data[0];
            _generic_star_tracker_data[1] = data[1];
            _generic_star_tracker_data[2] = data[2];

            /* Mark data as valid */
            _generic_star_tracker_data_is_valid = true;

            _not_parsed = false;

            /* Debug print */
            sim_logger->trace("Generic_star_trackerDataPoint::Generic_star_trackerDataPoint:  Parsed svb = %f %f %f", _generic_star_tracker_data[0], _generic_star_tracker_data[1], _generic_star_tracker_data[2]);
        } catch (const std::exception &e) {
            sim_logger->error("Generic_star_trackerDataPoint::Generic_star_trackerDataPoint:  Error parsing svb.  Error=%s", e.what());
        }
    }

    /* Used for printing a representation of the data point */
    std::string Generic_star_trackerDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_star_trackerDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Generic_star_tracker Data Point:   Valid: ";
        ss << (_generic_star_tracker_data_is_valid ? "Valid" : "INVALID");
        ss << std::setprecision(std::numeric_limits<double>::digits10); /* Full double precision */
        ss << " Generic_star_tracker Data: "
           << _generic_star_tracker_data[0]
           << " "
           << _generic_star_tracker_data[1]
           << " "
           << _generic_star_tracker_data[2];

        return ss.str();
    }
} /* namespace Nos3 */
