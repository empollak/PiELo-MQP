#include "vm.h"
#include "robotFunctions.h"

int doNothingButTalkAboutIt() {
    std::cout << "doing nothing!!" << std::endl;
    
    return 0;
}

int goForward() {
    robot.setRobotVel({1, 0, 0});
    return 0;
}

int printRobotPos() {
    vec pos = robot.getRobotPos();
    std::cout << "Robot pos: x = " << pos.x << " y = " << pos.y << " z = " << pos.z << std::endl;
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Please provide a filename\n");
        exit(-1);
    }
    PiELo::registerFunction("do_nothing", &doNothingButTalkAboutIt);
    PiELo::registerFunction("go_forward", &goForward);
    PiELo::registerFunction("print_robot_pos", &printRobotPos);
    PiELo::load(argv[1]);
    while(PiELo::step() == PiELo::VMState::READY);
    printf("Done!\n");
}