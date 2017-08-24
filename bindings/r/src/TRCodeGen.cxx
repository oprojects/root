/*************************************************************************
 * Copyright (C) 2013-2017, Omar Andres Zapata Mesa                      *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#include <TRCodeGen.h>
#include <TROOT.h>
#include <TInterpreter.h>

using namespace ROOT::R;

//______________________________________________________________________________
TRCodeGen::TRCodeGen(TClass *cl) : fCppyy(cl)
{
}

//______________________________________________________________________________
TRCodeGen::TRCodeGen(const TString name) : fCppyy(name)
{
}

//______________________________________________________________________________
TString TRCodeGen::GenClass()
{
   auto cname = fCppyy.GetClearName();
   auto name = fCppyy.GetName();
   fClassCode += Form("class R%s:public %s\n{\npublic:\n", cname.Data(), name.Data());
   auto mlist = fCppyy.GetListOfPublicMethods();
   for (auto i = 0; i < mlist->GetSize(); i++) {
      auto args = fCppyy.GetListOfMethodArgs(i);
      if (!fCppyy.IsPublicMethod(i))
         continue;
      if (fCppyy.IsConstructor(i)) {

         if (args->GetSize() == 0) {
            fClassCode += Form("\tR%s():%s(){}\n", cname.Data(), name.Data());
         } else {
            fClassCode += Form("\tR%s(", cname.Data());
            for (auto j = 0; j < args->GetSize(); j++) {
               fClassCode += fCppyy.GetArgPrototype(i, j);
               if (j < args->GetSize() - 1)
                  fClassCode += ",";
            }
            fClassCode += Form("):%s(", name.Data());
            for (auto j = 0; j < args->GetSize(); j++) {
               auto arg = fCppyy.GetMethodArg(i, j);
               if (fCppyy.IsArgEnum(arg)) {
                  fClassCode += Form("(%s)%s", arg->GetTitle(), arg->GetName());
               } else {
                  fClassCode += arg->GetName();
               }
               if (j < args->GetSize() - 1)
                  fClassCode += ",";
            }
            fClassCode += "){}\n";
         }
      } else {
      }
   }
   fClassCode += "};\n";
   return fClassCode;
}
