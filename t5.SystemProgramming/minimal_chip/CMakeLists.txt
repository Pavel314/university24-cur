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

project ("minimal_chip" VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")

include(FetchContent)

FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
	GIT_SHALLOW     TRUE
)
FetchContent_MakeAvailable(sdl3)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RES_DIR "${SRC_DIR}/res")
set(ROMDB_DIR "${SRC_DIR}/romdb")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCES
    ${SRC_DIR}/minimal_chip.c
    ${RES_DIR}/chip_resources.h.c
    ${ROMDB_DIR}/romdb.c
)

add_executable (minimal_chip MACOSX_BUNDLE ${SOURCES})

set_target_properties(minimal_chip PROPERTIES WIN32_EXECUTABLE $<CONFIG:Release>)
target_link_options(minimal_chip PRIVATE "$<$<C_COMPILER_ID:MSVC>:/ENTRY:mainCRTStartup>")
target_include_directories(minimal_chip PRIVATE ${INCLUDE_DIR})
target_compile_options(minimal_chip
    PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wall -Wextra -pedantic>
        $<$<C_COMPILER_ID:MSVC>:/W4>
)
target_link_libraries(minimal_chip PRIVATE SDL3::SDL3)
#/fsanitize=address
#only for desktop os(not android)
add_custom_command(TARGET minimal_chip POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy -t $<TARGET_FILE_DIR:minimal_chip> $<TARGET_RUNTIME_DLLS:minimal_chip>
    COMMAND_EXPAND_LISTS
)

add_custom_command(TARGET minimal_chip POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_SOURCE_DIR}/roms"
          "$<TARGET_FILE_DIR:minimal_chip>/roms"
)