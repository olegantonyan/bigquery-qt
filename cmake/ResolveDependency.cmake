include_guard(GLOBAL)
include(ExternalProject)

option(BQQT_VENDOR_ALL "Force every manageable dependency to build from 3rdparty (vendored)" OFF)

set(BQQT_EP_COMMON_ARGS
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}"
    "-DCMAKE_CXX_STANDARD=17"
    "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
    "-DBUILD_SHARED_LIBS=OFF"
    "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
    "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")

function(bqqt_resolve_dependency)
    cmake_parse_arguments(ARG "" "KEY;PACKAGE;SOURCE_DIR" "DEPENDS;CMAKE_ARGS" ${ARGN})
    string(TOUPPER "${ARG_KEY}" up)

    set(BQQT_${up}_SOURCE "auto" CACHE STRING "Source for ${ARG_KEY}: auto|system|vendored")
    set_property(CACHE BQQT_${up}_SOURCE PROPERTY STRINGS auto system vendored)

    set(requested "${BQQT_${up}_SOURCE}")
    if (BQQT_VENDOR_ALL)
        set(requested vendored)
    endif ()

    if (requested STREQUAL "system")
        set(provider system)
    elseif (requested STREQUAL "vendored")
        set(provider vendored)
    else ()
        find_package(${ARG_PACKAGE} CONFIG QUIET)
        if (${ARG_PACKAGE}_FOUND)
            set(provider system)
        else ()
            set(provider vendored)
        endif ()
    endif ()

    if (provider STREQUAL "vendored")
        set(ep_depends "")
        foreach (d ${ARG_DEPENDS})
            string(TOUPPER "${d}" dup)
            if (BQQT_${dup}_PROVIDER STREQUAL "vendored")
                list(APPEND ep_depends ep_${d})
            endif ()
        endforeach ()

        ExternalProject_Add(ep_${ARG_KEY}
            SOURCE_DIR "${ARG_SOURCE_DIR}"
            PREFIX "${CMAKE_BINARY_DIR}/ep/${ARG_KEY}"
            LIST_SEPARATOR "|"
            CMAKE_ARGS ${BQQT_EP_COMMON_ARGS} ${ARG_CMAKE_ARGS}
            DEPENDS ${ep_depends}
            USES_TERMINAL_CONFIGURE ON
            USES_TERMINAL_BUILD ON
            USES_TERMINAL_INSTALL ON)
        set_property(GLOBAL APPEND PROPERTY BQQT_VENDORED_EPS ep_${ARG_KEY})
        set_property(GLOBAL APPEND PROPERTY BQQT_RESOLVE_SUMMARY "${ARG_KEY}: vendored (${ARG_SOURCE_DIR})")
    else ()
        set_property(GLOBAL APPEND PROPERTY BQQT_RESOLVE_SUMMARY "${ARG_KEY}: system  (find_package(${ARG_PACKAGE}))")
    endif ()

    set(BQQT_${up}_PROVIDER "${provider}" PARENT_SCOPE)
    message(STATUS "bqqt: dependency '${ARG_KEY}' -> ${provider}")
endfunction()
