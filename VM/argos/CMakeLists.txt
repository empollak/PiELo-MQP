
add_library(pieloController MODULE
    pieloController.cpp pieloController.h)

target_link_libraries(pieloController
    argos3core_simulator
    argos3plugin_simulator_genericrobot
    PiELo instructions)