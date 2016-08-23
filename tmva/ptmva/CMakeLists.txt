############################################################################
# CMakeLists.txt file for building ROOT ptmva package
# @author Omar Zapata, http://oproject.org
############################################################################

set(libname PTMVA)


set(headers  ${CMAKE_SOURCE_DIR}/tmva/ptmva/inc/TMVA/ParallelExecutorBase.h 
             ${CMAKE_SOURCE_DIR}/tmva/ptmva/inc/TMVA/ParallelExecutor.h )
set(linkdef  ${CMAKE_SOURCE_DIR}/tmva/ptmva/inc/LinkDef.h )

set(linkdefopt "" )


find_package(MPI)

if(MPI_CXX_FOUND)
    include_directories(${MPI_INCLUDE_PATH})
    LIST(APPEND headers ${CMAKE_SOURCE_DIR}/tmva/ptmva/inc/TMVA/ParallelExecutorMpi.h)
    SET(linkdefopt "-DMPI")
endif()


ROOT_GENERATE_DICTIONARY(G__${libname} ${headers} MODULE ${libname} LINKDEF ${linkdef} OPTIONS ${linkdefopt})


ROOT_LINKER_LIBRARY(${libname} *.cxx G__${libname}.cxx LIBRARIES Core ${TBB_LIBRARIES}
                    DEPENDENCIES TMVA MultiProc RIO Hist Tree TreePlayer MLP Minuit XMLIO)# ${MPI_LIBRARIES})

if(MPI_CXX_FOUND)
    SET_TARGET_PROPERTIES(${libname} PROPERTIES
    LINK_FLAGS "${MPI_LINK_FLAGS}")

    SET_TARGET_PROPERTIES(${libname} PROPERTIES
    COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()


ROOT_INSTALL_HEADERS()


#ROOT_ADD_TEST_SUBDIRECTORY(test)




