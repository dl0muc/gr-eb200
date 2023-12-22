# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT eb200_sources)
    message(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT eb200_sources)

########################################################################
# Check for pygccxml
########################################################################
gr_python_check_module_raw("pygccxml" "import pygccxml" PYGCCXML_FOUND)

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND eb200_python_files
    if_stream_decode_python.cc python_bindings.cc)

gr_pybind_make_oot(eb200 ../../.. gr::eb200 "${eb200_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(
    TARGET eb200_python
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:eb200_python>
            ${PROJECT_BINARY_DIR}/test_modules/gnuradio/eb200/)

install(
    TARGETS eb200_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/eb200
    COMPONENT pythonapi)
