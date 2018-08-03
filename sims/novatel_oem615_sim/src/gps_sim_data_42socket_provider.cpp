/* Copyright (C) 2015 - 2016 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

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

#include <ItcLogger/Logger.hpp>

#include <gps_sim_data_point.hpp>
#include <gps_sim_data_42socket_provider.hpp>

namespace Nos3
{
    REGISTER_DATA_PROVIDER(GPSSimData42SocketProvider,"GPS42SOCKET");

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/

    GPSSimData42SocketProvider::GPSSimData42SocketProvider(const boost::property_tree::ptree& config)
        : SimData42SocketProvider(config)
    {
        connect_reader_thread_as_42_socket_client(config.get("simulator.hardware-model.data-provider.hostname", "localhost"),
            config.get("simulator.hardware-model.data-provider.port", 4242));
    }

    /*************************************************************************
     * Non-mutating public worker methods
     *************************************************************************/

    boost::shared_ptr<SimIDataPoint> GPSSimData42SocketProvider::get_data_point(void) const
    {
        const boost::shared_ptr<Sim42DataPoint> dp42 =
            boost::dynamic_pointer_cast<Sim42DataPoint>(SimData42SocketProvider::get_data_point());

        SimIDataPoint *dp = new GPSSimDataPoint(dp42->get_abs_time(), dp42->get_gps_week(), dp42->get_gps_sec_week(), dp42->get_gps_frac_sec(),
            dp42->get_ECEF(), dp42->get_ECI(), dp42->get_DCM(), dp42->get_ECI_velocity());

        sim_logger->trace("GPSSimDataFileProvider::get_data_point: %s", dp->to_string().c_str()); // log data in a man readable format
        return boost::shared_ptr<SimIDataPoint>(dp);
    }

    /*************************************************************************
     * Private helper methods
     *************************************************************************/

}
