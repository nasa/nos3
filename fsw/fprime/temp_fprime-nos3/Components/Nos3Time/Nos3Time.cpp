// ======================================================================
// \title  Nos3Time.cpp
// \author jstar
// \brief  cpp file for Nos3Time component implementation class
// ======================================================================

#include "Components/Nos3Time/Nos3Time.hpp"
#include "FpConfig.hpp"

#include <Fw/Time/Time.hpp>
#include <ctime>

/* nos engine includes */
#include "Client/CInterface.h"
#include "nos_link.h"


/* Constants used for NOS Engine Time and NOS Engine bus */

#define ENGINE_SERVER_URI       "tcp://nos_engine_server:12000"
#define ENGINE_BUS_NAME         "command"
#define TICKS_PER_SECOND        100
#define POSIX_EPOCH             1760776200 //ADVANCING TIME TO 2025-10-18:00:00 

NE_Bus          *Fprime_Bus;
pthread_mutex_t  Fprime_sim_time_mutex;
NE_SimTime       Fprime_sim_time;
int64_t          Fprime_ticks_per_second;

int flag=0;

void Fprime_NosTickCallback(NE_SimTime time)
{
    pthread_mutex_lock(&Fprime_sim_time_mutex);
    Fprime_sim_time = time;
    pthread_mutex_unlock(&Fprime_sim_time_mutex);
}


namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  Nos3Time::Nos3Time(const char* name) : Nos3TimeComponentBase(name)
    {
    }

    Nos3Time::~Nos3Time() {
    }

    void Nos3Time::timeGetPort_handler(
            NATIVE_INT_TYPE portNum, /*!< The port number*/
            Fw::Time &time /*!< The U32 cmd argument*/
        ) {
        int32_t Nos3Time_upper;
        int32_t Nos3Time_lower;

        if(flag==0){
        Fprime_Bus = NE_create_bus(hub, ENGINE_BUS_NAME, ENGINE_SERVER_URI);
        NE_bus_add_time_tick_callback(Fprime_Bus, Fprime_NosTickCallback);
        flag = 1;
        }

        Nos3Time_upper = static_cast<int32_t>((Fprime_sim_time/100)); //ticks/100 = seconds (1 tick =10000 microseconds)
        Nos3Time_lower = static_cast<int32_t>((Fprime_sim_time % 100)*10000); //10000 for microseconds
        
        Nos3Time_upper += POSIX_EPOCH; //setting to 2025 


        time.set(TB_WORKSTATION_TIME,0, Nos3Time_upper, Nos3Time_lower);
    }

}
