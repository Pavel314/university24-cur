cmake_minimum_required(VERSION 3.29 FATAL_ERROR)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()
if (NOT DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif()

project (SMAD VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

function(set_compiler_flags trgt)
  target_compile_options(${trgt} PRIVATE
    $<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:-Wall;-Wextra;-pedantic>
    $<$<COMPILE_LANG_AND_ID:C,MSVC>:/W4;/utf-8>
  )
  target_compile_definitions(${trgt} PRIVATE UNICODE _UNICODE)
endfunction()

function(add_prog tgt)
	add_executable (${tgt} ${ARGN})
	set_compiler_flags(${tgt})
    if(UNIX)
        target_link_libraries(${tgt} PRIVATE m)
    endif()
endfunction()

add_prog(lr1 "lr1.c")
add_prog(lr2 "lr2.c")
