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

#include <fstream>

#include <boost/filesystem.hpp>

#include <ItcLogger/Logger.hpp>

#include <gps_sim_data_file_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(GPSSimDataFileProvider,"GPSFILE");

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/

    GPSSimDataFileProvider::GPSSimDataFileProvider(const boost::property_tree::ptree& config) :
        SimIDataProvider(config), _file_loc(0),
        _absolute_start_time(config.get("common.absolute-start-time", 552110400.0)),
        _sim_microseconds_per_tick(config.get("common.sim-microseconds-per-tick", 1000000)),
        _data_file(config.get("simulator.hardware-model.data-provider.filename", "gps_data.42"))
    {
        sim_logger->info("GPSSimDataFileProvider::GPSSimDataFileProvider:  Configuring GpsSimDataFileProvider.");

        // Set up the time node which is **required** for this data provider
        std::string bus_name = config.get("data-provider.connection.bus-name", "command");
        _time_bus.reset(new NosEngine::Client::Bus(_hub, config.get("common.nos-connection-string", "tcp://127.0.0.1:12001"), bus_name));
        sim_logger->debug("GPSSimDataFileProvider::GPSSimDataFileProvider:  Time bus %s now active.", bus_name.c_str());
    }

    /*************************************************************************
     * Non-mutating public worker methods
     *************************************************************************/

    boost::shared_ptr<SimIDataPoint> GPSSimDataFileProvider::get_data_point() const
    {
        //get_gps_data();
        //SimIDataPoint *dp = new GPSSimDataPoint(_data_point);
        return get_gps_data(); // boost::shared_ptr<SimIDataPoint>(dp);
    }

    /*************************************************************************
     * Private helper methods
     *************************************************************************/
    boost::shared_ptr<GPSSimDataPoint> GPSSimDataFileProvider::get_gps_data() const
    {
		int i, j;
        double j2000 = 0.0;
        int32_t gps_week;
        int32_t gps_sec_week;
        double gps_frac_sec;
        std::vector<double> ECI, vel, ECEF;
        ECEF.resize(3);
        ECI.resize(3);
        vel.resize(3);

		// Variables for data from 42 data file

		// File Stream for 42 data file
		std::ifstream filebuf;

		// File processing for 42 data file filebuf
		filebuf.open(_data_file);
		filebuf.seekg(_file_loc);

		double abs_time = _absolute_start_time + (double(_time_bus->get_time() * _sim_microseconds_per_tick)) / 1000000.0;

		while (j2000 < abs_time) {

			filebuf >> j2000;
			get_gps_time(j2000, gps_week, gps_sec_week, gps_frac_sec);

			for ( i = 0; i < 3; i++ ){
				filebuf >> ECI[i];
			}

			for ( i = 0; i < 3; i++ ){
				filebuf >> ECEF[i];
			}

			for ( i = 0; i < 3; i++ ){
				filebuf >> vel[i];
			}

			_file_loc = filebuf.tellg();
		}
		filebuf.close();

        GPSSimDataPoint* data_point =
            new GPSSimDataPoint(j2000, gps_week, gps_sec_week, gps_frac_sec, ECEF, ECI, vel);
        sim_logger->trace("GPSSimDataFileProvider::get_gps_data: %s", data_point->to_string().c_str());
		return boost::shared_ptr<GPSSimDataPoint>(data_point);
    }

	void GPSSimDataFileProvider::get_gps_time(double absTime,
        int32_t& gps_week, int32_t& gps_seconds_in_week, double& gps_fractions_of_a_second) const
	{
		gps_week = (int)floor(absTime / 604800);
		gps_seconds_in_week = (int)floor(absTime) - gps_week * 604800;
        gps_fractions_of_a_second = absTime - (gps_week * 604800) - gps_seconds_in_week;
	}

}
