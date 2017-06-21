############################################################################
# CMakeLists.txt file for building ROOT ptmva package
# @author Omar Zapata, http://oproject.org
############################################################################

set(libname PTMVA)


set(headers  ${CMAKE_SOURCE_DIR}/tmva/ptmva/inc/TMVA/ParallelExecutorBase.h 
             ${CMAKE_SOURCE_DIR}/tmva/ptmva/inc/TMVA/ParallelExecutor.h )
set(linkdef  ${CMAKE_SOURCE_DIR}/tmva/ptmva/inc/LinkDef.h )

set(linkdefopt "" )


ROOT_GENERATE_DICTIONARY(G__${libname} ${headers} MODULE ${libname} LINKDEF ${linkdef} OPTIONS ${linkdefopt})


ROOT_LINKER_LIBRARY(${libname} *.cxx G__${libname}.cxx LIBRARIES Core ${TBB_LIBRARIES}
                    DEPENDENCIES TMVA MultiProc RIO Hist Tree TreePlayer MLP Minuit XMLIO)# ${MPI_LIBRARIES})

ROOT_INSTALL_HEADERS()

if(testing)
    ROOT_ADD_TEST_SUBDIRECTORY(test)
endif()





