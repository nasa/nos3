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

#ifndef NOS3_SIMIDATAPROVIDER_HPP
#define NOS3_SIMIDATAPROVIDER_HPP

#include <sim_data_provider_maker.hpp>
#define REGISTER_DATA_PROVIDER(T,K) static Nos3::SimDataProviderMaker<T> maker(K) // T = type, K = key

#include <sim_config.hpp>
#include <sim_i_data_point.hpp>

namespace Nos3
{
    /// \brief Interface for a provider of simulation data.
    class SimIDataProvider
    {
    public:
        /// @name Constructors / destructors
        //@{
        /// \brief Constructor taking a configuration object.
        /// @param  sc  The configuration for the simulation
        SimIDataProvider(const boost::property_tree::ptree& config) {};
        /// \brief Destructor.
        virtual ~SimIDataProvider() {};
        //@}

        /// @name Non-mutating public worker methods
        //@{
        /** \brief Method to retrieve sim data for the current time.  This method must be overridden in a derived class.
         *
         * @returns                     A data point for the current time.
         */
        virtual boost::shared_ptr<SimIDataPoint> get_data_point() const = 0;
        //@}

        // Used to open up access to command the data provider
        // TODO: Does the current architecture make sense?  These are 42 specific.
        virtual void cmd_qrn(double q1, double q2, double q3, double q4) {};
        virtual void cmd_qrl(double q1, double q2, double q3, double q4) {};
        virtual void cmd_angles_wrt_frame(double ang1, double ang2, double ang3, long rotSeq, double frame) {};
        virtual void cmd_angles(double ang1, double ang2, double ang3) {};
        virtual void cmd_vector_ra_dec(double vecR0, double vecR1, double vecR2, double ra, double dec) {};
        virtual void cmd_vector_world_lng_lat_alt(double vecR0, double vecR1, double vecR2, double world, double lng, double lat, double alt) {};
        virtual void cmd_vector_world(double vecR0, double vecR1, double vecR2, double world) {};
        virtual void cmd_vector_ground_station(double vecR0, double vecR1, double vecR2, double groundStation) {};
        virtual void cmd_vector_sc_point(double vecR0, double vecR1, double vecR2, long sc, long body, double vec0, double vec1, double vec2) {};
        virtual void cmd_vector_sc(double vecR0, double vecR1, double vecR2, long sc) {};
        virtual void cmd_vector_point_at(double vecR0, double vecR1, double vecR2, const char* target) {};
        virtual void cmd_align(double vecR0, double vecR1, double vecR2, long sc, long body, double vec0, double vec1, double vec2) {};
        virtual void cmd_align_c_frame(double vecR0, double vecR1, double vecR2, char frameChar, double vec0, double vec1, double vec2) {};
    };
}

#endif
