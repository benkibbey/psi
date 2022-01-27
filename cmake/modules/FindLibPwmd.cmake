#=============================================================================
# Copyright 2016-2017 Psi+ Project, Vitaly Tonkacheyev
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    set(D "d")
endif()
if( LIBPWMD_INCLUDE_DIR AND LIBPWMD_LIBRARY )
    # in cache already
    set(LIBPWMD_FIND_QUIETLY TRUE)
endif()

if( UNIX AND NOT( APPLE OR CYGWIN ) )
    find_package( PkgConfig QUIET )
    pkg_check_modules( PC_LIBPWMD QUIET libpwmd-8.0 )
    if( PC_LIBPWMD_FOUND )
        set( LIBPWMD_DEFINITIONS ${PC_LIBPWMD_CFLAGS} ${PC_LIBPWMD_CFLAGS_OTHER} )
    endif()
endif()

set( LIBPWMD_ROOT "" CACHE STRING "Path to libpwmd" )

find_path(
    LIBPWMD_INCLUDE_DIR libpwmd.h
    HINTS
    ${LIBPWMD_ROOT}/include
    ${PC_LIBPWMD_INCLUDEDIR}
    ${PC_LIBPWMD_INCLUDE_DIRS}
)
set(LIBPWMD_NAMES
    pwmd${D}
    pwmd
)
find_library(
    LIBPWMD_LIBRARY
    NAMES ${LIBPWMD_NAMES}
    HINTS
    ${PC_LIBPWMD_LIBDIR}
    ${PC_LIBPWMD_LIBRARY_DIRS}
    ${LIBPWMD_ROOT}/lib
    ${LIBPWMD_ROOT}/bin
)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
                LibPwmd
                DEFAULT_MSG
                LIBPWMD_LIBRARY
                LIBPWMD_INCLUDE_DIR
)
if( LIBPWMD_FOUND )
    set( LIBPWMD_LIBRARIES ${LIBPWMD_LIBRARY} )
    set( LIBPWMD_INCLUDE_DIRS ${LIBPWMD_INCLUDE_DIR} )
    add_definitions( -DHAVE_LIBPWMD )
    message( STATUS "Found libpwmd" )
else()
    message( STATUS "Could not find libpwmd" )
endif()

mark_as_advanced( LIBPWMD_INCLUDE_DIR LIBPWMD_LIBRARY )
