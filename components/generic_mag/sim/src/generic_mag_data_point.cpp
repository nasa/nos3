#include <ItcLogger/Logger.hpp>

#include <generic_mag_data_point.hpp>

namespace Nos3
{
    extern ItcLogger::Logger *sim_logger;

    Generic_magDataPoint::Generic_magDataPoint(int16_t spacecraft, const boost::shared_ptr<Sim42DataPoint> dp)
    {
        sim_logger->trace("Generic_magDataPoint::Generic_magDataPoint:  42 Constructor executed");

        _generic_mag_data_is_valid = false;
        // Force data to be set to a known value, which by the way... in this example, (0,0,0) is not valid for a unit vector
        _generic_mag_data[0] = 0.0;
        _generic_mag_data[1] = 0.0;
        _generic_mag_data[2] = 0.0;

        // Declare 42 telemetry string prefix
        // 42 variables defined in `42/Include/42types.h`
        // 42 data stream defined in `42/Source/IPC/SimWriteToSocket.c`
        std::ostringstream MatchString;
        MatchString << "SC[" << spacecraft << "].svb = "; // <<< Change me to match the data from 42 you are interested in
        size_t MSsize = MatchString.str().size();

        // Parse 42 telemetry
        std::vector<std::string> lines = dp->get_lines();
        try 
        {
            for (int i = 0; i < lines.size(); i++) 
            {
                // Compare prefix
                if (lines[i].compare(0, MSsize, MatchString.str()) == 0) 
                {
                    size_t found = lines[i].find_first_of("=");
                    // Parse line
                    std::istringstream iss(lines[i].substr(found+1, lines[i].size()-found-1));
                    // vvv Here is where you need to do some custom work to extract the data from the 42 string and save it off in the member data of this data point
                    std::string s;
                    iss >> s;
                    _generic_mag_data[0] = std::stod(s);
                    iss >> s;
                    _generic_mag_data[1] = std::stod(s);
                    iss >> s;
                    _generic_mag_data[2] = std::stod(s);

                    _generic_mag_data_is_valid = true;
                    
                    sim_logger->trace("Generic_magDataPoint::Generic_magDataPoint:  Parsed svb = %f %f %f", _generic_mag_data[0], _generic_mag_data[1], _generic_mag_data[2]);
                }
            }
        } 
        catch(const std::exception& e) 
        {
            _generic_mag_data_is_valid = false;
            // Force data to be set to a known value, which by the way... in this example, (0,0,0) is not valid for a unit vector
            _generic_mag_data[0] = 0.0;
            _generic_mag_data[1] = 0.0;
            _generic_mag_data[2] = 0.0;
            sim_logger->error("Generic_magDataPoint::Generic_magDataPoint:  Parsing exception %s", e.what());
        }
    }

    // Mainly used for printing a representation of the data point
    std::string Generic_magDataPoint::to_string(void) const
    {
        sim_logger->trace("Generic_magDataPoint::to_string:  Executed");
        
        std::stringstream ss;

        ss << std::fixed << std::setfill(' ');
        ss << "Generic_mag Data Point:   Valid: ";
        ss << (_generic_mag_data_is_valid ? "Valid" : "INVALID");
        ss << std::setprecision(std::numeric_limits<double>::digits10); // Full double precision
        ss << " Generic_mag Data: "
           << _generic_mag_data[0]
           << " "
           << _generic_mag_data[1]
           << " "
           << _generic_mag_data[2];

        return ss.str();
    }
}
