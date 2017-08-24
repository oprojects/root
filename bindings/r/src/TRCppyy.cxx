/*************************************************************************
 * Copyright (C) 2013-2017, Omar Andres Zapata Mesa                      *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#include <TRCppyy.h>
#include <TROOT.h>
#include <TInterpreter.h>

using namespace ROOT::R;

//______________________________________________________________________________
TRCppyy::TRCppyy(TClass *cl) : fCl(cl)
{
   TString name = cl->GetName();
   fNamespaces = GetNameSpaces(name, fClearName);
   if (fClearName.IsNull())
      fClearName = name;
}

//______________________________________________________________________________
TRCppyy::TRCppyy(const TString name)
{
   fCl = gROOT->GetClass(name.Data(), 1, 0);
   fNamespaces = GetNameSpaces(name, fClearName);
   if (fClearName.IsNull())
      fClearName = name;
}

//______________________________________________________________________________
TListOfFunctions *TRCppyy::GetListOfMethods()
{
   return (TListOfFunctions *)fCl->GetListOfMethods(1);
}

//______________________________________________________________________________
TListOfFunctions *TRCppyy::GetListOfPublicMethods()
{
   return (TListOfFunctions *)fCl->GetListOfAllPublicMethods(1);
}

//______________________________________________________________________________
TMethod *TRCppyy::GetMethod(Int_t index)
{
   return (TMethod *)fCl->GetListOfMethods(1)->At(index);
}

//______________________________________________________________________________
Bool_t TRCppyy::IsPublicMethod(Int_t index)
{
   auto m = GetMethod(index);
   if (m) {
      return m->Property() & kIsPublic;
   }
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsConstMethod(Int_t index)
{
   auto m = GetMethod(index);
   if (m) {
      return m->Property() & kIsConstMethod;
   }
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsPublicMethod(TMethod *m)
{
   if (m) {
      return m->Property() & kIsPublic;
   }
   return kFALSE;
}

//______________________________________________________________________________
TMethodArg *TRCppyy::GetMethodArg(TMethod *m, Int_t index)
{

   if (m)
      return (TMethodArg *)m->GetListOfMethodArgs()->At(index);
   return nullptr;
}

//______________________________________________________________________________
TMethodArg *TRCppyy::GetMethodArg(Int_t mindex, Int_t aindex)
{
   auto m = GetMethod(mindex);
   if (m)
      return (TMethodArg *)m->GetListOfMethodArgs()->At(aindex);
   return nullptr;
}

//______________________________________________________________________________
TList *TRCppyy::GetListOfMethodArgs(TMethod *m)
{
   if (m)
      return m->GetListOfMethodArgs();
   return nullptr;
}

//______________________________________________________________________________
TString TRCppyy::GetArgPrototype(Int_t mindex, Int_t aindex)
{
   auto ma = GetMethodArg(mindex, aindex);
   if (ma)
      return GetArgPrototype(ma);
   return nullptr;
}

//______________________________________________________________________________
TList *TRCppyy::GetListOfMethodArgs(Int_t index)
{
   auto m = GetMethod(index);
   if (m)
      return m->GetListOfMethodArgs();
   return nullptr;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsConstructor(Int_t index)
{
   auto m = GetMethod(index);
   if (m)
      return m->ExtraProperty() & kIsConstructor;
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsOperator(Int_t index)
{
   auto m = (TFunction *)GetMethod(index);
   if (m)
      return m->ExtraProperty() & kIsOperator;
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsArgEnum(TMethodArg *arg)
{
   return gInterpreter->ClassInfo_IsEnum(arg->GetTitle());
}

// enums are changed by Int_t to support compilation in Rcpp
// when the argument is passed we apply a cast to original enum type
//______________________________________________________________________________
TString TRCppyy::GetArgPrototype(TMethodArg *arg)
{
   TString prototype;
   if (IsArgEnum(arg)) {
      prototype = Form("%s %s", "Int_t", arg->GetName());
   } else {
      TString tittle = arg->GetTitle();
      if (tittle.Contains("(*)")) // in case of poibter to function
      {
         prototype = tittle.ReplaceAll("(*)", Form("(*%s)", arg->GetName()));
      } else {
         prototype = Form("%s %s", arg->GetTitle(), arg->GetName());
      }
   }
   return prototype;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsNamespace()
{
   if (fCl)
      return fCl->Property() & kIsNamespace;
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsNamespace(TClass *cl)
{
   if (cl)
      return cl->Property() & kIsNamespace;
   return kFALSE;
}
//______________________________________________________________________________
Bool_t TRCppyy::IsNamespace(const TString name)
{
   auto cl = gROOT->GetClass(name.Data());
   if (cl)
      return cl->Property() & kIsNamespace;
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsConstMethod(TMethod *m)
{
   if (m) {
      return m->Property() & kIsConstMethod;
   }
   return kFALSE;
}

//______________________________________________________________________________
std::vector<TString> TRCppyy::GetNameSpaces(const TString name, TString &cname)
{
   std::vector<TString> namespaces;
   if (name.Contains("::")) {
      auto names = name.Tokenize("::");
      for (auto i = 0; i < names->GetEntries(); i++) {
         auto item = ((TObjString *)names->At(i))->GetString();
         if (IsNamespace(item))
            namespaces.push_back(item);
         else
            cname = item;
      }
   }
   return namespaces;
}

//______________________________________________________________________________
std::vector<TString> TRCppyy::GetNameSpaces(const TString name)
{
   TString dummy;
   return GetNameSpaces(name, dummy);
}

//______________________________________________________________________________
std::vector<TString> TRCppyy::GetNameSpaces(TClass *cl)
{
   if (cl)
      return GetNameSpaces(cl->GetName());
   return std::vector<TString>();
}

//______________________________________________________________________________
Bool_t TRCppyy::IsConstructor(TMethod *m)
{
   if (m)
      return m->ExtraProperty() & kIsConstructor;
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TRCppyy::IsOperator(TMethod *m)
{
   auto f = (TFunction *)m;
   if (f)
      return f->ExtraProperty() & kIsOperator;
   return kFALSE;
}
