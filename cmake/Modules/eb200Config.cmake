INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_EB200 eb200)

FIND_PATH(
    EB200_INCLUDE_DIRS
    NAMES eb200/api.h
    HINTS $ENV{EB200_DIR}/include
        ${PC_EB200_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    EB200_LIBRARIES
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

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EB200 DEFAULT_MSG EB200_LIBRARIES EB200_INCLUDE_DIRS)
MARK_AS_ADVANCED(EB200_LIBRARIES EB200_INCLUDE_DIRS)

