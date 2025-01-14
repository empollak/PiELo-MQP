#include "vm.h"

int doNothingButTalkAboutIt() {
    std::cout << "doing nothing!!" << std::endl;
    
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Please provide a filename\n");
        exit(-1);
    }
    PiELo::registerFunction("do_nothing", &doNothingButTalkAboutIt);
    PiELo::load(argv[1]);
    while(PiELo::step() == PiELo::VMState::READY);
    printf("Done!\n");
}