# CMakeList.txt: проект CMake для labs; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
#https://learn.microsoft.com/en-us/cpp/build/cmake-presets-vs?view=msvc-170#select-your-compilers:
#https://www.kitware.com/import-std-in-cmake-3-30/
#https://stackoverflow.com/questions/77319107/how-to-do-import-std-with-cmake

cmake_minimum_required (VERSION 3.29 FATAL_ERROR)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (aisd VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")

add_executable (lr1 "lr1.cpp" "utils.hpp")
add_executable (lr2 "lr2.cpp" "utils.hpp")
add_executable (lr3 "lr3.cpp" "utils.hpp")
add_executable (lr4 "lr4.cpp" "utils.hpp")
add_executable (lr5 "lr5.cpp" "utils.hpp")
add_executable (lr6 "lr6.cpp" "utils.hpp")
add_executable (lr8 "lr8.cpp" "utils.hpp")

