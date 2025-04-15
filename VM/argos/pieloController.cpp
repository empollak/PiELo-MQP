#include "pieloController.h"
#include <exception>
#include <argos3/core/utility/logging/argos_log.h>


void CPiELoController::Init(TConfigurationNode& t_node) {
    try {
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
          THROW_ARGOSEXCEPTION("Error finding bID from name \"" << GetId() << "\"");
       }
       if(strBCFName != "") {
         vm.globalSymbolTable["robotID"] = m_unRobotId;
         vm.globalSymbolTable["controller"] = (void*) this;
         vm.load(strBCFName);
         RLOG << "Running VM until it spins..." << std::endl;
         while(vm.bytecode[vm.programCounter].asInstruction != PiELo::Instruction::SPIN && vm.state == PiELo::VM::READY) takeVMStep();
       } 
       else {
          THROW_ARGOSEXCEPTION("No bytecode file name test!");
       }
       RLOG << "PiELo initialization complete. Controller: " << (size_t) this << std::endl;
    }
    catch(CARGoSException& ex) {
       THROW_ARGOSEXCEPTION_NESTED("Error initializing the PiELo controller", ex);
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

 void CPiELoController::takeVMStep() {
   if(vm.state == PiELo::VM::VMState::READY) {
      // RLOG << "Stepping. Bytecode size: " << vm.bytecode.size() << " top type: " << vm.bytecode[0].getTypeAsString() << std::endl;
      try {
         // RLOG << "In try block" << std::endl;
         vm.state = vm.step();
      } catch (std::runtime_error e) {
         vm.state = PiELo::VM::VMState::ERROR;
         RLOG << "I have crashed! PC: " << vm.programCounter << ", bytecode size: " << vm.bytecode.size() << "Exception: " << e.what() << std::endl;
         // THROW_ARGOSEXCEPTION(("VM!!!! " + std::string(e.what())))
         // THROW_ARGOSEXCEPTION_NESTED("vm", e)
         // std::cout << "Exception: " << e.what() << std::endl;
      } catch (std::out_of_range e) {
         THROW_ARGOSEXCEPTION(("[" + GetId() + "]" + " Out of range: " + std::string(e.what()) + ". Address of controller: " + std::to_string((size_t) this)))
      }
   }
   else {
      fprintf(stderr, "[ROBOT %s] Robot is not ready to execute PiELo script. VM State: %d\n\n",
              GetId().c_str(), vm.state);
      
   }
 } 

void CPiELoController::runVMFunction(std::string name) {
   RLOG << "Running function " << name << std::endl;
   size_t initialIndex = vm.currentClosureIndex;
   vm.stack.push(0);
   vm.loadToStack(name);
   vm.callClosure();
   vm.programCounter++;
   while(vm.currentClosureIndex != initialIndex && vm.state == PiELo::VM::VMState::READY) {
      takeVMStep();
   }
   vm.stack.pop(); // Pop the return value of step
   vm.garbageCollector.collectGarbage(&vm);
   if (vm.state != PiELo::VM::READY) {
      RLOGERR << "Attempted to run VM function " << name << " while VM is in state " << vm.state;
   }
}

 void CPiELoController::ControlStep() {
   // Clear out all of the new messages
   bool gotNewMessage;
   do {
      RLOG << "Clearing out messages. " << std::endl;
      size_t initialIndex = vm.currentClosureIndex;
      gotNewMessage = vm.network.checkForMessage();
      bool reactivityHappened = gotNewMessage && vm.currentClosureIndex != initialIndex;
      // If we got a message and there's reactivity afoot, increment the PC by one to get to the start of its closure!
      if (reactivityHappened) vm.programCounter++;
      while(gotNewMessage && vm.currentClosureIndex != initialIndex && vm.state == PiELo::VM::VMState::READY) {
         RLOG << " Taking step for message " << std::endl;
         takeVMStep();
      }
      if (reactivityHappened) {
         vm.stack.pop(); // Pop return value
         vm.garbageCollector.collectGarbage(&vm);
      }
   } while (gotNewMessage);
   runVMFunction("step");
}

/*
 * This statement notifies ARGoS of the existence of the controller.
 * It binds the class passed as first argument to the string passed as
 * second argument.
 * The string is then usable in the configuration file to refer to this
 * controller.
 * When ARGoS reads that string in the configuration file, it knows which
 * controller class to instantiate.
 * See also the configuration files for an example of how this is used.
 */
REGISTER_CONTROLLER(CPiELoController, "pieloController")
