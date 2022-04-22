macro(add_svlang_subdirectory name)
    add_llvm_subdirectory(SVLANG TOOL ${name})
endmacro()

macro(add_svlang_library name)
    if(BUILD_SHARED_LIBS)
        set(LIBTYPE SHARED)
    else()
        set(LIBTYPE STATIC)
    endif()

    llvm_add_library(${name} ${LIBTYPE} ${ARGN})

    if(TARGET ${name})
        target_link_libraries(${name} INTERFACE ${LLVM_COMMON_LIBS})
    else()
        add_custom_target(${name})
    endif()
endmacro()

macro(add_svlang_executable name)
    add_llvm_executable(${name} ${ARGN})
endmacro()

