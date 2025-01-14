#include "robotFunctions.h"
#include "sys/time.h"
Robot robot;

vec Robot::getRobotPos() {
    return robotPos;
}

vec Robot::getRobotVel() {
    return robotVel;
}

// in units/s
void Robot::setRobotVel(vec v) {
    robotVel = v;
}

timeval lastUpdate = {0, 0};
// TODO: make this take into account how long it's been since last update
void Robot::updatePos() {
    double lastUpdateTime = lastUpdate.tv_sec + lastUpdate.tv_usec / 1000000;
    timeval currTime;
    gettimeofday(&currTime, nullptr);
    double currUpdateTime = currTime.tv_sec + currTime.tv_usec / 1000000;
    double timeDiff = currUpdateTime - lastUpdateTime;
    robotPos.x += robotVel.x * timeDiff;
    robotPos.y += robotVel.y * timeDiff;
    robotPos.z += robotVel.z * timeDiff;
    gettimeofday(&lastUpdate, nullptr);
}