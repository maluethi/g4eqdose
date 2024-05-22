add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(run_tests ConvCoeffTest.cc)

configure_file("Klapper_Effective dose.txt" ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

target_link_libraries(run_tests gtest gtest_main eqdose_lib ${Geant4_LIBRARIES})


