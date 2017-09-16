############################################################################
# CMakeLists.txt file for building ROOT MPI package
############################################################################
#Autor: Omar Andres Zapata Mesa 2016
# Omar.Zapata@cern.ch
# http://oproject.org

set(libname RMpi)
include_directories(inc/)

set(RMPI_HEADERS Mpi/TEnvironment.h
		 Mpi/TMpiMessage.h
		 Mpi/TCommunicator.h
		 Mpi/TIntraCommunicator.h
		 Mpi/TInterCommunicator.h
		 Mpi/TCartCommunicator.h
		 Mpi/TGraphCommunicator.h
		 Mpi/TStatus.h
		 Mpi/TRequest.h
         Mpi/Globals.h
         Mpi/TOp.h
         Mpi/TGroup.h
         Mpi/TInfo.h
         Mpi/TPort.h
         Mpi/TMpiTimer.h
         Mpi/TMpiFile.h
         Mpi/TErrorHandler.h
		 TMpi.h
		 )
		 
set(RMPI_SRCS src/Globals.cxx 
              src/TCartCommunicator.cxx 
              src/TCommunicator.cxx 
              src/TEnvironment.cxx 
              src/TErrorHandler.cxx 
              src/TGraphCommunicator.cxx 
              src/TGroup.cxx 
              src/TInfo.cxx 
              src/TInterCommunicator.cxx 
              src/TIntraCommunicator.cxx 
              src/TMpiFile.cxx 
              src/TMpiMessage.cxx 
              src/TMpiTimer.cxx 
              src/TPort.cxx 
              src/TRequest.cxx 
              src/TStatus.cxx
              )
		 
if(SCR_FOUND)
    set(RMPI_HEADERS ${RMPI_HEADERS} Mpi/TCheckPoint.h)
    set(RMPI_SRCS ${RMPI_SRCS} src/TCheckPoint.cxx)    
endif()

ROOT_GENERATE_DICTIONARY(G__RMpi ${RMPI_HEADERS} MODULE ${libname} LINKDEF LinkDef.h)

if(SCR_FOUND)
    ROOT_LINKER_LIBRARY(RMpi ${RMPI_SRCS} G__RMpi.cxx LIBRARIES ${SCR_LIBRARY} ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} DEPENDENCIES Core Thread RIO Net Hist Tree)
else()
    ROOT_LINKER_LIBRARY(RMpi ${RMPI_SRCS} G__RMpi.cxx LIBRARIES ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} DEPENDENCIES Core Thread RIO Net Hist Tree)
endif()

ROOT_INSTALL_HEADERS()






