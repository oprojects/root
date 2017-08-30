# Try to find Scalable Checkpoint / Restart (SCR) Library
# See
# https://computation.llnl.gov/projects/scalable-checkpoint-restart-for-mpi  and
# https://github.com/LLNL/scr
#
#
# It defines the following variables:
#  SCR_FOUND         - system has SCR libs
#  SCR_INCLUDE_DIR   - where to find headers
#  SCR_LIBRARY       - the libscr library


# look for headers
find_path(SCR_INCLUDE_DIR
    NAMES scr.h
    PATHS "${CMAKE_SCR_PATH}/include" $ENV{SCR_PATH}/include /usr/include /usr/local/include /opt/scr/include
)

# look for scr library
find_library(SCR_LIBRARY
    NAMES scr
    PATHS "${CMAKE_SCR_PATH}/lib" $ENV{SCR_PATH}/lib /usr/lib /usr/local/lib /opt/scr/lib
)

if(SCR_LIBRARY)
    MESSAGE(STATUS "Found SCR library: ${SCR_LIBRARY}")
endif()

if(SCR_INCLUDE_DIR)
    MESSAGE(STATUS "Found SCR include: ${SCR_INCLUDE_DIR}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SCR REQUIRED_VARS SCR_INCLUDE_DIR SCR_LIBRARY)
mark_as_advanced(SCR_INCLUDE_DIR SCR_LIBRARY)

