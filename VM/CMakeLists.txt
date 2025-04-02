add_subdirectory(instructions)

add_library(PiELo SHARED
    exceptions.h
    gc.cpp gc.h
    instructionHandler.cpp instructionHandler.h
    networking.cpp networking.h
    parser.cpp parser.h
    robotFunctions.cpp robotFunctions.h
    vm.cpp vm.h)

add_executable(vm main.cpp)
target_link_libraries(vm PiELo instructions)

add_executable(router router.cpp)
target_link_libraries(router PiELo)

add_library(controller 
    pieloController.cpp pieloController.h)

target_link_libraries(controller
    argos3core_simulator
    argos3plugin_simulator_genericrobot)