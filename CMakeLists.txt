cmake_minimum_required(VERSION 4.4.3)
project(SurvCalc)

# to run (Windows, MinGW gcc):
#   cmake -G "MinGW Makefiles" -S . -B build
#   cmake --build build
#
# to run (Linux, gcc):
#   cmake -G "Unix Makefiles" -S . -B build
#   cmake --build build

# Let CMake compile the Windows resource (.rc) file (uses windres with MinGW)
if(WIN32)
  enable_language(RC)
endif()

set(MAIN_NAME SurvCalc)
set(RANDOM newran)

# Kept as a list so the flags can be limited to C++ files
set(COMPILE_OPTIONS_MAIN -O0 -g3 -Wall -fmessage-length=0)

# Autogeneration of version.h in the src directory (currently a placeholder)
add_custom_target(version ALL
  #COMMAND cp ${PROJECT_SOURCE_DIR}/src/version.h ${PROJECT_SOURCE_DIR}/src/version.h.old
  #COMMAND svn_version --quiet --format c++ --path ${PROJECT_SOURCE_DIR} --output ${PROJECT_SOURCE_DIR}/src/version.h --recursive
)

# Random number library
add_library(${RANDOM}
  src/newran_02C/extreal.cpp
  src/newran_02C/newran.cpp
  src/newran_02C/myexcept.cpp
)

# Main executable
add_executable(${MAIN_NAME}
  src/datamanager.cpp
  src/development.cpp
  src/dictionary.cpp
  src/model.cpp
  src/parameter_set.cpp
  src/sample.cpp
  src/species.cpp
  src/station.cpp
  src/stratum.cpp
  src/survey.cpp
  src/trip.cpp
  src/t_catch.cpp
  src/t_lgth.cpp
  src/t_subcatch.cpp
  src/t_LW_coeffs.cpp
  src/t_station.cpp
  src/t_station_catch.cpp
  src/t_stratum.cpp
  src/t_stratum_catch.cpp
  src/betadiff/betadiff.cpp
)

# Compile flags apply to C++ files only, so they are not passed to windres
target_compile_options(${MAIN_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${COMPILE_OPTIONS_MAIN}>)

target_link_libraries(${MAIN_NAME} ${RANDOM})

# Windows only: embed the icon resource and link the runtime statically
if(WIN32)
  target_sources(${MAIN_NAME} PRIVATE src/SurvCalc.rc)
  target_link_options(${MAIN_NAME} PRIVATE -static)
endif()

# version.h must be generated before the executable is compiled
add_dependencies(${MAIN_NAME} version)
