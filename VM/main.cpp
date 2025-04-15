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

PiELo::Variable get_distance_covered(PiELo::VM* vm) {
    return 0.1f;
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
    vm.registerFunction("get_distance_covered", &get_distance_covered);
    vm.globalSymbolTable["robotID"] = 4;
    vm.load(argv[1]);
    while(vm.step() == PiELo::VM::VMState::READY && 
        (vm.bytecode[vm.programCounter].type != PiELo::opCodeInstructionOrArgument::INSTRUCTION || vm.bytecode[vm.programCounter].asInstruction != PiELo::SPIN));
    size_t initialIndex = vm.currentClosureIndex;
    vm.stack.push(0);
    vm.loadToStack("step");
    vm.callClosure();
    vm.programCounter++;
    while(vm.currentClosureIndex != initialIndex && vm.state == PiELo::VM::VMState::READY) {
        vm.step();
    }
    vm.stack.pop(); // Pop the return value of step
    vm.garbageCollector.collectGarbage(&vm);

    vm.loadToStack("leftWheelVelocity");
    std::cout << " Loaded leftWheelVelocity to stack with type " << vm.stack.top().getTypeAsString() << " closure index: " << vm.stack.top().getClosureIndex() << std::endl;
    std::cout << " Cached value has type " << vm.closureList[vm.stack.top().getClosureIndex()].cachedValue.getTypeAsString() << std::endl;
    printf("Done!\n");
}