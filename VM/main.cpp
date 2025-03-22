#include "vm.h"
#include "robotFunctions.h"
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/time.h>

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

int randomSleep() {
    int maxSleepUs = 3000000;
    int sleepTime = rand() % maxSleepUs;
    std::cout << "Sleeping for " << sleepTime / 1000.0 << "ms " << std::endl;
    usleep(sleepTime);
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Please provide a filename\n");
        exit(-1);
    }
    // Get a random seed based on the microsecond-precision time this program is running
    struct timeval time;
    gettimeofday(&time, NULL);
    srand(time.tv_sec * time.tv_usec);

    PiELo::registerFunction("do_nothing", &doNothingButTalkAboutIt);
    PiELo::registerFunction("go_forward", &goForward);
    PiELo::registerFunction("print_robot_pos", &printRobotPos);
    PiELo::registerFunction("random_sleep", &randomSleep);
    PiELo::load(argv[1]);
    while(PiELo::step() == PiELo::VMState::READY);
    printf("Done!\n");
}