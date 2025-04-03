
add_library(pieloController SHARED
    pieloController.cpp pieloController.h)

add_library(pielo_khepera_iv MODULE 
    pieloControllerKheperaIV.cpp pieloControllerKheperaIV.h)

target_link_libraries(pieloController
    argos3core_simulator
    argos3plugin_simulator_genericrobot
    PiELo instructions)

target_link_libraries(pielo_khepera_iv
    argos3core_simulator
    argos3plugin_simulator_genericrobot
    PiELo instructions
    pieloController)