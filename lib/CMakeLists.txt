# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND eb200_sources
    if_stream_decode_impl.cc
)

set(eb200_sources "${eb200_sources}" PARENT_SCOPE)
if(NOT eb200_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT eb200_sources)

add_library(gnuradio-eb200 SHARED ${eb200_sources})
target_link_libraries(gnuradio-eb200 ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-eb200 PROPERTIES DEFINE_SYMBOL "gnuradio_eb200_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-eb200 PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-eb200
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_eb200_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_eb200.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_eb200.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_if_stream_decode.cc
)

add_executable(test-eb200 ${test_eb200_sources})

target_link_libraries(
  test-eb200
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-eb200
)

GR_ADD_TEST(test_eb200 test-eb200)
