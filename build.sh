#!/bin/bash
clear
# Revert version.h
svn revert src/version.h

# Make
cmake -G "Unix Makefiles"
make all

# Revert version.h
rm -f src/version.h.old
svn revert src/version.h

# Strip and place in local directories
strip SurvCalc

