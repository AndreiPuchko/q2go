cmake_minimum_required(VERSION 3.14)

set(THIRD_PARTY_DIR ${CMAKE_SOURCE_DIR}/3party)

if (WIN32)
    project(q2go LANGUAGES CXX)
elseif(UNIX)
    project(q2go)
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)

set(ZLIB_ROOT "${THIRD_PARTY_DIR}/zlib.install")
find_package(ZLIB REQUIRED)
add_subdirectory(${THIRD_PARTY_DIR}/quazip)


#=================== INCLUSION OF Project Files ====================#
set(FORMS_DIR "${CMAKE_SOURCE_DIR}/ui")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(RC_DIR "${CMAKE_SOURCE_DIR}/resources")

include_directories(${FORMS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

# add_subdirectory(quazip)

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
add_executable(q2go WIN32 ${SOURCES} ${RC_DIR} ${WIN32_RESOURCES})

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

message("*********************************")
message(${CMAKE_BINARY_DIR})
message(${CMAKE_INSTALL_PREFIX})

if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND $ENV{QtDIR}/bin/windeployqt.exe --release $<TARGET_FILE_DIR:q2go>
        COMMENT "Deploying Qt DLLs with windeployqt"
    )
    # Qt6Core5Compat.dll
    set(Qt6Core5Compat_PATH "$ENV{QtDIR}/bin/Qt6Core5Compat.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${Qt6Core5Compat_PATH}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying Qt6Core5Compat.dll to the output directory"
    )
    # QUAZIP
    set(QUAZIP_DLL_PATH "${CMAKE_BINARY_DIR}/3party/quazip/quazip/libquazip1-qt6.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${QUAZIP_DLL_PATH}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying QuaZip DLL to the output directory"
    )
    # ZLIB
    set(ZLIB_DLL_PATH "${CMAKE_SOURCE_DIR}/3party/zlib.install/bin/libzlib.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${ZLIB_DLL_PATH}"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying ZLib DLL to the output directory"
    )
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "installer")
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION .)
install(DIRECTORY "${CMAKE_BINARY_DIR}/" DESTINATION .
    FILES_MATCHING 
        PATTERN "*.dll" 
        PATTERN "*.exe" 
        PATTERN "*.pdb" 
        PATTERN ".cmake" EXCLUDE
        PATTERN "CMakeFiles" EXCLUDE
        PATTERN ".qt" EXCLUDE
        PATTERN "3party" EXCLUDE
        PATTERN "_deps" EXCLUDE
        PATTERN "q2go_autogen" EXCLUDE
    )

# 
if(WIN32)
    set(CPACK_GENERATOR "ZIP;NSIS")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "DEB;TGZ")
endif()


# Project info
set(CPACK_PACKAGE_NAME "q2go")
set(CPACK_PACKAGE_VENDOR "YourCompanyName")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "The installer/launcher fot the q2rad project")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "andrei.puchko@gmx.de")
set(CPACK_NSIS_DISPLAY_NAME "q2 installer/launcher")
set(CPACK_NSIS_MODIFY_PATH "ON")
# set(CPACK_NSIS_HELP_LINK "https://yourprojectsite.com/help")
# set(CPACK_NSIS_URL_INFO_ABOUT "https://yourprojectsite.com")
# set(CPACK_INSTALL_DIRECTORY "${CMAKE_SOURCE_DIR}/final_release")
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_SOURCE_DIR}/release")
set (CPACK_CREATE_DESKTOP_LINKS ${PROJECT_NAME})

set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\q2go.lnk' '$INSTDIR\\\\q2go.exe' ''"
)
set (CPACK_PACKAGE_INSTALL_DIRECTORY)


include(CPack)
