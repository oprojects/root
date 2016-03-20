/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id$
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2005, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/

/**
\file RooAbsCache.cxx
\class RooAbsCache
\ingroup Roofitcore

RooAbsCache is the abstract base class for data members of RooAbsArgs
that cache other (composite) RooAbsArg expressions. The RooAbsCache
interface defines the interaction between the owning RooAbsArg object
and the cache data member to communicate server redirects, operation
mode changes and constant term optimization management calls.
**/


#include "RooFit.h"
#include "RooAbsCache.h"
#include "RooAbsArg.h"
#include "RooArgList.h"

using namespace std;

ClassImp(RooAbsCache) 
   ;


////////////////////////////////////////////////////////////////////////////////
/// Constructor. Takes owner as argument and register cache with owner

RooAbsCache::RooAbsCache(RooAbsArg* owner) : _owner(owner) 
{ 
  if (_owner) {
    _owner->registerCache(*this) ; 
  }
} 



////////////////////////////////////////////////////////////////////////////////
/// Copy constructor. Takes owner as argument and registers cache with owne

RooAbsCache::RooAbsCache(const RooAbsCache&, RooAbsArg* owner ) : _owner(owner) 
{ 
  if (_owner) {
    owner->registerCache(*this) ; 
  }
}



////////////////////////////////////////////////////////////////////////////////
/// Destructor. Unregisters cache with owner

RooAbsCache::~RooAbsCache() 
{ 
  if (_owner) {
    _owner->unRegisterCache(*this) ; 
  }
}



////////////////////////////////////////////////////////////////////////////////
/// Interface for processing of cache mode optimization calls

void RooAbsCache::optimizeCacheMode(const RooArgSet& /*obs*/, RooArgSet&, RooLinkedList& ) 
{
}



////////////////////////////////////////////////////////////////////////////////
/// Interface for server redirect calls

Bool_t RooAbsCache::redirectServersHook(const RooAbsCollection& /*newServerList*/, Bool_t /*mustReplaceAll*/, Bool_t /*nameChange*/, Bool_t /*isRecursive*/) 
{ 
  return kFALSE ; 
} 



////////////////////////////////////////////////////////////////////////////////
/// Interface for operation mode changes

void RooAbsCache::operModeHook() 
{
} 



////////////////////////////////////////////////////////////////////////////////
/// Interface for constant term node finding calls

void RooAbsCache::findConstantNodes(const RooArgSet&, RooArgSet&, RooLinkedList& ) 
{  
}



////////////////////////////////////////////////////////////////////////////////
/// Interface for printing of cache guts in tree mode printing

void RooAbsCache::printCompactTreeHook(std::ostream&, const char *)
{
}

