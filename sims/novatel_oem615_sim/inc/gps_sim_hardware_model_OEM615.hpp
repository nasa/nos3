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

#ifndef NOS3_GPSSIMHARDWAREMODELOEM615_HPP
#define NOS3_GPSSIMHARDWAREMODELOEM615_HPP

#include <cstdint>
#include <vector>
#include <map>
#include <boost/tuple/tuple.hpp>

#include <Uart/Client/Uart.hpp>

#include <GeodeticParameters.h>
#include <CoordinateSystemParameters.h>
#include <CoordinateConversionService.h>

#include <gps_sim_hardware_model_common.hpp>

namespace Nos3
{
    /** \brief Class for a NovAtel OEM615 GPS simulation hardware model.
     *
     *  http://www.novatel.com/products/gnss-receivers/oem-receiver-boards/oem6-receivers/oem615/
     *  http://www.novatel.com/assets/Documents/Manuals/om-20000128.pdf
     *  http://www.novatel.com/assets/Documents/Manuals/om-20000129.pdf
     */
    class GPSSimHardwareModelOEM615 : public GPSSimHardwareModelCommon
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  sim_data_provider The data provider to use to retrieve data
        /// @param  sc  The configuration for the simulation
        GPSSimHardwareModelOEM615(const boost::property_tree::ptree& config);
        /// \brief Destructor
        ~GPSSimHardwareModelOEM615(void);
        //@}

    private:
        // Private helper methods
        void uart_read_callback(const uint8_t *buf, size_t len);
        std::vector<uint8_t> determine_response_for_request(const std::vector<uint8_t>& in_data);
        void send_periodic_data(NosEngine::Common::SimTime time);

        void string_to_uint8vector(const std::string& in, std::vector<uint8_t>& outvector);
        uint8_t char_to_hex(char in);
        void hexstring_to_uint8vector(const std::string& in_data, std::vector<uint8_t>& outvector);
        void double_to_uint8vector(double in_data, std::vector<uint8_t>& outvector);
        bool is_valid_period(std::string in_string, double& period);

        void create_binary_error(const GPSSimDataPoint& data_point, std::vector<uint8_t>& error);
        void create_ascii_error(const std::vector<std::string>& words, const GPSSimDataPoint& data_point,  std::vector<uint8_t>& error);

        void get_ascii_header_string(const std::string& message, const GPSSimDataPoint& data_point, std::string& header);
        void get_binary_header_bytes(uint16_t message, uint16_t length, const GPSSimDataPoint& data_point, std::vector<uint8_t>& header);
        void get_gpggaa_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& response);
        void get_bestxyza_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& response);
        void get_bestxyzb_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& response);
        void get_rangecmpa_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& response);
        void get_rangecmpb_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& response);

        typedef uint32_t    Hex4;
        Hex4 CRC32Value(int i);
        Hex4 CalculateBlockCRC32(unsigned long ulCount /* Number of bytes in the data block */, const char *ucBuffer ) /* Data block */;
        void compute_checksum(const std::string& message, std::string& checksum);

        void convert_geocentric_to_geodetic_msl_egm96(MSP::CCS::CoordinateConversionService& ccs_geocentric_to_geodetic_msl_egm96,
            double x, double y, double z, double& lat, double& lon, double& height);
        void convert_geocentric_to_geodetic_ellipsoid(MSP::CCS::CoordinateConversionService& ccs_geodetic_ellipsoid_to_geocentric,
            double x, double y, double z, double& lat, double& lon, double& height);

        // Private data
        typedef void (GPSSimHardwareModelOEM615::*get_log_data_func)(const GPSSimDataPoint&, std::vector<uint8_t>&);
        std::unique_ptr<NosEngine::Client::Bus> _time_bus;
        std::unique_ptr<NosEngine::Uart::Uart> _uart_connection;
        std::map<std::string, get_log_data_func> _get_log_data_map; // message, function to call to generate data for that message
        std::map<std::string, boost::tuple<double, double>> _periodic_logs; // message, (last absolute time function was called, period (seconds) to call function)
        MSP::CCS::CoordinateSystemParameters _geocentric_parameters;
        MSP::CCS::GeodeticParameters _msl_egm96_parameters;
        MSP::CCS::GeodeticParameters _ellipsoid_parameters;
        MSP::CCS::CoordinateConversionService _ccs_geocentric_to_geodetic_msl_egm96;
        MSP::CCS::CoordinateConversionService _ccs_geodetic_ellipsoid_to_geocentric;

    };
}

#endif
