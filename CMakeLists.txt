cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(eqdose)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
#add_definitions(-DG4MULTITHREADED=1)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(eqdose eqdose.cc ${sources} ${headers})
target_link_libraries(eqdose ${Geant4_LIBRARIES})

set(EXAMPLEB3_SCRIPTS
        run.mac
        vis.mac
        init_vis.mac
        )

foreach(_script ${EXAMPLEB3_SCRIPTS})
    configure_file(
            ${PROJECT_SOURCE_DIR}/${_script}
            ${PROJECT_BINARY_DIR}/${_script}
            COPYONLY
    )
endforeach()

