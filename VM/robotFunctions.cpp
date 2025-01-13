#include "robotFunctions.h"
Robot robot;

vec Robot::getRobotPos() {
    return robotPos;
}

vec Robot::getRobotVel() {
    return robotVel;
}

void Robot::setRobotVel(vec v) {
    robotVel = v;
}

// TODO: make this take into account how long it's been since last update
void Robot::updatePos() {
    robotPos.x += robotVel.x;
    robotPos.y += robotVel.y;
    robotPos.z += robotVel.z;
}