find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_EB200 gnuradio-eb200)

FIND_PATH(
    GR_EB200_INCLUDE_DIRS
    NAMES gnuradio/eb200/api.h
    HINTS $ENV{EB200_DIR}/include
        ${PC_EB200_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_EB200_LIBRARIES
    NAMES gnuradio-eb200
    HINTS $ENV{EB200_DIR}/lib
        ${PC_EB200_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-eb200Target.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_EB200 DEFAULT_MSG GR_EB200_LIBRARIES GR_EB200_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_EB200_LIBRARIES GR_EB200_INCLUDE_DIRS)
