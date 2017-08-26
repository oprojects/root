// @(#)root/r:$Id$
// Author: Omar Zapata   20/7/2017

/*************************************************************************
 * Copyright (C) 2013-2017, Omar Andres Zapata Mesa                      *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_R_TRCodeGen
#define ROOT_R_TRCodeGen

#include <RExports.h>
#include <Rcpp/XPtr.h>
#include <TRCppyy.h>

namespace ROOT {
namespace R {
class TRCodeGen : public TObject {
   TRCppyy fCppyy;
   TString fClassCode;
   TString fWrapCode;

public:
   TRCodeGen(TClass *cl);
   TRCodeGen(const TString name);

   TString GenClass();
   TString GenClassWrap();

   ClassDef(TRCodeGen, 0) //
};
}
}

#endif
