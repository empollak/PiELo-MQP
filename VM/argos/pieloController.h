#ifndef PIELO_CONTROLLER_H
#define PIELO_CONTROLLER_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>
#include <argos3/core/utility/math/ray3.h>
#include <argos3/core/utility/datatypes/set.h>
#include "../vm.h"

using namespace argos;

class CPiELoController : public CCI_Controller {
public:
    CPiELoController() {}
    virtual ~CPiELoController() {}

    virtual void Init(TConfigurationNode& t_node);
    virtual void Reset();
    virtual void ControlStep();

    inline const std::string& GetBytecodeFName() const {
        return m_strBytecodeFName;
    }

    protected:

    /* Pointer to the range and bearing actuator */
    CCI_RangeAndBearingActuator*  m_pcRABA;
    /* Pointer to the range and bearing sensor */
    CCI_RangeAndBearingSensor* m_pcRABS;
    /* Pointer to the positioning sensor */
    CCI_PositioningSensor* m_pcPos;
    /* Pointer to the battery sensor */
    CCI_BatterySensor* m_pcBattery;
 
    /* The robot numeric id */
    UInt16 m_unRobotId;
    /* Name of the bytecode file */
    std::string m_strBytecodeFName;


    PiELo::VM vm;
};


#endif