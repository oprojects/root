
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories(inc/)

SET(ROOTMPI_SOURCES src/main.cxx src/TRootMpi.cxx)
add_executable(rootmpi ${ROOTMPI_SOURCES})
target_link_libraries(rootmpi ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} Core Rint)
install(TARGETS rootmpi DESTINATION bin)

