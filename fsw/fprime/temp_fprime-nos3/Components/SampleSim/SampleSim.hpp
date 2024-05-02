// ======================================================================
// \title  SampleSim.hpp
// \author jstar
// \brief  hpp file for SampleSim component implementation class
// ======================================================================

#ifndef Components_SampleSim_HPP
#define Components_SampleSim_HPP

#include "Components/SampleSim/SampleSimComponentAc.hpp"

namespace Components {

  class SampleSim :
    public SampleSimComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Component construction and destruction
      // ----------------------------------------------------------------------

      //! Construct SampleSim object
      SampleSim(
          const char* const compName //!< The component name
      );

      //! Destroy SampleSim object
      ~SampleSim();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for commands
      // ----------------------------------------------------------------------
      U32 m_greetingCount;
      //! Handler implementation for command SAY_HELLO
      //!
      //! Command to issue greeting with maximum length of 20 characters
      void SAY_HELLO_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& greeting //!< Greeting to repeat in the Hello event
      ) override;

      void REQUEST_HOUSEKEEPING_cmdHandler(
        FwOpcodeType opCode, 
        U32 cmdSeq
      ) override;

      void NOOP_cmdHandler(
        FwOpcodeType opCode, 
        U32 cmdSeq
      )override;

  };

}

#endif
