#include "vm.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Please provide a filename\n");
        exit(-1);
    }
    PiELo::load(argv[1]);
    while(PiELo::step() == PiELo::VMState::READY);
    printf("Done!\n");
}