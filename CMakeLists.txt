cmake_minimum_required(VERSION 3.14)

if (WIN32)
    project(q2go LANGUAGES CXX)
elseif(UNIX)
    project(q2go)
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)
set(CMAKE_INSTALL_PREFIX )

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/build/deploy")
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)

# add_subdirectory(./deps/fmt)

#=================== INCLUSION OF Project Files ====================#
set(FORMS_DIR "${CMAKE_SOURCE_DIR}/ui")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(RC_DIR "${CMAKE_SOURCE_DIR}/resources")

include_directories(${FORMS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

add_subdirectory(quazip)

file(GLOB_RECURSE SOURCES
    "${FORMS_DIR}/*.ui"
    "${FORMS_DIR}/*.qrc"
    "${INCLUDE_DIR}/*.h"
    "${SOURCE_DIR}/*.cpp"
    "${RC_DIR}/*.qrc"
)

#=================== SETUP EXECTUABLE ====================#
# Enable debug logging on RELWITHDEBINFO configuration
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/resources/resources.rc)
endif()


# Add the executable
if (WIN32)
    # add_executable(q2go WIN32 ${SOURCES} ${WIN32_RESOURCES})
    add_executable(q2go WIN32 ${SOURCES} ${RC_DIR} ${WIN32_RESOURCES})
elseif(UNIX)
    target_sources(q2go
    PRIVATE
        src/q2mess.h src/q2mess.cpp
    )
    add_executable(q2go ${SOURCES})
endif()

# Add the target includes for q2go 
target_include_directories(q2go PRIVATE ${FORMS_DIR})
target_include_directories(q2go PRIVATE ${INCLUDE_DIR})
target_include_directories(q2go PRIVATE ${SOURCE_DIR})
target_include_directories(q2go PRIVATE ${RC_DIR})


#===================== LINKING LIBRARIES =======================#
target_link_libraries(q2go Qt6::Core)
target_link_libraries(q2go Qt6::Widgets)
target_link_libraries(q2go Qt6::Network)
target_link_libraries(q2go QuaZip::QuaZip)
