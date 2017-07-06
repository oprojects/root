// @(#)root/mpi:$Id: LinkDef.h  -- :: $

/*************************************************************************
 * Copyright (C) 2016, Omar Andres Zapata Mesa           .               *
 * Omar.Zapata@cern.ch   http://oproject.org             .               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ namespace Mpi;

/*
 * Some raw MPI datatypes
 */
#pragma link C++ class MPI_Status+;
#pragma link C++ class MPI_Request+;
#pragma link C++ class MPI_Info+;

#pragma link off all methods;

/*
 * ROOTMpi datatypes
 */
#pragma link C++ class ROOT::Mpi::TMpiMessage+;
#pragma link C++ class ROOT::Mpi::TMpiMessageInfo+;
#pragma link C++ class ROOT::Mpi::TMpiFile+;
#pragma link C++ class ROOT::Mpi::TMpiFileMerger+;
#pragma link C++ class ROOT::Mpi::TInfo+;
#pragma link C++ class ROOT::Mpi::TPort+;
#pragma link C++ class ROOT::Mpi::TMpiTimer;
#pragma link C++ class ROOT::Mpi::TStatus+;
#pragma link C++ class ROOT::Mpi::TRequest+;
#pragma link C++ class ROOT::Mpi::TPrequest+;
#pragma link C++ class ROOT::Mpi::TGrequest;
#pragma link C++ class ROOT::Mpi::TEnvironment;
#pragma link C++ class ROOT::Mpi::TNullCommunicator;
#pragma link C++ class ROOT::Mpi::TCommunicator;
#pragma link C++ class ROOT::Mpi::TCommunicator::Send<TMatrixD>;
#pragma link C++ class ROOT::Mpi::TCommunicator::Recv<TMatrixD>;
#pragma link C++ class ROOT::Mpi::TIntraCommunicator;
#pragma link C++ class ROOT::Mpi::TInterCommunicator;
#pragma link C++ class ROOT::Mpi::TGroup;
#pragma link C++ class ROOT::Mpi::TErrorHandler;

/*
 * Global communicator
 */
#pragma link C++ global ROOT::Mpi::COMM_WORLD;

#endif
