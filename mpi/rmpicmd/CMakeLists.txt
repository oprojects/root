
set(libname RMpiCmd)

# SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

include_directories(inc/)
set(RMPICMD_HEADERS Mpi/TCmdExecutor.h
		 Mpi/TCmdModuleBase.h
		 Mpi/TCmdModuleROOT.h
		 Mpi/TCmdModulePython.h
		 Mpi/TCmdModuleMpiExec.h
		 Mpi/TCmdModuleValgrind.h
		 )
set(RMPICMD_SRCS src/TCmdExecutor.cxx
                 src/TCmdModuleBase.cxx
                 src/TCmdModuleROOT.cxx
                 src/TCmdModulePython.cxx
                 src/TCmdModuleMpiExec.cxx
                 src/TCmdModuleValgrind.cxx
		 )

ROOT_GENERATE_DICTIONARY(G__RMpiCmd ${RMPICMD_HEADERS} MODULE ${libname} LINKDEF LinkDef.h)
ROOT_LINKER_LIBRARY(RMpiCmd ${RMPICMD_SRCS} G__RMpiCmd.cxx  DEPENDENCIES Core Thread RIO Net Hist Tree)

add_executable(rootmpi src/main.cxx )
target_link_libraries(rootmpi ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} Core Rint RMpiCmd)
install(TARGETS rootmpi DESTINATION bin)
ROOT_INSTALL_HEADERS()

