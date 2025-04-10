#include "pieloControllerKheperaIV.h"
#include <argos3/core/utility/logging/argos_log.h>

CPiELoKheperaIV::CPiELoKheperaIV() :
    m_pcWheels(NULL),
    m_pcProximity(NULL),
    m_cAlpha(10.0f),
    m_fDelta(0.5f),
    m_fWheelVelocity(2.5f),
    m_cGoStraightAngleRange(-ToRadians(m_cAlpha),
                            ToRadians(m_cAlpha)) {}

CCI_DifferentialSteeringSensor CPiELoKheperaIV::getEncoders() {
    return *m_pcEncoder;
}

PiELo::Variable get_distance_covered(PiELo::VM* vm) {
    // std::cout << "Getting distance covered." << std::endl;
    CPiELoKheperaIV* controller = reinterpret_cast<CPiELoKheperaIV*>(
        vm->findVariable("controller")->getPointer());
    float dist = controller->m_totalDistanceTravelled;
    // LOG << "[" << controller->GetId() << "] " << "Distance covered this step: " << dist << std::endl;
    return dist;
}

void CPiELoKheperaIV::Init(TConfigurationNode& t_node) {
    m_pcWheels    = GetActuator<CCI_DifferentialSteeringActuator>("differential_steering");
    m_pcEncoder   = GetSensor  <CCI_DifferentialSteeringSensor  >("differential_steering");
    m_pcProximity = GetSensor  <CCI_KheperaIVProximitySensor    >("kheperaiv_proximity"  );
    /*
     * Parse the configuration file
     *
     * The user defines this part. Here, the algorithm accepts three
     * parameters and it's nice to put them in the config file so we don't
     * have to recompile if we want to try other settings.
     */
    GetNodeAttributeOrDefault(t_node, "alpha", m_cAlpha, m_cAlpha);
    m_cGoStraightAngleRange.Set(-ToRadians(m_cAlpha), ToRadians(m_cAlpha));
    GetNodeAttributeOrDefault(t_node, "delta", m_fDelta, m_fDelta);
    GetNodeAttributeOrDefault(t_node, "velocity", m_fWheelVelocity, m_fWheelVelocity);
    vm.globalSymbolTable["leftWheelVelocity"] = 1;
    vm.globalSymbolTable["rightWheelVelocity"] = 1;

    vm.registerFunction("get_distance_covered", get_distance_covered);

    CPiELoController::Init(t_node);
}

void CPiELoKheperaIV::ControlStep() {
    CPiELoController::ControlStep();
    // PiELo::Variable leftWheelVelocityVar = vm.globalSymbolTable["leftWheelVelocity"];
    // PiELo::Variable rightWheelVelocityVar = vm.globalSymbolTable["rightWheelVelocity"];
    double leftWheelVelocity;
    double rightWheelVelocity;
    vm.loadToStack("leftWheelVelocity");
    vm.uncache();
    vm.loadToStack("rightWheelVelocity");
    vm.uncache();
    PiELo::Variable rightWheelVelocityVar = vm.stack.top(); vm.stack.pop();
    PiELo::Variable leftWheelVelocityVar = vm.stack.top(); vm.stack.pop();
    if (leftWheelVelocityVar.getType() == PiELo::INT) leftWheelVelocity = leftWheelVelocityVar.getIntValue();
    else leftWheelVelocity = leftWheelVelocityVar.getFloatValue();
    if (rightWheelVelocityVar.getType() == PiELo::INT) rightWheelVelocity = rightWheelVelocityVar.getIntValue();
    else rightWheelVelocity = rightWheelVelocityVar.getFloatValue();
    m_pcWheels->SetLinearVelocity(leftWheelVelocity, rightWheelVelocity);

    m_totalDistanceTravelled += m_pcEncoder->GetReading().CoveredDistanceLeftWheel;
    RLOG << "Set wheel speeds to " << leftWheelVelocity << ", " << rightWheelVelocity << std::endl;
    RLOG << "Total distance covered: " << m_totalDistanceTravelled << std::endl;
}

REGISTER_CONTROLLER(CPiELoKheperaIV, "pielo_khepera_iv")
