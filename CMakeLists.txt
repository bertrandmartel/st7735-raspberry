cmake_minimum_required(VERSION 3.0)
project(st7735 LANGUAGES CXX VERSION 1.0)
set(example_dir "out/examples")
set (CMAKE_CXX_STANDARD 11)
include(GNUInstallDirs)
add_library(st7735 
    SHARED 
    lib/st7735.cpp
    lib/common.cpp
)
set_target_properties(st7735 PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    LIBRARY_OUTPUT_DIRECTORY out
    PUBLIC_HEADER lib/st7735.h)
 
target_include_directories(st7735 PRIVATE lib)

set (WPI_PATH ./raspberry-dev/wiringPi/wiringPi)
include_directories (include ${WPI_PATH})

#library install
install(TARGETS st7735
    LIBRARY DESTINATION /usr/lib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#link wiringpi
find_library(WPI_LIB wiringPi HINTS ${WPI_PATH} NO_CMAKE_FIND_ROOT_PATH)
if(NOT WPI_LIB)
message(FATAL_ERROR "wiringPi library not found")
endif()

#iterate examples
file( GLOB EXAMPLES_SRC examples/*.cpp )
foreach( testsourcefile ${EXAMPLES_SRC} )
	get_filename_component(base_name ${testsourcefile} NAME_WE)
    add_executable(${base_name} ${testsourcefile})
    message(${base_name})
    set_target_properties(${base_name} 
    	PROPERTIES 
    		OUTPUT_NAME ${base_name}
    		RUNTIME_OUTPUT_DIRECTORY ${example_dir})
    target_link_libraries(${base_name} ${WPI_LIB})
    target_link_libraries( ${base_name} st7735 )
    target_link_libraries( ${base_name} pthread )
    target_include_directories(${base_name} PUBLIC lib)
endforeach( testsourcefile ${EXAMPLES_SRC} )