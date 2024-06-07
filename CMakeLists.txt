cmake_minimum_required(VERSION 3.7)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

message("Build type is : ${CMAKE_BUILD_TYPE}")

project(rtsp_relay VERSION 1.0 LANGUAGES CXX)

find_package(Threads REQUIRED)
find_package(jsoncpp REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0)
pkg_check_modules(gstreamer-rtsp-server REQUIRED IMPORTED_TARGET gstreamer-rtsp-server-1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/config)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cc)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads PkgConfig::gstreamer PkgConfig::gstreamer-app PkgConfig::gstreamer-rtsp-server jsoncpp_lib)
target_compile_definitions(${PROJECT_NAME} PUBLIC CONFIG_PATH=\"${CONFIG_PATH}/\")


include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
        EXPORT "${PROJECT_NAME}Targets"
        RUNTIME DESTINATION bin COMPONENT Binaries
        LIBRARY DESTINATION lib COMPONENT Libraries
)
install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/config
        DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}
)