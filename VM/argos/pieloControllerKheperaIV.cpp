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
    PiELo::globalSymbolTable["leftWheelVelocity"] = 1.0f;
    PiELo::globalSymbolTable["rightWheelVelocity"] = 1.0f;
    PiELo::globalSymbolTable["robotID"] = m_unRobotId;
}

void CPiELoKheperaIV::ControlStep() {
    float leftWheelVelocity = PiELo::globalSymbolTable["leftWheelVelocity"].getFloatValue();
    float rightWheelVelocity = PiELo::globalSymbolTable["rightWheelVelocity"].getFloatValue();
    m_pcWheels->SetLinearVelocity(leftWheelVelocity, rightWheelVelocity);
    RLOG << "Set wheel speeds to " << leftWheelVelocity << ", " << rightWheelVelocity << std::endl;
}

REGISTER_CONTROLLER(CPiELoKheperaIV, "pielo_khepera_iv")
