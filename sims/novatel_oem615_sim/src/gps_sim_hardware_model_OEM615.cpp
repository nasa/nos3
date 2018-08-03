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

#include <stdexcept>
#include <cmath>
#include <stdint.h>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <sofa.h>
#include <GeodeticCoordinates.h>
#include <CartesianCoordinates.h>
#include <Accuracy.h>

#include <ItcLogger/Logger.hpp>

#include <gps_sim_data_file_provider.hpp>
#include <gps_sim_hardware_model_OEM615.hpp>

namespace Nos3
{
    REGISTER_HARDWARE_MODEL(GPSSimHardwareModelOEM615,"OEM615");

    extern ItcLogger::Logger *sim_logger;

    /*************************************************************************
     * Constructors
     *************************************************************************/

    GPSSimHardwareModelOEM615::GPSSimHardwareModelOEM615(const boost::property_tree::ptree& config)
        : GPSSimHardwareModelCommon(config),
        _geocentric_parameters(MSP::CCS::CoordinateType::geocentric),
        _msl_egm96_parameters(MSP::CCS::CoordinateType::geodetic, MSP::CCS::HeightType::EGM96FifteenMinBilinear),
        _ellipsoid_parameters(MSP::CCS::CoordinateType::geodetic, MSP::CCS::HeightType::ellipsoidHeight),
        _ccs_geocentric_to_geodetic_msl_egm96("WGE", &_geocentric_parameters, "WGE", &_msl_egm96_parameters),
        _ccs_geodetic_ellipsoid_to_geocentric("WGE", &_ellipsoid_parameters, "WGE", &_geocentric_parameters)
    {
        // SET env var MSPCCS_DATA

        std::string connection_string = config.get("common.nos-connection-string", "tcp://127.0.0.1:12001");

        // Set up the time node which is **required** for this model
        std::string time_bus_name = "command";
        if (config.get_child_optional("hardware-model.connections")) {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("hardware-model.connections")) {
                // v.first is the name of the child.
                // v.second is the child tree.
                if (v.second.get("type", "").compare("time") == 0) {
                    time_bus_name = v.second.get("bus-name", "command");
                    break;
                }
            }
        }
        _time_bus.reset(new NosEngine::Client::Bus(_hub, connection_string, time_bus_name));
        sim_logger->debug("GPSSimHardwareModelOEM615::GPSSimHardwareModelOEM615:  Time bus %s now active.", time_bus_name.c_str());

        std::string bus_name = "usart_0";
        int node_port = 0;
        // Use config data if it exists
        if (config.get_child_optional("simulator.hardware-model.connections")) {
            BOOST_FOREACH(const boost::property_tree::ptree::value_type &v, config.get_child("simulator.hardware-model.connections")) {

                std::ostringstream oss;
                write_xml(oss, v.second);
                sim_logger->trace("GPSSimHardwareModelOEM615::GPSSimHardwareModelOEM615:  "
                    "simulator.hardware-model.connections.connection subtree:\n%s", oss.str().c_str());

                // v.first is the name of the child.
                // v.second is the child tree.
                if (v.second.get("type", "").compare("usart") == 0) {
                    bus_name = v.second.get("bus-name", bus_name);
                    node_port = v.second.get("node-port", node_port);
                    break;
                }
            }
        }
        _time_bus->add_time_tick_callback(std::bind(&GPSSimHardwareModelOEM615::send_periodic_data, this, std::placeholders::_1));
        _uart_connection.reset(new NosEngine::Uart::Uart(_hub, config.get("simulator.name", "gps"), connection_string,
            bus_name));
        _uart_connection->open(node_port);
        _uart_connection->set_read_callback(
            std::bind(&GPSSimHardwareModelOEM615::uart_read_callback, this, std::placeholders::_1, std::placeholders::_2));

