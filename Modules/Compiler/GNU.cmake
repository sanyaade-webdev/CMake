
#=============================================================================
# Copyright 2002-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# This module is shared by multiple languages; use include blocker.
if(__COMPILER_GNU)
  return()
endif()
set(__COMPILER_GNU 1)

macro(__compiler_gnu lang)
  # Feature flags.
  set(CMAKE_${lang}_VERBOSE_FLAG "-v")
  set(CMAKE_${lang}_COMPILE_OPTIONS_PIC "-fPIC")
  set(CMAKE_${lang}_COMPILE_OPTIONS_PIE "-fPIE")
  set(CMAKE_SHARED_LIBRARY_${lang}_FLAGS "-fPIC")
  set(CMAKE_SHARED_LIBRARY_CREATE_${lang}_FLAGS "-shared")

  # Older versions of gcc (< 4.5) contain a bug causing them to report a missing
  # header file as a warning if depfiles are enabled, causing check_header_file
  # tests to always succeed.  Work around this by disabling dependency tracking
  # in try_compile mode.
  GET_PROPERTY(_IN_TC GLOBAL PROPERTY IN_TRY_COMPILE)
  if(NOT _IN_TC OR CMAKE_FORCE_DEPFILES)
    # distcc does not transform -o to -MT when invoking the preprocessor
    # internally, as it ought to.  Work around this bug by setting -MT here
    # even though it isn't strictly necessary.
    set(CMAKE_DEPFILE_FLAGS_${lang} "-MMD -MT <OBJECT> -MF <DEPFILE>")
  endif()

  # Initial configuration flags.
  set(CMAKE_${lang}_FLAGS_INIT "")
  set(CMAKE_${lang}_FLAGS_DEBUG_INIT "-g")
  set(CMAKE_${lang}_FLAGS_MINSIZEREL_INIT "-Os -DNDEBUG")
  set(CMAKE_${lang}_FLAGS_RELEASE_INIT "-O3 -DNDEBUG")
  set(CMAKE_${lang}_FLAGS_RELWITHDEBINFO_INIT "-O2 -g")
  set(CMAKE_${lang}_CREATE_PREPROCESSED_SOURCE "<CMAKE_${lang}_COMPILER> <DEFINES> <FLAGS> -E <SOURCE> > <PREPROCESSED_SOURCE>")
  set(CMAKE_${lang}_CREATE_ASSEMBLY_SOURCE "<CMAKE_${lang}_COMPILER> <DEFINES> <FLAGS> -S <SOURCE> -o <ASSEMBLY_SOURCE>")
  if(NOT APPLE)
    set(CMAKE_INCLUDE_SYSTEM_FLAG_${lang} "-isystem ")
  endif(NOT APPLE)
endmacro()
