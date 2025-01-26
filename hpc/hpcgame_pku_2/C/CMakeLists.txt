cmake_minimum_required(VERSION 3.20)

project(FilterProject LANGUAGES C CXX Fortran)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_Fortran_STANDARD 2003)
set(CMAKE_Fortran_STANDARD_REQUIRED ON)
set(CMAKE_Fortran_EXTENSIONS OFF)

add_executable(program main.cpp filter.F90)

set_source_files_properties(filter.F90 PROPERTIES LANGUAGE Fortran)

if (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    set(CPU_TARGET "icelake-server")

    set(OPT_FLAGS "-O2" "-funroll-loops" "-qopt-zmm-usage=high" "-fp-model=fast" "-qopt-multiple-gather-scatter-by-shuffles" "-qopenmp")
    

    target_compile_options(program PRIVATE
        $<$<COMPILE_LANGUAGE:C>:-x${CPU_TARGET}>
        $<$<COMPILE_LANGUAGE:C>:${OPT_FLAGS}>

        $<$<COMPILE_LANGUAGE:CXX>:-x${CPU_TARGET}>
        $<$<COMPILE_LANGUAGE:CXX>:${OPT_FLAGS}>

        $<$<COMPILE_LANGUAGE:Fortran>:-I${CMAKE_SOURCE_DIR}>
        $<$<COMPILE_LANGUAGE:Fortran>:-x${CPU_TARGET}>
        $<$<COMPILE_LANGUAGE:Fortran>:${OPT_FLAGS}>
    )

    # 可选：启用链接时优化
    target_link_options(program PRIVATE
        -ipo
    )
endif()