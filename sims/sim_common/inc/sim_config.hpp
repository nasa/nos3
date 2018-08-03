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

#ifndef NOS3_SIMCONFIG_HPP
#define NOS3_SIMCONFIG_HPP

#define SIM_LOGGER                "nos3.sim"

#include <string>
#include <stdint.h>

#include <boost/property_tree/ptree.hpp>

namespace Nos3
{

    /// \brief Class to describe the configuration for the simulation.
    class SimConfig
    {
    public:
        /// @name Constructors
        //@{
        /** \brief Constructor taking the number of arguments passed to the program and the arguments passed to the program.
         * @param argc  The number of arguments.
         * @param argv  The array of (string) arguments.
         *
         * The constructor takes care of creating the simulation configuration from a combination of the arguments
         * passed on the command line, from a configuration file, and from defaults.
         */
        SimConfig(int argc, char *argv[]);
        //@}

        /// @name Accessors
        //@{

        /// \brief Given a simulator name, this method determines the configuration for the simulator and runs it.
        /// @param simulator  The name of the simulator to run.
        void run_simulator(std::string simulator_name) const;

        /// \brief Given a simulator name, this method returns a property tree of common and simulator specific configuration data for the simulator.
        /// @param simulator  The name of the simulator to create a property tree configuration for.
        /// @return A property tree containing common and specific simulator configuration data for the named simulator.
        boost::property_tree::ptree get_config_for_simulator(std::string simulator) const;

        /// \brief Returns a copy of the property tree of all configuration data read from the command line and configuration file.
        boost::property_tree::ptree get_config(void) const;

        /// \brief Returns a string representation of this object.
        /// @return A string representing the data in this object.
        std::string to_string(void) const;

        //@}
    private:
        // Private helper methods
        void        parse_options(int argc, char *argv[]);

        // Private data
        boost::property_tree::ptree _config;
        std::string _config_filename;
    };

}

#endif

