cmake_minimum_required (VERSION 3.29 FATAL_ERROR)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("KPO" VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")
#TODO предупреждение "переопределение "/MDd" на "/MTd""

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

function(kpo_add)
	foreach(arg IN LISTS ARGN)
		add_executable (${arg} "${arg}.cpp" "utils.hpp")
	endforeach()
endfunction()

kpo_add(lr7_10 lr7_20 lr7_30 lr7_40)
kpo_add(lr8_4 lr8_20 lr8_29 lr8_36)
kpo_add(lr9)
kpo_add(lr10_10 lr10_11)