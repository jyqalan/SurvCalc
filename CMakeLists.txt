cmake_minimum_required(VERSION 4.4.3)
project(SurvCalc)
 
# Let CMake compile the Windows resource (.rc) file itself, so the resulting
# object file goes into the build directory (uses windres with MinGW)
IF (WIN32)
  enable_language(RC)
ENDIF (WIN32)
 
# to run (Windows, Mingw32-gcc)
# cmake -G "MinGW Makefiles" -S . -B build
# cmake --build build
 
# to run (Linux, gcc)
# cmake -G "Unix Makefiles" -S . -B build
# cmake --build build
 
# Assumes svn_version is available
 
SET(MAIN_NAME SurvCalc)
SET(RANDOM newran)
 
# Kept as a list so the flags can be limited to C++ files (see target_compile_options below)
SET(COMPILE_OPTIONS_MAIN -O0 -g3 -Wall -fmessage-length=0)
 
IF (WIN32)
  SET(NEWRAN src/newran_02C)
  SET(LINK_OPTIONS "-static")
ELSE (WIN32)
  SET(NEWRAN src/newran_02C)
ENDIF (WIN32)
 
# Autogeneration of Version.h file in src directory (backing up current ('NULL') version
ADD_CUSTOM_TARGET(version ALL
  #COMMAND cp ${PROJECT_SOURCE_DIR}/src/version.h ${PROJECT_SOURCE_DIR}/src/version.h.old
  #COMMAND svn_version --quiet --format c++ --path ${PROJECT_SOURCE_DIR} --output ${PROJECT_SOURCE_DIR}/src/version.h --recursive
)
 
SET(SOURCE_FILES
src/newran_02C/extreal.cpp
src/newran_02C/newran.cpp
src/newran_02C/myexcept.cpp
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
src/t_LW_coeffs.cpp
src/t_station.cpp
src/t_subcatch.cpp
src/t_station_catch.cpp
src/t_stratum.cpp
src/t_stratum_catch.cpp
src/betadiff/betadiff.cpp
)
 
ADD_EXECUTABLE(${MAIN_NAME}
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
 
# Windows icon/resource file, compiled by CMake into the build directory
IF (WIN32)
  target_sources(${MAIN_NAME} PRIVATE src/SurvCalc.rc)
ENDIF (WIN32)
 
SET(MAIN_LIBRARIES
  ${RANDOM}
)
 
ADD_LIBRARY(${RANDOM}
src/newran_02C/extreal.cpp
src/newran_02C/newran.cpp
src/newran_02C/myexcept.cpp
)
 
# Build
# Compile flags are applied to C++ files only, so they are not passed to windres for the .rc file
target_compile_options(${MAIN_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${COMPILE_OPTIONS_MAIN}>)
 
IF (WIN32) 
  SET_TARGET_PROPERTIES(${MAIN_NAME} PROPERTIES LINK_FLAGS ${LINK_OPTIONS})
  TARGET_LINK_LIBRARIES(${MAIN_NAME} ${LIBRARY_NAME} ${MAIN_LIBRARIES})
ELSE (WIN32)
  TARGET_LINK_LIBRARIES(${MAIN_NAME} ${LIBRARY_NAME} ${MAIN_LIBRARIES})
ENDIF (WIN32)
 
ADD_DEPENDENCIES(${MAIN_NAME} version ${RANDOM})