#include "vm.h"
#include "robotFunctions.h"
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

PiELo::Variable doNothingButTalkAboutIt(PiELo::VM* vm) {
    std::cout << "doing nothing!!" << std::endl;
    
    return 0;
}

PiELo::Variable goForward(PiELo::VM* vm) {
    robot.setRobotVel({1, 0, 0});
    return 0;
}

PiELo::Variable printRobotPos(PiELo::VM* vm) {
    vec pos = robot.getRobotPos();
    std::cout << "Robot pos: x = " << pos.x << " y = " << pos.y << " z = " << pos.z << std::endl;
    return 0;
}

PiELo::Variable randomSleep(PiELo::VM* vm) {
    int maxSleepUs = 3000000;
    int sleepTime = rand() % maxSleepUs;
    std::cout << "Sleeping for " << sleepTime / 1000.0 << "ms " << std::endl;
    usleep(sleepTime);
    return 0;
}
PiELo::VM vm;

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Please provide a filename\n");
        exit(-1);
    }
    // Get a random seed based on the microsecond-precision time this program is running
    struct timeval time;
    gettimeofday(&time, NULL);
    srand(time.tv_sec * time.tv_usec);

    vm.registerFunction("do_nothing", &doNothingButTalkAboutIt);
    vm.registerFunction("go_forward", &goForward);
    vm.registerFunction("print_robot_pos", &printRobotPos);
    vm.registerFunction("random_sleep", &randomSleep);
    vm.load(argv[1]);
    while(vm.step() == PiELo::VM::VMState::READY);
    printf("Done!\n");
}