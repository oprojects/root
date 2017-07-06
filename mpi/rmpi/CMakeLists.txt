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
		 Mpi.h
		 )
if(python)
    if(PYTHONLIBS_FOUND)
        include_directories(${PYTHON_INCLUDE_DIRS})
        add_definitions(-w "-DPYTHON_FOUND=1" )
    else()
        add_definitions(-w "-DPYTHON_FOUND=0" )
    endif()
endif()
ROOT_GENERATE_DICTIONARY(G__RMpi ${RMPI_HEADERS} MODULE ${libname} LINKDEF LinkDef.h)

if(python)
    if(PYTHONLIBS_FOUND)
        ROOT_LINKER_LIBRARY(RMpi *.cxx G__RMpi.cxx LIBRARIES ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} ${PYTHON_LIBRARIES}  DEPENDENCIES Core Thread RIO Net Hist Tree)
    else()
        ROOT_LINKER_LIBRARY(RMpi *.cxx G__RMpi.cxx LIBRARIES ${MPI_C_LIBRARIES} ${MPI_CXX_LIBRARIES} DEPENDENCIES Core Thread RIO Net Hist Tree)
    endif()
endif()
ROOT_INSTALL_HEADERS()






