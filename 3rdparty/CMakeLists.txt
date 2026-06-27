cmake_minimum_required(VERSION 3.22)
project(bigquery-qt-deps LANGUAGES C CXX)

get_filename_component(_repo "${CMAKE_SOURCE_DIR}/.." ABSOLUTE)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${_repo}/build/deps-install" CACHE PATH "Where built dependencies are installed" FORCE)
endif ()

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()

list(APPEND CMAKE_MODULE_PATH "${_repo}/cmake")
include(ExternalProject)
include(ResolveDependency)

set(TP "${CMAKE_SOURCE_DIR}")

# Order matters: DEPENDS only wires a prerequisite that resolved earlier.
bqqt_resolve_dependency(KEY abseil PACKAGE absl
    SOURCE_DIR "${TP}/abseil-cpp-20260526.0"
    CMAKE_ARGS -DABSL_PROPAGATE_CXX_STD=ON -DABSL_ENABLE_INSTALL=ON -DBUILD_TESTING=OFF)

bqqt_resolve_dependency(KEY protobuf PACKAGE Protobuf
    SOURCE_DIR "${TP}/protobuf-35.1"
    DEPENDS abseil
    CMAKE_ARGS -Dprotobuf_BUILD_TESTS=OFF -Dprotobuf_ABSL_PROVIDER=package -Dprotobuf_INSTALL=ON)

bqqt_resolve_dependency(KEY grpc PACKAGE gRPC
    SOURCE_DIR "${TP}/grpc-1.81.1"
    DEPENDS abseil protobuf
    CMAKE_ARGS -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF
               -DgRPC_ABSL_PROVIDER=package -DgRPC_PROTOBUF_PROVIDER=package
               -DgRPC_SSL_PROVIDER=package -DgRPC_ZLIB_PROVIDER=package
               -DgRPC_CARES_PROVIDER=package -DgRPC_RE2_PROVIDER=package)

bqqt_resolve_dependency(KEY nlohmann_json PACKAGE nlohmann_json
    SOURCE_DIR "${TP}/json-3.12.0"
    CMAKE_ARGS -DJSON_BuildTests=OFF)

bqqt_resolve_dependency(KEY opentelemetry-cpp PACKAGE opentelemetry-cpp
    SOURCE_DIR "${TP}/opentelemetry-cpp-1.27.0"
    CMAKE_ARGS -DBUILD_TESTING=OFF -DWITH_BENCHMARK=OFF -DWITH_EXAMPLES=OFF
               -DWITH_FUNC_TESTS=OFF -DOPENTELEMETRY_INSTALL=ON -DWITH_STL=CXX17)

set(_trio "${BQQT_ABSEIL_PROVIDER};${BQQT_PROTOBUF_PROVIDER};${BQQT_GRPC_PROVIDER}")
list(REMOVE_DUPLICATES _trio)
list(LENGTH _trio _trio_n)
if (_trio_n GREATER 1)
    message(WARNING
        "abseil/protobuf/gRPC resolved from mixed sources "
        "(abseil=${BQQT_ABSEIL_PROVIDER}, protobuf=${BQQT_PROTOBUF_PROVIDER}, grpc=${BQQT_GRPC_PROVIDER}). "
        "These are ABI-version-coupled; consider -DBQQT_VENDOR_ALL=ON or set all three to the same source.")
endif ()

get_property(_veps GLOBAL PROPERTY BQQT_VENDORED_EPS)

ExternalProject_Add(ep_google_cloud_cpp
    SOURCE_DIR "${TP}/google-cloud-cpp-3.6.0"
    PREFIX "${CMAKE_BINARY_DIR}/ep/google-cloud-cpp"
    LIST_SEPARATOR "|"
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_CXX_STANDARD=17
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DBUILD_TESTING=OFF
        -DGOOGLE_CLOUD_CPP_WITH_MOCKS=OFF
        -DGOOGLE_CLOUD_CPP_ENABLE_EXAMPLES=OFF
        "-DGOOGLE_CLOUD_CPP_ENABLE=bigquery|bigquerycontrol|opentelemetry"
    DEPENDS ${_veps}
    USES_TERMINAL_CONFIGURE ON
    USES_TERMINAL_BUILD ON
    USES_TERMINAL_INSTALL ON)

get_property(_summary GLOBAL PROPERTY BQQT_RESOLVE_SUMMARY)
message(STATUS "================ bigquery-qt dependencies ================")
foreach (line ${_summary})
    message(STATUS "  ${line}")
endforeach ()
message(STATUS "  google-cloud-cpp: vendored (${TP}/google-cloud-cpp-3.6.0)")
message(STATUS "  install prefix:   ${CMAKE_INSTALL_PREFIX}")
message(STATUS "=========================================================")
