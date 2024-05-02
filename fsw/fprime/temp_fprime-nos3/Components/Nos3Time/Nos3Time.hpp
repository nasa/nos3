// ======================================================================
// \title  Nos3Time.hpp
// \author jstar
// \brief  hpp file for Nos3Time component implementation class
// ======================================================================

#ifndef Components_Nos3Time_HPP
#define Components_Nos3Time_HPP

#include "Components/Nos3Time/Nos3TimeComponentAc.hpp"

namespace Components {

  class Nos3Time: public Nos3TimeComponentBase {
    public:
        explicit Nos3Time(const char* compName);
        virtual ~Nos3Time();
    protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
};

}

#endif
