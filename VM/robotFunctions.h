#pragma once


struct vec {
    long x;
    long y;
    long z;
};

class Robot {
    private:
        vec robotPos;
        vec robotVel;
    public:
        vec getRobotPos();
        vec getRobotVel();

        // Set the robot's velocity
        void setRobotVel(vec a);

        // Update the robot's position based on the velocity
        void updatePos();
};

extern Robot robot;