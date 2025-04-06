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
    CPiELoKheperaIV* controller = reinterpret_cast<CPiELoKheperaIV*>(
        vm->findVariable("controller")->getClosureIndex());
    float dist = controller->getEncoders().GetReading().CoveredDistanceLeftWheel;
    LOG << "[" << controller->GetId() << "] " << "Distance covered: " << dist << std::endl;
    return dist;
}

void CPiELoKheperaIV::Init(TConfigurationNode& t_node) {
    CPiELoController::Init(t_node);
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
    vm.globalSymbolTable["leftWheelVelocity"] = 1.0f;
    vm.globalSymbolTable["rightWheelVelocity"] = 1.0f;
    vm.globalSymbolTable["robotID"] = m_unRobotId;
    vm.globalSymbolTable["controller"] = (size_t) this;
    vm.registerFunction("get_distance_covered", get_distance_covered);
}

void CPiELoKheperaIV::ControlStep() {
    CPiELoController::ControlStep();
    float leftWheelVelocity = vm.globalSymbolTable["leftWheelVelocity"].getFloatValue();
    float rightWheelVelocity = vm.globalSymbolTable["rightWheelVelocity"].getFloatValue();
    m_pcWheels->SetLinearVelocity(leftWheelVelocity, rightWheelVelocity);
    RLOG << "Set wheel speeds to " << leftWheelVelocity << ", " << rightWheelVelocity << std::endl;
}

REGISTER_CONTROLLER(CPiELoKheperaIV, "pielo_khepera_iv")
