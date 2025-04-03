#include "pieloController.h"

void CPiELoController::Init(TConfigurationNode& t_node) {
    try {
      std::cout << " test print " << std::endl;
       /* Get pointers to devices */
       m_pcRABA   = GetActuator<CCI_RangeAndBearingActuator>("range_and_bearing");
       m_pcRABS   = GetSensor  <CCI_RangeAndBearingSensor  >("range_and_bearing");
       try {
          m_pcPos = GetSensor  <CCI_PositioningSensor>("positioning");
       }
       catch(CARGoSException& ex) {}
       try {
          m_pcBattery = GetSensor<CCI_BatterySensor>("battery");
       }
       catch(CARGoSException& ex) {}
       /* Get the script name */
       std::string strBCFName;
       GetNodeAttributeOrDefault(t_node, "bytecode_file", strBCFName, strBCFName);
       /* Initialize the rest */
       bool bIDSuccess = false;
       m_unRobotId = 0;
       /* Find Buzz ID */
       size_t tStartPos = GetId().find_last_of("_");
       if(tStartPos != std::string::npos){
          /* Checks for ID after last "_" ie. footbot_group3_10 -> 10 */
          m_unRobotId = FromString<UInt16>(GetId().substr(tStartPos+1));
          bIDSuccess = true;
       }
       /* FromString() returns 0 if passed an invalid string */
       if(!m_unRobotId || !bIDSuccess){
          /* Checks for ID after first number footbot_simulated10 -> 10 */
          tStartPos = GetId().find_first_of("0123456789");
          if(tStartPos != std::string::npos){
             m_unRobotId = FromString<UInt16>(GetId().substr(tStartPos));
             bIDSuccess = true;
          }
       }
       if(!bIDSuccess) {
          THROW_ARGOSEXCEPTION("Error finding Buzz ID from name \"" << GetId() << "\"");
       }
       if(strBCFName != "")
          PiELo::load(strBCFName);
       else {
          THROW_ARGOSEXCEPTION("No bytecode file name!");
       }
    }
    catch(CARGoSException& ex) {
       THROW_ARGOSEXCEPTION_NESTED("Error initializing the Buzz controller", ex);
    }
 }
 
 /****************************************/
 /****************************************/
 
 void CPiELoController::Reset() {
   THROW_ARGOSEXCEPTION("No reset function implemented");
   //  if(buzzvm_function_call(m_tBuzzVM, "reset", 0) != BUZZVM_STATE_READY) {
   //     fprintf(stderr, "[ROBOT %u] %s: execution terminated abnormally: %s\n\n",
   //             m_tBuzzVM->robot,
   //             m_strBytecodeFName.c_str(),
   //             ErrorInfo().c_str());
   //     for(UInt32 i = 1; i <= buzzdarray_size(m_tBuzzVM->stacks); ++i) {
   //        buzzdebug_stack_dump(m_tBuzzVM, i, stdout);
   //     }
   //     return;
   //  }
   //  /* Reset debug information */
   //  m_sDebug.Clear();
   //  m_sDebug.TrajectoryClear();
   //  m_sDebug.TrajectoryDisable();
   //  m_sDebug.RayClear();
   //  try {
   //     /* Set the bytecode again */
   //     if(m_strBytecodeFName != "" && m_strDbgInfoFName != "")
   //        SetBytecode(m_strBytecodeFName, m_strDbgInfoFName);
   //     else
   //        UpdateSensors();
   //  }
   //  catch(CARGoSException& ex) {
   //     LOGERR << ex.what();
   //  }
 }


 void CPiELoController::ControlStep() {
   if(PiELo::state == PiELo::VMState::READY) {
      PiELo::step();
   }
   else {
      fprintf(stderr, "[ROBOT %s] Robot is not ready to execute PiELo script.\n\n",
              GetId().c_str());
   }
}

