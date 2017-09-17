SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

#################
#MPI ENVIRONMENT#
#################
FIND_PACKAGE(MPI REQUIRED)
FIND_PACKAGE(SCR)
include(GenerateExportHeader)

set(EXPORT_HEADER_OPT "")
find_program(VALGRIND "valgrind")
if(VALGRIND)
    message(STATUS "Found ${VALGRIND}")
    set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_VALGRINDFOUND\n")
    set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_VALGRIND \"${VALGRIND}\"\n")
endif()

if(python OR python2 OR python3)
    if(PYTHONINTERP_FOUND)
        set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define PYTHONINTERP_FOUND\n")
        set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define PYTHON_EXECUTABLE \"${PYTHON_EXECUTABLE}\"\n")
    endif(PYTHONINTERP_FOUND)
endif()

set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_EXEC \"${MPIEXEC}\"\n")
set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_CXX \"${CXX}\"\n")
set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_ROOTSYS \"$ENV{ROOTSYS}\"\n")

execute_process(COMMAND ${MPI_C_COMPILER} -showme:compile
                 OUTPUT_VARIABLE  MPI_COMPILE_CMDLINE OUTPUT_STRIP_TRAILING_WHITESPACE
                 ERROR_VARIABLE   MPI_COMPILE_CMDLINE ERROR_STRIP_TRAILING_WHITESPACE
                 RESULT_VARIABLE  MPI_COMPILER_RETURN)

if(MPI_COMPILER_RETURN EQUAL 0)
    set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_CFLAGS \"${MPI_CXX_COMPILE_FLAGS} ${MPI_COMPILE_CMDLINE}\"\n")
else()
    set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_CFLAGS \"${MPI_CXX_COMPILE_FLAGS} ${MPI_C_COMPILE_FLAGS}\"\n")
endif()
execute_process(COMMAND ${MPI_C_COMPILER} -showme:link
                 OUTPUT_VARIABLE  MPI_LINK_CMDLINE OUTPUT_STRIP_TRAILING_WHITESPACE
                 ERROR_VARIABLE   MPI_LINK_CMDLINE ERROR_STRIP_TRAILING_WHITESPACE
                 RESULT_VARIABLE  MPI_COMPILER_RETURN)
if(MPI_COMPILER_RETURN EQUAL 0)
    set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_LDFLAGS \"${MPI_CXX_LINK_FLAGS} ${MPI_LINK_CMDLINE}\"\n")
else()
    set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_LDFLAGS \"${MPI_CXX_LINK_FLAGS} ${MPI_C_LINK_FLAGS}\"\n")
endif()


#Standalone Build
if(NOT CMAKE_PROJECT_NAME STREQUAL ROOT)
  cmake_minimum_required(VERSION 3.4.3 FATAL_ERROR)
  project(ROOTMpi)
  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../cmake/modules)
  include(RootBuildOptions)
  include(RootNewMacros)
  include(CheckCompiler)
  include(MacroEnsureVersion)
  include(StandaloneBuild)
  #---Check for Python installation-------------------------------------------------------
  if(python OR python2 OR python3)
  message(STATUS "Looking for Python")
  #---First look for the python interpreter and fix the version of it for the libraries--
  if(python3)
    find_package(PythonInterp 3.5)
  else()
    find_package(PythonInterp)
  endif()
  if(PYTHONINTERP_FOUND)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import sys;sys.stdout.write(sys.version[:3])"
                    OUTPUT_VARIABLE PYTHON_VERSION)
    message(STATUS "Found Python interpreter version ${PYTHON_VERSION}")
  endif()
  set(Python_ADDITIONAL_VERSIONS ${PYTHON_VERSION})
  endif()

  if(testing)  
    configure_file(${CMAKE_SOURCE_DIR}/../cmake/modules/CTestCustom.cmake ${CMAKE_BINARY_DIR} COPYONLY)
    enable_testing()
    include(RootCTest)
    add_subdirectory(test)
  endif()
  
  string(REPLACE "/bin/mpiexec" "" MPI_PATH "${MPIEXEC}")
  file( WRITE  "${CMAKE_BINARY_DIR}/env.sh" "export PATH=${MPI_PATH}/bin:$PATH\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh" "export PATH=$PATH:${CMAKE_BINARY_DIR}/rootmpi\n")
  
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "if [ -n \"$ROOT_LIBRARY_PATH\" ] ; then\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh" "\texport ROOT_LIBRARY_PATH=$ROOT_LIBRARY_PATH:${CMAKE_BINARY_DIR}/rmpi\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "else\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh" "\texport ROOT_LIBRARY_PATH=${CMAKE_BINARY_DIR}/rmpi\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "fi\n")

  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "if [ -n \"$LD_LIBRARY_PATH\" ] ; then\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh" "\texport LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${CMAKE_BINARY_DIR}/rmpi:${MPI_PATH}/lib\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "else\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh" "\texport LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/rmpi:${MPI_PATH}/lib\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "fi\n")
  
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "if [ -n \"$ROOT_INCLUDE_PATH\" ] ; then\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh" "\texport ROOT_INCLUDE_PATH=$ROOT_INCLUDE_PATH:${CMAKE_SOURCE_DIR}/rmpi/inc:${MPI_PATH}/include\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "else\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh" "\texport ROOT_INCLUDE_PATH=${CMAKE_SOURCE_DIR}/rmpi/inc:${MPI_PATH}/include\n")
  file( APPEND "${CMAKE_BINARY_DIR}/env.sh"  "fi\n")

  get_property(__allHeaders GLOBAL PROPERTY ROOT_HEADER_TARGETS)
  add_custom_target(move_headers ALL DEPENDS ${__allHeaders})  
endif()

if(SCR_FOUND)
message(STATUS "Enabling ROOT Mpi Scalable Checkpoint / Restart ")
include_directories(${SCR_INCLUDE_DIR})
set(EXPORT_HEADER_OPT "${EXPORT_HEADER_OPT}#define ROOT_MPI_SCR\n")
endif()

include_directories(${MPI_CXX_INCLUDE_PATH} ${MPI_C_INCLUDE_PATH} "${CMAKE_BINARY_DIR}/include")
ROOT_ADD_CXX_FLAG(CMAKE_CXX_FLAGS  -Wno-overloaded-virtual  -Wno-literal-suffix)

add_subdirectory(rmpi)
add_subdirectory(rmpicmd)

GENERATE_EXPORT_HEADER(RMpi EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/Mpi/TMpiExport.h  CUSTOM_CONTENT_FROM_VARIABLE EXPORT_HEADER_OPT )
install(FILES "${CMAKE_BINARY_DIR}/include/Mpi/TMpiExport.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/Mpi" COMPONENT headers)


if(testing)  
ROOT_ADD_TEST_SUBDIRECTORY(test)
endif()
