cmake_minimum_required(VERSION 3.22)
project(bigquery-qt VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BQQT_DEPS_PREFIX "${CMAKE_BINARY_DIR}/3rdparty-install")
set(BQQT_DEPS_JOBS "" CACHE STRING "Parallel jobs for the one-time dependency build (empty = all cores)")

if (NOT EXISTS "${BQQT_DEPS_PREFIX}/lib64/cmake/google_cloud_cpp_bigquerycontrol"
        AND NOT EXISTS "${BQQT_DEPS_PREFIX}/lib/cmake/google_cloud_cpp_bigquerycontrol")
    message(STATUS "bigquery-qt: building vendored dependencies into ${BQQT_DEPS_PREFIX} (first build, slow)")
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/3rdparty")
    execute_process(
        COMMAND "${CMAKE_COMMAND}"
            -S "${CMAKE_SOURCE_DIR}/3rdparty" -B "${CMAKE_BINARY_DIR}/3rdparty"
            -G "${CMAKE_GENERATOR}"
            "-DCMAKE_INSTALL_PREFIX=${BQQT_DEPS_PREFIX}"
            "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
            "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
        COMMAND_ERROR_IS_FATAL ANY)
    set(_deps_build "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}/3rdparty")
    if (BQQT_DEPS_JOBS)
        list(APPEND _deps_build --parallel "${BQQT_DEPS_JOBS}")
    endif ()
    execute_process(COMMAND ${_deps_build} COMMAND_ERROR_IS_FATAL ANY)
endif ()

list(PREPEND CMAKE_PREFIX_PATH "${BQQT_DEPS_PREFIX}")

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Concurrent)
find_package(google_cloud_cpp_bigquerycontrol REQUIRED)

qt_standard_project_setup()

qt_add_executable(bigquery-qt
    WIN32 MACOSX_BUNDLE
    app/main.cpp
    app/mainwindow.cpp
    app/mainwindow.h
    app/mainwindow.ui
    app/querytab.cpp
    app/querytab.h
    app/bigqueryclient.cpp
    app/bigqueryclient.h
)

target_link_libraries(bigquery-qt
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Concurrent
        google-cloud-cpp::experimental-bigquerycontrol
)