        _get_log_data_map.insert(std::map<std::string, get_log_data_func>::value_type("BESTXYZA", &GPSSimHardwareModelOEM615::get_bestxyza_response));
        _get_log_data_map.insert(std::map<std::string, get_log_data_func>::value_type("GPGGAA", &GPSSimHardwareModelOEM615::get_gpggaa_response));
        _get_log_data_map.insert(std::map<std::string, get_log_data_func>::value_type("RANGECMPA", &GPSSimHardwareModelOEM615::get_rangecmpa_response));
        _get_log_data_map.insert(std::map<std::string, get_log_data_func>::value_type("BESTXYZA", &GPSSimHardwareModelOEM615::get_bestxyzb_response));
        _get_log_data_map.insert(std::map<std::string, get_log_data_func>::value_type("RANGECMPA", &GPSSimHardwareModelOEM615::get_rangecmpb_response));
        // TODO - The following two lines are a hack for now to set the configuration of the sim to be the same as the configuration that is saved
        // in the firmware of the STF-1 NovAtel OEM615 - Remove me and make me a configuration option and/or out of band commanding option
        _periodic_logs.insert(std::map<std::string, boost::tuple<double, double>>::value_type("RANGECMPA", boost::tuple<double, double>(_absolute_start_time + 10.0, 1.0)));
        _periodic_logs.insert(std::map<std::string, boost::tuple<double, double>>::value_type("BESTXYZA", boost::tuple<double, double>(_absolute_start_time + 10.0, 1.0)));
    }

    GPSSimHardwareModelOEM615::~GPSSimHardwareModelOEM615(void)
    {
        _uart_connection->close();
    }

    /*************************************************************************
     * Private helper methods
     *************************************************************************/

    void GPSSimHardwareModelOEM615::uart_read_callback(const uint8_t *buf, size_t len)
    {
        // Get the data out of the message bytes - Hardware independent
        std::vector<uint8_t> in_data(buf, buf + len);

        sim_logger->debug("GPSSimHardwareModelOEM615::uart_read_callback:  REQUEST %s",
            SimIHardwareModel::uint8_vector_to_hex_string(in_data).c_str()); // log data in a man readable format

        // Get the hardware response for the request - Hardware and algorithm dependent
        std::vector<uint8_t> out_data = determine_response_for_request(in_data);

        // Ship the message bytes off (we're done!) - Hardware independent
        sim_logger->debug("GPSSimHardwareModelOEM615::uart_read_callback:  REPLY   %s\n",
            SimIHardwareModel::uint8_vector_to_hex_string(out_data).c_str()); // log data in a man readable format

        _uart_connection->write(&out_data[0], out_data.size());
    }

    std::vector<uint8_t> GPSSimHardwareModelOEM615::determine_response_for_request(const std::vector<uint8_t>& in_data)
    {
        std::vector<uint8_t> out_data;

        // 2.  Ask the data provider for the GPS data to return - Hardware independent, BUT data provider dependent
        const boost::shared_ptr<GPSSimDataPoint> data_point =
            boost::dynamic_pointer_cast<GPSSimDataPoint>(_sim_data_provider->get_data_point());

        if (in_data.size() < 3) { // Assume binary and something is wrong
            sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  Invalid REQUEST:  Size less than 3 bytes");
            create_binary_error(*data_point, out_data);
        } else if ((in_data[0] == 0xAA) && (in_data[1] == 0x44) && (in_data[2] == 0x12)) { // binary command
            sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  Binary commands are not (yet) supported."
                "  REQUEST:  0x%x", in_data[6]);
            create_binary_error(*data_point, out_data);
            if (in_data.size() >= 28) {
                out_data[4] = in_data[4]; // Message ID
                out_data[5] = in_data[5]; // Message ID
                out_data[6] = in_data[6] | 0x80; // Message Type
                out_data[7] = in_data[7]; // Port Address
            }
        } else { // ascii command
            std::string in_ascii;
            // 1.  Unpack the message content - Hardware dependent
            for (size_t i = 0; i < in_data.size(); i++) {
                in_ascii += (char)in_data[i];
            }
            std::vector<std::string> words;
            boost::split(words, in_ascii, boost::is_any_of(", "), boost::token_compress_on);
            for (size_t i = 0; i < words.size(); i++) {
                boost::to_upper(words[i]);
            }
            if (words.size() > 0) {
                std::string command = words[0];
                if ((command.size()) > 0 && (command[command.size() - 1] == 'A')) { // ASCII command
                    sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  ASCII commands are not (yet) supported.  Command:  %s",
                    command.c_str());
                    create_ascii_error(words, *data_point, out_data);
                } else { // abbreviated ASCII command
                    sim_logger->debug("GPSSimHardwareModelOEM615::determine_response_for_request:  Abbreviated ASCII command.  Command:  %s, REQUEST:  %s",
                        command.c_str(), in_ascii.c_str());

                    // TODO - Set up parser to more generically process commands... including ignoring case, understanding positional/optional parameters, etc.
                    // Reference:  OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
                    if (command.compare("LOG") == 0) { // Requesting log output
                        if ((words.size() < 2) || (words[1].compare(0, 3, "COM") != 0)) {
                            words.insert(words.begin() + 1, "NO_PORTS");
                        }
                        if (words.size() >= 3) {
                            std::string log_type = words[2];
                            std::map<std::string, get_log_data_func>::iterator search = _get_log_data_map.find(log_type);
                            if (search != _get_log_data_map.end()) {
                                sim_logger->debug("GPSSimHardwareModelOEM615::determine_response_for_request:  LOG requested of type:  %s",
                                    log_type.c_str());
                                if ((words.size() >= 5) && (words[3].compare("ONTIME") == 0) /* && period_is_valid(words[4])*/) {
                                    sim_logger->debug("GPSSimHardwareModelOEM615::determine_response_for_request:  "
                                        "LOG requested with ONTIME trigger, period:  %s", words[4].c_str());
                                    double period;
                                    if (is_valid_period(words[4], period)) {
                                        std::pair<std::map<std::string, boost::tuple<double, double>>::iterator, bool> ret =
                                            _periodic_logs.insert(std::map<std::string, boost::tuple<double, double>>::value_type(
                                                search->first, boost::tuple<double, double>(data_point->get_abs_time(), period)));
                                        if (ret.second) {
                                            string_to_uint8vector("<OK", out_data);
                                        } else {
                                            string_to_uint8vector("<TRIGGER ALREADY EXISTS; NOT VALID FOR THIS LOG", out_data);
                                        }
                                    } else {
                                        sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  "
                                            "Invalid period specified for ONTIME log.  Period:  %s", words[4].c_str());
                                        string_to_uint8vector("<REQUESTED RATE IS INVALID", out_data);
                                    }
                                } else if ((words.size() == 3) || (words[3].compare("ONCE") == 0)) {
                                    sim_logger->debug("GPSSimHardwareModelOEM615::determine_response_for_request:  LOG of type:  %s requested with ONCE (or no) trigger",
                                        log_type.c_str());
                                    get_log_data_func f = search->second;
                                    (this->*f)(*data_point, out_data);
                                } else {
                                    sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  "
                                        "Invalid trigger or not enough arguments provided.  Trigger:  %s,  number of args:  %d",
                                        words[3].c_str(), words.size());
                                    string_to_uint8vector("<INVALID MESSAGE ID", out_data);
                                }
                            } else {
                                // Do nothing... let the abbreviated ASCII error be returned
                                sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  LOG of type:  %s is not (yet) supported",
                                    log_type.c_str());
                                string_to_uint8vector("<INVALID MESSAGE ID", out_data);
                            }
                        } else {
                            sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  LOG requested, but no log specified");
                            string_to_uint8vector("<INVALID MESSAGE ID", out_data);
                        }
                    } else if (command.compare("UNLOGALL") == 0) { // Requesting to unlog all output
                        _periodic_logs.clear();
                        string_to_uint8vector("<OK", out_data);
                    } else if (command.compare("UNLOG") == 0) { // Requesting to unlog output
                        if ((words.size() < 2) || (words[1].compare(0, 3, "COM") != 0)) {
                            words.insert(words.begin() + 1, "NO_PORTS");
                        }
                        if (words.size() >= 3) {
                            std::string log_type = words[2];
                            _periodic_logs.erase(log_type);
                            string_to_uint8vector("<OK", out_data);
                        } else {
                            sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  UNLOG requested, but no log specified");
                            string_to_uint8vector("<INVALID MESSAGE ID", out_data);
                        }
                    } else if (command.compare("SERIALCONFIG") == 0) { // Request to set the serial port configuration... nothing needs done by the sim
                        sim_logger->warning("GPSSimHardwareModelOEM615::determine_response_for_request:  SERIALCONFIG requested... nothing to do.");
                        string_to_uint8vector("<OK", out_data);
                    } else {
                        sim_logger->debug("GPSSimHardwareModelOEM615::determine_response_for_request:  Unsupported abbreviated ASCII command");
                        string_to_uint8vector("<INVALID MESSAGE ID", out_data);
                    }
                }
            } else {
                sim_logger->debug("GPSSimHardwareModelOEM615::determine_response_for_request:  No words found in ASCII request.  REQUEST:  %s",
                    in_ascii.c_str());
                string_to_uint8vector("<INVALID MESSAGE ID", out_data);
            }
        }

        return out_data;
    }

    void GPSSimHardwareModelOEM615::send_periodic_data(NosEngine::Common::SimTime time)
    {
        const boost::shared_ptr<GPSSimDataPoint> data_point =
            boost::dynamic_pointer_cast<GPSSimDataPoint>(_sim_data_provider->get_data_point());

        std::vector<uint8_t> data;

        double abs_time = _absolute_start_time + (double(time * _sim_microseconds_per_tick)) / 1000000.0;

        for (std::map<std::string, boost::tuple<double, double>>::iterator it = _periodic_logs.begin(); it != _periodic_logs.end(); it++) {
            boost::tuple<double, double> value = it->second;
            double prev_time = boost::tuples::get<0>(value);
            double period = boost::tuples::get<1>(value);
            double next_time = prev_time + period - (_sim_microseconds_per_tick / 1000000.0) / 2; // within half a tick time period
            if (next_time < abs_time) { // Time to send more data
                it->second = boost::tuple<double, double>(abs_time, period);
                std::map<std::string, get_log_data_func>::iterator search = _get_log_data_map.find(it->first);
                if (search != _get_log_data_map.end()) {
                    get_log_data_func f = search->second;
                    (this->*f)(*data_point, data);
                    _uart_connection->write(&data[0], data.size());
                }
            }
        }
    }

    /* Just reinterprets each character as its ASCII value */
    void GPSSimHardwareModelOEM615::string_to_uint8vector(const std::string& in_data, std::vector<uint8_t>& out_data)
    {
        for (size_t i = 0; i < in_data.length(); i++) {
            out_data.push_back(in_data[i]);
        }
    }

    uint8_t GPSSimHardwareModelOEM615::char_to_hex(char in)
    {
        uint8_t out = 0;

        if (('0' <= in) && (in <= '9')) {
            out = in - '0';
        }
        else if (('A' <= in) && (in <= 'F')) {
            out = in - 'A' + 10;
        }
        else if (('a' <= in) && (in <= 'f')) {
            out = in - 'a' + 10;
        }

        return out;
    }

    /* Expects each character to be 0-9, A-F and reinterprets each pair of characters as a hex number, 00-FF */
    void GPSSimHardwareModelOEM615::hexstring_to_uint8vector(const std::string& in_data, std::vector<uint8_t>& out_data)
    {
        uint8_t high_nibble;
        uint8_t low_nibble;
        for (size_t i = 0; i < in_data.length(); i+=2) {
            // Convert first character to high nibble
            high_nibble = char_to_hex(in_data[i]);
            // Convert second character to low nibble (if there is a second character
            if (i < in_data.length() - 1) {
                low_nibble = char_to_hex(in_data[i+1]);
            } else {
                low_nibble = 0;
            }
            // Save off the nibbles as a byte
            out_data.push_back((high_nibble << 8) + low_nibble);
        }
       
    }

    void GPSSimHardwareModelOEM615::double_to_uint8vector(double in_data, std::vector<uint8_t>& out_data)
    {
        uint8_t *p = (uint8_t *)&in_data;
        /*for (int i = 7; i >= 0; i--) {
            out_data.push_back(p[i]);
            }*/
        for (int i = 0; i <= 7; i++){
            out_data.push_back(p[i]);
        }
    }

    bool GPSSimHardwareModelOEM615::is_valid_period(std::string in_string, double& period)
    {
        if (in_string.compare("0.05") == 0) {
            period = 0.05;
            return true;
        } else if (in_string.compare("0.1") == 0) {
            period = 0.1;
            return true;
        } else if (in_string.compare("0.2") == 0) {
            period = 0.2;
            return true;
        } else if (in_string.compare("0.25") == 0) {
            period = 0.25;
            return true;
        } else if (in_string.compare("0.5") == 0) {
            period = 0.5;
            return true;
        } else {
            int iperiod = atoi(in_string.c_str());
            if (iperiod > 0) {
                period = iperiod;
                return true;
            }
        }
        return false;
    }

    // Reference:  Section 1.1, pp. 21-22, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    typedef int8_t      Char;
    typedef uint8_t     UChar;
    typedef int16_t     Short;
    typedef uint16_t    UShort;
    typedef int32_t     Long;
    typedef uint32_t    ULong;
    typedef float       Float;
    typedef double      Double;
    typedef std::string Enum;
    typedef uint8_t     Hex1;
    typedef std::string String;

    void GPSSimHardwareModelOEM615::create_binary_error(const GPSSimDataPoint& data_point, std::vector<uint8_t>& out_data)
    {
        out_data.reserve(32);
        out_data[ 0] = 0xAA; // Sync
        out_data[ 1] = 0x44; // Sync
        out_data[ 2] = 0x12; // Sync
        out_data[ 3] =   28; // Header Lgth
        out_data[ 4] =    0; // Message ID
        out_data[ 5] =    0; // Message ID
        out_data[ 6] = 0x80; // Message Type
        out_data[ 7] =    0; // Port address - TODO FIX ME - Just an example
        out_data[ 8] =    0; // Message Length
        out_data[ 9] =    4; // Message Length
        out_data[10] =    0; // Sequence
        out_data[11] =    0; // Sequence
        out_data[12] =    0; // Idle Time - TODO FIX ME - Just an example
        int16_t week = data_point.get_gps_week();
        if (week == 0) {
            out_data[13] = 20; // Time Status - Time validity is unknown
        } else {
            out_data[13] = 180; // Time Status - Time is fine set and being steered
        }
        out_data[14] = (uint8_t)((week >> 8) & 0xff); // Week
        out_data[15] = (uint8_t)((week     ) & 0xff); // Week
        int32_t gpsec = (int32_t)((data_point.get_gps_sec_week() + data_point.get_gps_frac_sec()) * 1000000.0);
        out_data[16] = (uint8_t)((gpsec >> 24) & 0xff); // ms
        out_data[17] = (uint8_t)((gpsec >> 16) & 0xff); // ms
        out_data[18] = (uint8_t)((gpsec >>  8) & 0xff); // ms
        out_data[19] = (uint8_t)((gpsec      ) & 0xff); // ms
        out_data[20] = 0x00; // Receiver Status - TODO FIX ME - Just an example
        out_data[21] = 0x00; // Receiver Status - TODO FIX ME - Just an example
        out_data[22] = 0x00; // Receiver Status - TODO FIX ME - Just an example
        out_data[23] = 0x40; // Receiver Status - TODO FIX ME - Just an example
        out_data[24] = 0xD8; // Reserved - TODO FIX ME - Just an example
        out_data[25] = 0x21; // Reserved - TODO FIX ME - Just an example
        out_data[26] = 0x0A; // Receiver S/W Version - TODO FIX ME - Just an example
        out_data[27] = 0xA4; // Receiver S/W Version - TODO FIX ME - Just an example

        out_data[28] =    0; // Response ID - TODO FIX ME - Just an example
        out_data[29] =    0; // Response ID - TODO FIX ME - Just an example
        out_data[30] =    0; // Response ID - TODO FIX ME - Just an example
        out_data[31] =    6; // Response ID - INVALID MESSAGE ID - TODO FIX ME - Just an example
    }

    void GPSSimHardwareModelOEM615::create_ascii_error(const std::vector<std::string>& words, const GPSSimDataPoint& data_point, std::vector<uint8_t>& out_data)
    {
        // message, port
        std::string message("BLANK");
        if (words.size() > 0) message = words[0];
        std::string port("NO_PORTS");
        if (words.size() >1) port = words[1];
        Long sequence_num = 0; // - TODO FIX ME - Just an example
        Float pct_idle_time = 0.0; // - TODO FIX ME - Just an example
        ULong week = data_point.get_gps_week();
        std::string time_status("UNKNOWN"); // Time Status - Time validity is unknown
        if (week != 0) {
            time_status = "FINESTEERING"; // Time Status - Time is fine set and being steered
        }
        float gpsec = data_point.get_gps_sec_week() + data_point.get_gps_frac_sec();
        ULong receiver_status = 0x00000040; // - TODO FIX ME - Just an example
        ULong header_reserved = 0xD821;
        ULong receiver_sw_version = 2724;
        std::string response("\"INVALID MESSAGE ID\"");

        // Format output data
        std::stringstream ss;
        // TODO FIX ME - Just an example
        //ss << "BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,00000040,D821,";
        //ss << "2724;SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,";
        //ss << "4942496.3270,0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,";
        //ss << "-0.0049,-0.0001,0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,";
        //ss << "12,11,11,11,0,01,0,33"; // CRC is:  E9EAFECA
        ss << std::fixed << std::uppercase;
        ss << message << ",";
        ss << port << ",";
        ss << sequence_num << ",";
        ss << std::setprecision(1) << pct_idle_time << ",";
        ss << time_status << ",";
        ss << week << ",";
        ss << std::setprecision(3) << gpsec << ",";
        ss << std::setfill('0') << std::setw(8) << std::hex << receiver_status << ",";
        ss << std::hex << header_reserved << ",";
        ss << std::dec << receiver_sw_version << ";";
        ss << response;

        // Output data
        Hex4 CRC = CalculateBlockCRC32(ss.str().length(), ss.str().c_str());
        ss << "*" << std::hex << CRC << "\r\n";
        std::string sentence;
        sentence.append("#").append(ss.str());

        string_to_uint8vector(sentence, out_data);
    }

    // Reference:  Section 1.1.1, p. 24, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    void GPSSimHardwareModelOEM615::get_ascii_header_string(const std::string& message, const GPSSimDataPoint& data_point, std::string& out_data)
    {
        String port("COM1"); // - TODO FIX ME - Just an example
        Long sequence_num = 0; // - TODO FIX ME - Just an example
        Float pct_idle_time = 0.0; // - TODO FIX ME - Just an example
        Enum time_status("FINESTEERING"); // - TODO FIX ME - Just an example
        ULong week = data_point.get_gps_week();
        float gpsec = data_point.get_gps_sec_week() + data_point.get_gps_frac_sec();
        ULong receiver_status = 0x00000040; // - TODO FIX ME - Just an example
        ULong header_reserved = 0xD821;
        ULong receiver_sw_version = 2724;

        // Format output data
        std::stringstream ss;
        // TODO FIX ME - Just an example
        //ss << "BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,00000040,D821,";
        //ss << "2724;SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,";
        //ss << "4942496.3270,0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,";
        //ss << "-0.0049,-0.0001,0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,";
        //ss << "12,11,11,11,0,01,0,33"; // CRC is:  E9EAFECA
        ss << std::fixed << std::uppercase;
        ss << message << ",";
        ss << port << ",";
        ss << sequence_num << ",";
        ss << std::setprecision(1) << pct_idle_time << ",";
        ss << time_status << ",";
        ss << week << ",";
        ss << std::setprecision(3) << gpsec << ",";
        ss << std::setfill('0') << std::setw(8) << std::hex << receiver_status << ",";
        ss << std::hex << header_reserved << ",";
        ss << std::dec << receiver_sw_version << ";";

        out_data = ss.str();
    }
    // Reference:  Section 1.1.3, pp. 26-27, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    void GPSSimHardwareModelOEM615::get_binary_header_bytes(uint16_t message, uint16_t length, const GPSSimDataPoint& data_point, std::vector<uint8_t>& out)
    {
        uint16_t week = data_point.get_gps_week();
        uint32_t gpsec = (data_point.get_gps_sec_week() * 1000) + (data_point.get_gps_frac_sec() * 1000.0);

        //Sync Bytes
        out.push_back(0xAA);
        out.push_back(0x44);
        out.push_back(0x12);
        //Header Size
        out.push_back((uint8_t)28);
        //Message ID
        out.push_back(0x00ff & message);
        out.push_back(0xff00 & message);
        //Port Address
        out.push_back(0);
        //Message Length
        out.push_back(0x20); // - TODO FIX ME - Just an example
        //Messge Length
        out.push_back(0x00ff & length);
        out.push_back(0xff00 & length);
        //Sequence
        out.push_back(0); // - TODO FIX ME - Just an example
        out.push_back(0); // - TODO FIX ME - Just an example
        //Idle Time
        out.push_back(0); // - TODO FIX ME - Just an example
        //Time Status
        out.push_back((uint8_t)180); // - TODO FIX ME - Just an example
        //Week
        out.push_back(0xff & week); // GPS reference week number
        out.push_back(0xff & (week >> 8)); // GPS reference week number
        //ms
        out.push_back(gpsec & 0xff); // GPS milliseconds from the beginning of the GPS reference weekw
        out.push_back((gpsec >> 8) & 0xff); // GPS milliseconds from the beginning of the GPS reference week
        out.push_back((gpsec >> 16) & 0xff); // GPS milliseconds from the beginning of the GPS reference week
        out.push_back((gpsec >> 24) & 0xff); // GPS milliseconds from the beginning of the GPS reference week
        //Receiver Status
        out.push_back(0x00); // - TODO FIX ME - Just an example
        out.push_back(0x00); // - TODO FIX ME - Just an example
        out.push_back(0x00); // - TODO FIX ME - Just an example
        out.push_back(0x40); // - TODO FIX ME - Just an example
        //Reserved
        out.push_back(0xD8);
        out.push_back(0x21);
        //Recvr SW version
        out.push_back(0x0A);
        out.push_back(0xA4);
    }

    // Reference:  Section 3.2.4, pp. 474-476, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    void GPSSimHardwareModelOEM615::get_gpggaa_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& out_data)
    {
        // Computations
        double abs_time = data_point.get_abs_time();
        int year, month, day, ihmsf[4];
        double fractional_day;
        char sign;

        // Time computations
        iauJd2cal(2451545.0, abs_time / 86400.0, &year, &month, &day, &fractional_day);
        iauD2tf(2, fractional_day, &sign, ihmsf);
        sim_logger->trace("GPSSimHardwareModelOEM615::get_gpgga_string:  abs_time: %12.4f to Julian date/time: %4.4d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d.%2.2d",
            abs_time, year, month, day, ihmsf[0], ihmsf[1], ihmsf[2], ihmsf[3]);

        // Position computations
        double ecef_x = data_point.get_ECEF_x();
        double ecef_y = data_point.get_ECEF_y();
        double ecef_z = data_point.get_ECEF_z();

        double latitude, longitude, ellipsoid_height, msl_height, undulation, latitude_whole_degrees, latitude_fractional_degrees, longitude_whole_degrees, longitude_fractional_degrees;
        convert_geocentric_to_geodetic_ellipsoid(_ccs_geodetic_ellipsoid_to_geocentric, ecef_x, ecef_y, ecef_z, latitude, longitude, ellipsoid_height);
        convert_geocentric_to_geodetic_msl_egm96(_ccs_geocentric_to_geodetic_msl_egm96, ecef_x, ecef_y, ecef_z, latitude, longitude, msl_height);
        latitude = latitude * 180.0 / M_PI;
        longitude = longitude * 180.0 / M_PI;
        latitude_whole_degrees = int(latitude);
        latitude_fractional_degrees = latitude - latitude_whole_degrees;
        longitude_whole_degrees = int(longitude);
        longitude_fractional_degrees = longitude - longitude_whole_degrees;
        sim_logger->trace("GPSSimHardwareModelOEM615::get_gpgga_string:  "
            "ecef_x/y/z: %12.4f/%12.4f/%12.4f to latitude/longitude/msl_height/ellipsoid_height/undulation: %12.4f/%12.4f/%12.4f/%12.4f/%12.4f",
            ecef_x, ecef_y, ecef_z, latitude, longitude, msl_height, ellipsoid_height, undulation);

        // Create output data
        // TODO - CHECK ALL OF THIS!  UNITS, CONVERSIONS, ACCURACY, DATUMS, COORDINATE FRAMES, ETC.
        std::string talker_id("GP");
        std::string type_of_message("GGA");
        double utc = ihmsf[0] * 10000 + ihmsf[1] * 100 + ihmsf[2] + (double)ihmsf[3] / 100.0; // hhmmss.ss
        double lat = latitude_whole_degrees * 100.0 + latitude_fractional_degrees * 60.0; // DDmm.mm
        double lon = longitude_whole_degrees * 100.0 + longitude_fractional_degrees * 60.0; // DDmm.mm
        std::string quality("8"); // Simulator mode
        int num_sats = 10; // Number of satellites in use (00-12) - TODO FIX ME - Just an example
        double hdop = 1.0; // - TODO FIX ME - Just an example
        double alt_msl = msl_height; // Antenna altitude above/below msl
        std::string alt_units("M"); // meters
        undulation = ellipsoid_height - msl_height; // Height of EGM96 geoid above WGS84 ellipsoid
        std::string u_units("M"); // meters
        std::string age(""); // Age of Differential GPS data (in seconds)
        std::string stn_ID(""); // Differential base station ID, 0000-1023

        // Format output data
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2);
        ss << talker_id << type_of_message << ",";
        ss << utc << ",";
        ss << std::setprecision(7);
        ss << ((lat >= 0) ? lat : -1.0 * lat) << ",";
        ss << ((lat >= 0) ? "N" : "S") << ",";
        ss << ((lon >= 0) ? lon : -1.0 * lon) << ",";
        ss << ((lon >= 0) ? "E" : "W") << ",";
        ss << quality << ",";
        ss << num_sats << ",";
        ss << std::setprecision(1);
        ss << hdop << ",";
        ss << std::setprecision(3);
        ss << alt_msl << ",";
        ss << alt_units << ",";
        ss << undulation << ",";
        ss << u_units << ",";
        ss << age << ",";
        ss << stn_ID;
        //ss << "$GPGGA,134658.00,5106.9792,N,11402.3003,W,2,09,1.0,1048.47,M,-16.27,M,08,AAAA*60";

        // Output data
        std::string message = ss.str();
        std::string checksum;
        compute_checksum(message, checksum);
        std::string sentence("$");
        sentence.append(message).append("*").append(checksum).append("\r\n");

        sim_logger->debug("GPSSimHardwareModelOEM615::get_gpgga_string:  RESPONSE:  %s", sentence.c_str());
        string_to_uint8vector(sentence, out_data);
    }

    // Reference:  Section 3.2.17, pp. 420-422, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    void GPSSimHardwareModelOEM615::get_bestxyza_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& out_data)
    {
        // Computations

        Enum p_sol_status("SOL_COMPUTED"); // - TODO FIX ME - Just an example
        Enum pos_type("NARROW_INT"); // - TODO FIX ME - Just an example
        Double p_x = data_point.get_ECEF_x();
        Double p_y = data_point.get_ECEF_y();
        Double p_z = data_point.get_ECEF_z();
        Float p_x_sigma = 0.0; // - TODO FIX ME - Just an example
        Float p_y_sigma = 0.0; // - TODO FIX ME - Just an example
        Float p_z_sigma = 0.0; // - TODO FIX ME - Just an example
        Enum v_sol_status("SOL_COMPUTED"); // - TODO FIX ME - Just an example
        Enum vel_type("NARROW_INT"); // - TODO FIX ME - Just an example
        Double v_x = data_point.get_velocity_x();
        Double v_y = data_point.get_velocity_y();
        Double v_z = data_point.get_velocity_z();
        Double v_x_sigma = 0.0; // - TODO FIX ME - Just an example
        Double v_y_sigma = 0.0; // - TODO FIX ME - Just an example
        Double v_z_sigma = 0.0; // - TODO FIX ME - Just an example
        String stn_id("\"\""); // - TODO FIX ME - Just an example
        Float v_latency = 0.0; // - TODO FIX ME - Just an example
        Float diff_age = 0.0; // - TODO FIX ME - Just an example
        Float sol_age = 0.0; // - TODO FIX ME - Just an example
        UChar num_svs = 0; // - TODO FIX ME - Just an example
        UChar num_soln_svs = 0; // - TODO FIX ME - Just an example
        UChar num_ggL1 = 0; // - TODO FIX ME - Just an example
        UChar num_soln_multi_svs = 0; // - TODO FIX ME - Just an example
        Char reserved = 0;
        Hex1 ext_sol_stat = 0; // - TODO FIX ME - Just an example
        Hex1 galileo_and_beidou_sig_mask = 0; // - TODO FIX ME - Just an example
        Hex1 gps_and_glonass_sig_mask = 0; // - TODO FIX ME - Just an example

        // Format output data
        std::stringstream ss;
        std::string header;
        // TODO FIX ME - Just an example
        //ss << "BESTXYZA,COM1,0,55.0,FINESTEERING,1419,340033.000,00000040,D821,";
        //ss << "2724;SOL_COMPUTED,NARROW_INT,-1634531.5683,-3664618.0326,";
        //ss << "4942496.3270,0.0099,0.0219,0.0115,SOL_COMPUTED,NARROW_INT,0.0011,";
        //ss << "-0.0049,-0.0001,0.0199,0.0439,0.0230,\"AAAA\",0.250,1.000,0.000,";
        //ss << "12,11,11,11,0,01,0,33"; // CRC is:  E9EAFECA
        get_ascii_header_string("BESTXYZA", data_point, header);
        ss << header;

        ss << p_sol_status << ",";
        ss << std::setprecision(4);
        ss << pos_type << ",";
        ss << p_x << ",";
        ss << p_y << ",";
        ss << p_z << ",";
        ss << p_x_sigma << ",";
        ss << p_y_sigma << ",";
        ss << p_z_sigma << ",";
        ss << v_sol_status << ",";
        ss << vel_type << ",";
        ss << v_x << ",";
        ss << v_y << ",";
        ss << v_z << ",";
        ss << v_x_sigma << ",";
        ss << v_y_sigma << ",";
        ss << v_z_sigma << ",";
        ss << stn_id << ",";
        ss << std::setprecision(3);
        ss << v_latency << ",";
        ss << diff_age << ",";
        ss << sol_age << ",";
        ss << (int)num_svs << ",";
        ss << (int)num_soln_svs << ",";
        ss << (int)num_ggL1 << ",";
        ss << (int)num_soln_multi_svs << ",";
        ss << (int)reserved << ",";
        ss << std::setw(2) << (int)ext_sol_stat << ",";
        ss << (int)galileo_and_beidou_sig_mask << ",";
        ss << (int)gps_and_glonass_sig_mask;

        // Output data
        Hex4 CRC = CalculateBlockCRC32(ss.str().length(), ss.str().c_str());
        ss << "*" << std::hex << CRC << "\r\n";
        std::string sentence;
        sentence.append("#").append(ss.str());

        sim_logger->debug("GPSSimHardwareModelOEM615::get_bestxyza_string:  RESPONSE:  %s", sentence.c_str());
        string_to_uint8vector(sentence, out_data);
    }

    // Reference:  Section 3.2.17, pp. 420-422, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    void GPSSimHardwareModelOEM615::get_bestxyzb_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& out)
    {
        std::vector<uint8_t> bytes;
        get_binary_header_bytes(241, 112, data_point, out); // BESTXYZ header

        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // P-sol status
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(50); // pos type

        double_to_uint8vector(data_point.get_ECEF_x(), bytes); // P-X
        out.insert(out.end(), bytes.begin(), bytes.end());
        bytes.clear();
        double_to_uint8vector(data_point.get_ECEF_y(), bytes); // P-Y
        out.insert(out.end(), bytes.begin(), bytes.end());
        bytes.clear();
        double_to_uint8vector(data_point.get_ECEF_z(), bytes); // P-Z
        out.insert(out.end(), bytes.begin(), bytes.end());
        bytes.clear();

        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // P-X sigma - TODO FIX ME - Just an example
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // P-Y sigma - TODO FIX ME - Just an example
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // P-Z sigma - TODO FIX ME - Just an example

        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // V-sol status
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(50); // vel type

        double_to_uint8vector(data_point.get_velocity_x(), bytes); // V-X
        out.insert(out.end(), bytes.begin(), bytes.end());
        bytes.clear();
        double_to_uint8vector(data_point.get_velocity_y(), bytes); // V-Y
        out.insert(out.end(), bytes.begin(), bytes.end());
        bytes.clear();
        double_to_uint8vector(data_point.get_velocity_z(), bytes); // V-Z
        out.insert(out.end(), bytes.begin(), bytes.end());
        bytes.clear();

        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // V-X sigma - TODO FIX ME - Just an example
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // V-Y sigma - TODO FIX ME - Just an example
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // V-Z sigma - TODO FIX ME - Just an example
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // stn ID - TODO FIX ME - Just an example
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // V-latency - TODO FIX ME - Just an example
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // diff_age - TODO FIX ME - Just an example
        out.push_back(0); out.push_back(0); out.push_back(0); out.push_back(0); // sol_age - TODO FIX ME - Just an example
        out.push_back(0); // #SVs - TODO FIX ME - Just an example
        out.push_back(0); // #solnSVs - TODO FIX ME - Just an example
        out.push_back(0); // #ggL1 - TODO FIX ME - Just an example
        out.push_back(0); // #solnMultiSVs - TODO FIX ME - Just an example
        out.push_back(0); // Reserved - TODO FIX ME - Just an example
        out.push_back(0); // ext sol stat - TODO FIX ME - Just an example
        out.push_back(0); // Galileo and BeiDou sig mask - TODO FIX ME - Just an example
        out.push_back(0); // GPS and GLONASS sig mask - TODO FIX ME - Just an example

        // Output data
        // Unlike static_cast, but like const_cast, the reinterpret_cast expression does not compile to any CPU instructions.
        // It is purely a compiler directive which instructs the compiler to treat the sequence of bits (object representation)
        // of expression as if it had the type new_type.
        Hex4 CRC = CalculateBlockCRC32(out.size(), reinterpret_cast<const char *>(&out[0]));
        out.push_back(CRC & 0xff);
        out.push_back((CRC >> 8) & 0xff);
        out.push_back((CRC >> 16) & 0xff);
        out.push_back((CRC >> 24) & 0xff);



    }

    /* For rangecmpa and rangecmpb: */
    const std::vector<std::string> fake_range_data = {
        "049C10081857F2DF1F4A130BA2888EB9600603A709030000",
        "0B9C3001225BF58F334A130BB1E2BED473062FA609020000",
        "449C1008340400E0AAA9A109A7535BAC2015CF71C6030000",
        "4B9C300145030010A6A9A10959C2F09120151F7166030000",
        "0B9D301113C8FFEFC284000C6EA051DBF3089DA1A0010000",
        "249D1018C6B7F67FA228820AF2E5E39830180AE1A8030000",
        "2B9D301165C4F8FFB228820A500A089F31185FE0A8020000",
        "449D1018BE18F41F2AACAD0A1A934EFC40074ECF88030000",
        "4B9D301182B9F69F38ACAD0A3E3AC28841079FCB88020000",
        "849D101817A1F95F16D7AF0A69FBE1FA401D3FD064030000",
        "8B9D30112909FB2F20D7AF0A9F24A687521DDECE64020000",
        "249E1118AF4E0470F66D4309A0A631CD642CF5B821320000",
        "2B9EB110A55903502F6E4309EE28D1AD032C7CB7E1320000",
        "849E1118B878F54F4ED2AA098C35558A532BDE1765220000",
        "8B9EB110ABCFF71F5ED2AA09CB6AD0F9032B9D16C5220000"
        };

    // Reference:  Section 3.2.114, pp. 607-610, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    void GPSSimHardwareModelOEM615::get_rangecmpa_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& out_data)
    {
        // Computations
        // Create output data

        // Format output data
        std::stringstream ss;
        std::string header;

        // TODO FIX ME - Just an example
        get_ascii_header_string("RANGECMPA", data_point, header);
        ss << header;
        ss << fake_range_data.size() << ",";
        for (int i = 0; i < fake_range_data.size() - 1; i++) {
            ss << fake_range_data[i] << ",";
        }
        ss << fake_range_data[fake_range_data.size() - 1]; // last line has no comma

        // Output data
        Hex4 CRC = CalculateBlockCRC32(ss.str().length(), ss.str().c_str());
        ss << "*" << std::hex << CRC << "\r\n";
        std::string sentence;
        sentence.append("#").append(ss.str());

        sim_logger->debug("GPSSimHardwareModelOEM615::get_rangecmpa_string:  RESPONSE:  %s", sentence.c_str());
        string_to_uint8vector(sentence, out_data);
    }

    // Reference:  Section 3.2.114, pp. 607-610, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    void GPSSimHardwareModelOEM615::get_rangecmpb_response(const GPSSimDataPoint& data_point, std::vector<uint8_t>& out)
    {
        get_binary_header_bytes(140/*msg*/, 4 + 24 * fake_range_data.size(), data_point, out); // RANGECMP header

        std::vector<uint8_t> line;
        for (int i = 0; i < fake_range_data.size() - 1; i++) {
            hexstring_to_uint8vector(fake_range_data[i], line);
            for (int j = 0; j < line.size(); j++) {
                out.push_back(line[j]);
            }
            line.clear();
        }

        // Output data
        // Unlike static_cast, but like const_cast, the reinterpret_cast expression does not compile to any CPU instructions.
        // It is purely a compiler directive which instructs the compiler to treat the sequence of bits (object representation)
        // of expression as if it had the type new_type.
        Hex4 CRC = CalculateBlockCRC32(out.size(), reinterpret_cast<const char *>(&out[0]));
        out.push_back(CRC & 0xff);
        out.push_back((CRC >> 8) & 0xff);
        out.push_back((CRC >> 16) & 0xff);
        out.push_back((CRC >> 24) & 0xff);

    }

    // Reference:  Section 1.7, pp. 37-38, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    #define CRC32_POLYNOMIAL 0xEDB88320L
    // Reference:  Section 1.7, pp. 37-38, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    /* --------------------------------------------------------------------------
    Calculate a CRC value to be used by CRC calculation functions.
    -------------------------------------------------------------------------- */
    GPSSimHardwareModelOEM615::Hex4 GPSSimHardwareModelOEM615::CRC32Value(int i)
    {
        int j;
        Hex4 ulCRC;
        ulCRC = i;
        for ( j = 8 ; j > 0; j-- )
        {
            if ( ulCRC & 1 )
                ulCRC = ( ulCRC >> 1 ) ^ CRC32_POLYNOMIAL;
            else
                ulCRC >>= 1;
        }
        return ulCRC;
    }
    // Reference:  Section 1.7, pp. 37-38, OEM6 Family Firmware Reference Manual, OM-20000129, Rev 8, January 2015 (file om-20000129.pdf)
    /* --------------------------------------------------------------------------
    Calculates the CRC-32 of a block of data all at once
    -------------------------------------------------------------------------- */
    GPSSimHardwareModelOEM615::Hex4 GPSSimHardwareModelOEM615::CalculateBlockCRC32(
        unsigned long ulCount, /* Number of bytes in the data block */
        const char *ucBuffer ) /* Data block */
    {
        Hex4 ulTemp1;
        Hex4 ulTemp2;
        Hex4 ulCRC = 0;
        while ( ulCount-- != 0 )
        {
            ulTemp1 = ( ulCRC >> 8 ) & 0x00FFFFFFL;
            ulTemp2 = CRC32Value( ((int) ulCRC ^ *ucBuffer++ ) & 0xff );
            ulCRC = ulTemp1 ^ ulTemp2;
        }
        return( ulCRC );
    }

    void GPSSimHardwareModelOEM615::compute_checksum(const std::string& message, std::string& checksum)
    {
        int c = 0;
        for (size_t i = 0; i < message.length(); i++) {
            c ^= message.at(i);
        }
        std::stringstream out;
        out << std::hex << std::setw(2) << c;
        checksum = out.str();
    }

    /**
     * Function which uses the given Geocentric to Geodetic (MSL EGM 96 15M)
     * Coordinate Conversion Service, 'ccs_geocentric_to_geodetic_msl_egm96', to
     * convert the given x, y, z coordinates to a lat, lon, and height.
     **/
    void GPSSimHardwareModelOEM615::convert_geocentric_to_geodetic_msl_egm96(
       MSP::CCS::CoordinateConversionService& ccs_geocentric_to_geodetic_msl_egm96,
       double x,
       double y,
       double z,
       double& lat,
       double& lon,
       double& height)
    {
       MSP::CCS::Accuracy sourceAccuracy;
       MSP::CCS::Accuracy targetAccuracy;
       MSP::CCS::CartesianCoordinates sourceCoordinates(MSP::CCS::CoordinateType::geocentric, x, y, z);
       MSP::CCS::GeodeticCoordinates targetCoordinates(MSP::CCS::CoordinateType::geodetic, lon, lat, height);

       ccs_geocentric_to_geodetic_msl_egm96.convertSourceToTarget(
          &sourceCoordinates,
          &sourceAccuracy,
          targetCoordinates,
          targetAccuracy );

       lat = targetCoordinates.latitude();
       lon = targetCoordinates.longitude();
       height = targetCoordinates.height();
    }

    /**
     * Function which uses the given Geodetic (Ellipsoid Height) to Geocentric
     * Coordinate Conversion Service, 'ccsGeodeticEllipsoidToGeocentric', to
     * convert the given x, y, z coordinates to a lat, lon, and height.
     **/
    void GPSSimHardwareModelOEM615::convert_geocentric_to_geodetic_ellipsoid(
       MSP::CCS::CoordinateConversionService& ccs_geodetic_ellipsoid_to_geocentric,
       double x,
       double y,
       double z,
       double& lat,
       double& lon,
       double& height)
    {
       MSP::CCS::Accuracy geocentricAccuracy;
       MSP::CCS::Accuracy geodeticAccuracy;
       MSP::CCS::CartesianCoordinates geocentricCoordinates(MSP::CCS::CoordinateType::geocentric, x, y, z);
       MSP::CCS::GeodeticCoordinates geodeticCoordinates;

       // Note that the Geodetic (Ellipsoid Height) to Geocentric Coordinate
       // Conversion Service is used here in conjunction with the
       // convertTargetToSource() method (as opposed to a Geocentric to
       // Geodetic (Ellipsoid Height) Coordinate Conversion Service in
       // conjunction with the convertSourceToTarget() method)
       ccs_geodetic_ellipsoid_to_geocentric.convertTargetToSource(
          &geocentricCoordinates,
          &geocentricAccuracy,
          geodeticCoordinates,
          geodeticAccuracy);

       lat = geodeticCoordinates.latitude();
       lon = geodeticCoordinates.longitude();
       height = geodeticCoordinates.height();
    }

}
