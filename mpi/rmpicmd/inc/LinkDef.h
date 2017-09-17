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

/*
 * ROOTMpi datatypes
 */
#pragma link C++ class ROOT::Mpi::TCmdExecutor + ;
#pragma link C++ class ROOT::Mpi::TCmdModuleMpiExec;
#pragma link C++ class ROOT::Mpi::TCmdModuleROOT;
#pragma link C++ class ROOT::Mpi::TCmdModuleValgrind;
#pragma link C++ class ROOT::Mpi::TCmdModulePython;

#endif
