cmake_minimum_required(VERSION 2.4)
project(SurvCalc)

# to run (Windows, Mingw32-gcc)
# cmake -G "MinGW Makefiles"
# mingw32-make all

# to run (Linux, gcc)
# cmake -G "Unix Makefiles"
# make all

# Assumes svn_version is available

SET(MAIN_NAME SurvCalc)
SET(RANDOM newran)

IF (WIN32)
  SET(NEWRAN src/newran_02C)
  SET(LINK_OPTIONS "-static SurvCalc.o")
  SET(COMPILE_OPTIONS_MAIN "-O0 -g3 -Wall -fmessage-length=0")
  ADD_CUSTOM_TARGET(icon ALL
    COMMAND windres --output ${PROJECT_SOURCE_DIR}/SurvCalc.o --input ${PROJECT_SOURCE_DIR}/src/SurvCalc.rc
  )
ELSE (WIN32)
  SET(NEWRAN src/newran_02C)
  SET(COMPILE_OPTIONS_MAIN "-O0 -g3 -Wall -fmessage-length=0")
ENDIF (WIN32)

# Autogeneration of Version.h file in src directory (backing up current ('NULL') version
ADD_CUSTOM_TARGET(version ALL
  COMMAND cp ${PROJECT_SOURCE_DIR}/src/version.h ${PROJECT_SOURCE_DIR}/src/version.h.old
  COMMAND svn_version --quiet --format c++ --path ${PROJECT_SOURCE_DIR} --output ${PROJECT_SOURCE_DIR}/src/version.h --recursive
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

SET(MAIN_LIBRARIES
  ${RANDOM}
)

ADD_LIBRARY(${RANDOM}
src/newran_02C/extreal.cpp
src/newran_02C/newran.cpp
src/newran_02C/myexcept.cpp
)

# Build
IF (WIN32) 
  SET_TARGET_PROPERTIES(${MAIN_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS_MAIN} LINK_FLAGS ${LINK_OPTIONS})
  TARGET_LINK_LIBRARIES(${MAIN_NAME} ${LIBRARY_NAME} ${MAIN_LIBRARIES})
ELSE (WIN32)
  SET_TARGET_PROPERTIES(${MAIN_NAME} PROPERTIES COMPILE_FLAGS ${COMPILE_OPTIONS_MAIN})
  TARGET_LINK_LIBRARIES(${MAIN_NAME} ${LIBRARY_NAME} ${MAIN_LIBRARIES})
ENDIF (WIN32)

ADD_DEPENDENCIES(${MAIN_NAME} version icon ${RANDOM})
