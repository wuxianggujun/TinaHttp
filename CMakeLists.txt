cmake_minimum_required(VERSION 3.18...3.29 FATAL_ERROR)
project(TinaHttp LANGUAGES C CXX)


set(CMAKE_CXX_STANDARD 17)
include(GNUInstallDirs)
include(FetchContent)

find_package(Threads REQUIRED)

list(APPEND TINA_HTTP_SOURCES
        ${PROJECT_SOURCE_DIR}/src/Http.cpp
        ${PROJECT_SOURCE_DIR}/src/Http.hpp
        ${PROJECT_SOURCE_DIR}/src/Error.hpp
        ${PROJECT_SOURCE_DIR}/src/HttpUtils.hpp
        ${PROJECT_SOURCE_DIR}/src/StatusCode.hpp
        ${PROJECT_SOURCE_DIR}/src/TinaHttp.hpp
)

if (CMAKE_HOST_APPLE)
    add_definitions(-DOS_MACOSX)
elseif (CMAKE_HOST_UNIX)
    add_definitions(-DOS_LINUX)
else (CMAKE_HOST_APPLE)
    message(FATAL_ERROR "Unsupported platform")
endif ()

option(BUILD_TINA_HTTP_SHARED_LIBRARY "Build TinaHttp as a shared library" OFF)
option(BUILD_TINA_HTTP_STATIC_LIBRARY "Build TinaHttp as a shared library" ON)
option(BUILD_TINA_HTTP_EXAMPLES "Build TinaHttp tests" ON)

FetchContent_Declare(llhttp
        URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.2.1.tar.gz")

if (BUILD_TINA_HTTP_SHARED_LIBRARY)
    message(STATUS "Shared library not supported yet")
    set(BUILD_SHARED_LIBS ON CACHE INTERNAL "")
elseif (BUILD_TINA_HTTP_STATIC_LIBRARY)
    message(STATUS "Static library not supported yet")
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
endif ()

FetchContent_MakeAvailable(llhttp)

if (BUILD_TINA_HTTP_SHARED_LIBRARY)
    add_library(${PROJECT_NAME}_shared SHARED ${TINA_HTTP_SOURCES})
    target_include_directories(${PROJECT_NAME}_shared PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${PROJECT_NAME}_shared PUBLIC Threads::Threads llhttp_shared)
    install(TARGETS ${PROJECT_NAME}_shared DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (BUILD_TINA_HTTP_STATIC_LIBRARY)
    add_library(${PROJECT_NAME}_static STATIC ${TINA_HTTP_SOURCES})
    target_include_directories(${PROJECT_NAME}_static PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${PROJECT_NAME}_static PUBLIC Threads::Threads llhttp_static)
    install(TARGETS ${PROJECT_NAME}_static DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif ()

if (BUILD_TINA_HTTP_SHARED_LIBRARY OR BUILD_TINA_HTTP_STATIC_LIBRARY)
    install(FILES
            ${PROJECT_SOURCE_DIR}/src/Http.cpp
            ${PROJECT_SOURCE_DIR}/src/Http.hpp
            ${PROJECT_SOURCE_DIR}/src/Error.hpp
            ${PROJECT_SOURCE_DIR}/src/HttpUtils.hpp
            ${PROJECT_SOURCE_DIR}/src/Status.hpp
            ${PROJECT_SOURCE_DIR}/src/TinaHttp.hpp
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif ()

function(add_tina_executable EXECUTABLE_NAME EXECUTABLE_SOURCES)
    add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCES})
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC ${PROJECT_NAME}_static)
    install(TARGETS ${EXECUTABLE_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()

if (BUILD_TINA_HTTP_EXAMPLES)
    add_tina_executable(TinaHttpServer examples/TinaHttpServer.cpp)
    add_tina_executable(TinaHttpClient examples/TinaHttpClient.cpp)
endif ()
