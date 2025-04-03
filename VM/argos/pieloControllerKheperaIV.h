#ifndef PIELO_CONTROLLER_KHEPIV_H
#define PIELO_CONTROLLER_KHEPIV_H

#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_leds_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_actuator.h>
#include <argos3/plugins/robots/generic/control_interface/ci_range_and_bearing_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_battery_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_sensor.h>
#include <argos3/plugins/robots/kheperaiv/control_interface/ci_kheperaiv_proximity_sensor.h>
#include <argos3/core/utility/math/ray3.h>
#include <argos3/core/utility/datatypes/set.h>
#include "../vm.h"
#include "pieloController.h"


using namespace argos;

class CPiELoKheperaIV : public CPiELoController {
    public:
        CPiELoKheperaIV();

        virtual void Init(TConfigurationNode& t_node);

        /*
        * This function is called once every time step.
        * The length of the time step is set in the XML file.
        */
        virtual void ControlStep();

    private:
        /* Pointer to the differential steering actuator */
        CCI_DifferentialSteeringActuator* m_pcWheels;
        /* Pointer to the differential steering sensor */
        CCI_DifferentialSteeringSensor* m_pcEncoder;
        /* Pointer to the Khepera IV proximity sensor */
        CCI_KheperaIVProximitySensor* m_pcProximity;

        /*
         * The following variables are used as parameters for the
         * algorithm. You can set their value in the <parameters> section
         * of the XML configuration file, under the
         * <controllers><footbot_diffusion_controller> section.
         */

         /* Maximum tolerance for the angle between
          * the robot heading direction and
          * the closest obstacle detected. */
        CDegrees m_cAlpha;
        /* Maximum tolerance for the proximity reading between
         * the robot and the closest obstacle.
         * The proximity reading is 0 when nothing is detected
         * and grows exponentially to 1 when the obstacle is
         * touching the robot.
         */
        Real m_fDelta;
        /* Wheel speed. */
        Real m_fWheelVelocity;
        /* Angle tolerance range to go straight.
         * It is set to [-alpha,alpha]. */
        CRange<CRadians> m_cGoStraightAngleRange;

        PiELo::Variable push_alpha();
        PiELo::Variable push_delta();
        PiELo::Variable push_velocity();
        PiELo::Variable push_id();
};

#endif