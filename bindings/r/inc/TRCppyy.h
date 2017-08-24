// @(#)root/r:$Id$
// Author: Omar Zapata   20/7/2017

/*************************************************************************
 * Copyright (C) 2013-2017, Omar Andres Zapata Mesa                      *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_R_TRCppyy
#define ROOT_R_TRCppyy

#include <RExports.h>
#include <Rcpp/XPtr.h>

#include <TClass.h>
#include <TMethod.h>
#include <TMethodArg.h>
#include <TList.h>
#include <TListOfFunctions.h>

namespace ROOT {
namespace R {
class TRCppyy : public TObject {
   TClass *fCl;
   TString fClearName;
   std::vector<TString> fNamespaces;
   static std::vector<TString> GetNameSpaces(const Char_t *name, TString &cname);

public:
   TRCppyy(TClass *cl);
   TRCppyy(const Char_t *name);

   TListOfFunctions *GetListOfMethods();

   Bool_t IsNamespace();

   TMethod *GetMethod(Int_t index);
   Bool_t IsPublicMethod(Int_t index);

   Bool_t IsConstMethod(Int_t index);

   TMethodArg *GetMethodArg(TMethod *m, Int_t index);
   TMethodArg *GetMethodArg(Int_t mindex, Int_t aindex);
   TString GetArgPrototype(Int_t mindex, Int_t aindex);

   // Args related methods
   TList *GetListOfMethodArgs(Int_t index);

   const Char_t *GetName() { return fCl->GetName(); }
   const Char_t *GetClearName() { return fClearName.Data(); }

   std::vector<TString> GetNameSpaces() { return fNamespaces; }

   // static
   static Bool_t IsPublicMethod(TMethod *m);
   static TList *GetListOfMethodArgs(TMethod *m);
   static Bool_t IsArgEnum(TMethodArg *arg);
   static TString GetArgPrototype(TMethodArg *arg);
   static Bool_t IsNamespace(TClass *cl);
   static Bool_t IsNamespace(const Char_t *name);
   static Bool_t IsConstMethod(TMethod *m);
   static std::vector<TString> GetNameSpaces(const Char_t *name);
   static std::vector<TString> GetNameSpaces(TClass *cl);

   ClassDef(TRCppyy, 0) //
};
}
}

#endif
