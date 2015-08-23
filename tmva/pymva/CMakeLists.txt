############################################################################
# CMakeLists.txt file for PyMVA  package
############################################################################
#Autor: Omar Andres Zapata Mesa 22/08/2015


set(libname PyMVA)

ROOT_USE_PACKAGE(core)
ROOT_USE_PACKAGE(core/rint)
ROOT_USE_PACKAGE(core/thread)
ROOT_USE_PACKAGE(math/matrix)
ROOT_USE_PACKAGE(io/io)
ROOT_USE_PACKAGE(graf2d/gpad)
ROOT_USE_PACKAGE(hist/hist)
ROOT_USE_PACKAGE(tmva)
ROOT_USE_PACKAGE(tree/tree)


include_directories(${PYTHON_INCLUDE_DIRS})


set(PY_HEADERS ${CMAKE_SOURCE_DIR}/tmva/pymva/inc/TMVA/PyMethodBase.h 
               ${CMAKE_SOURCE_DIR}/tmva/pymva/inc/TMVA/MethodPyRandomForest.h)
              

SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)


ROOT_GENERATE_DICTIONARY(G__PyMVA ${PY_HEADERS} MODULE ${libname} LINKDEF LinkDef.h)
# ROOT_GENERATE_ROOTMAP(RInterface LINKDEF LinkDef.h)


ROOT_LINKER_LIBRARY(PyMVA *.cxx G__PyMVA.cxx LIBRARIES ${PYTHON_LIBRARIES}  DEPENDENCIES Core Matrix Thread RIO TMVA)


ROOT_INSTALL_HEADERS()





