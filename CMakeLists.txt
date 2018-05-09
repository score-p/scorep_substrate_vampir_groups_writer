project(vampir_groups_writer)

cmake_minimum_required(VERSION 3.5)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/scorep_plugin_common/;${CMAKE_MODULE_PATH}")
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/nitro/;${CMAKE_MODULE_PATH}")

file(GLOB SUBMODULE_FILES "${CMAKE_SOURCE_DIR}/nitro/*")
list(LENGTH SUBMODULE_FILES COUNT_NITRO_FILES)
file(GLOB SUBMODULE_FILES "${CMAKE_SOURCE_DIR}/scorep_plugin_common/*")
list(LENGTH SUBMODULE_FILES COUNT_SCOREP_FILES)

if(${COUNT_NITRO_FILES} EQUAL 0 OR ${COUNT_SCOREP_FILES} EQUAL 0)
    message(STATUS "Initializing git submodule")
    execute_process(COMMAND "git" "submodule" "init" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
    execute_process(COMMAND "git" "submodule" "update" WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

find_package(Scorep)
if(SCOREP_FOUND)
    include_directories(${SCOREP_INCLUDE_DIRS})
else()
    message(SEND_ERROR "Scorep was not found but is required!")
endif()

set(NITRO_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(nitro)

SET(SOURCES src/scorep.cpp
            src/groups_writer.cpp
            )        

add_library(scorep_substrate_vampir_groups_writer SHARED  ${SOURCES})

target_include_directories(scorep_substrate_vampir_groups_writer
    PRIVATE
        "${CMAKE_SOURCE_DIR}/include/")
target_link_libraries(scorep_substrate_vampir_groups_writer
    PRIVATE
        Nitro::nitro-log
        Nitro::nitro-env)
target_compile_features(scorep_substrate_vampir_groups_writer
    PUBLIC
        cxx_std_14)
target_compile_options(scorep_substrate_vampir_groups_writer
    PRIVATE $<$<CONFIG:Debug>:-Wall -pedantic -Wextra>)

install(TARGETS scorep_substrate_vampir_groups_writer LIBRARY DESTINATION lib)
