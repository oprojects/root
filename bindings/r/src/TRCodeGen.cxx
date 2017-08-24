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
   fClassCode = Form("class R%s:public %s\n{\npublic:\n", cname.Data(), name.Data());
   auto mlist = fCppyy.GetListOfPublicMethods();
   for (auto i = 0; i < mlist->GetSize(); i++) {
      auto meth = fCppyy.GetMethod(i);
      auto args = fCppyy.GetListOfMethodArgs(i);
      if (!fCppyy.IsPublicMethod(i))
         continue;
      if (fCppyy.IsOperator(i))
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
         if (fCppyy.IsStaticMethod(i))
            fClassCode += "\tstatic ";
         else
            fClassCode += "\t";
         fClassCode += Form("%s %s(", meth->GetReturnTypeName(), meth->GetName());
         for (auto j = 0; j < args->GetSize(); j++) {
            fClassCode += fCppyy.GetArgPrototype(i, j);
            if (j < args->GetSize() - 1)
               fClassCode += ",";
         }
         if (fCppyy.IsConstMethod(meth)) {
            fClassCode += ") const{\n";
         } else
            fClassCode += "){\n";
         if (meth->GetReturnTypeName() != TString("void")) {
            fClassCode += Form("\t\treturn ::%s(", meth->GetName());
         } else {
            fClassCode += Form("\t\t::%s(", meth->GetName());
         }
         for (auto j = 0; j < args->GetSize(); j++) {
            fClassCode += fCppyy.GetMethodArg(i, j)->GetName();
            if (j < args->GetSize() - 1)
               fClassCode += ",";
         }
         fClassCode += ");\n";
         fClassCode += "\t}\n";
      }
   }
   fClassCode += "};\n";
   return fClassCode;
}

//______________________________________________________________________________
TString TRCodeGen::GenWrap()
{
   auto cname = fCppyy.GetClearName();
   auto name = fCppyy.GetName();
   auto mlist = fCppyy.GetListOfPublicMethods();
   fWrapCode = "";
   for (auto i = 0; i < mlist->GetSize(); i++) {
      auto meth = fCppyy.GetMethod(i);
      auto args = fCppyy.GetListOfMethodArgs(i);
      if (!fCppyy.IsPublicMethod(i))
         continue;
      if (fCppyy.IsConstructor(i)) {

      } else {
         fWrapCode += Form("\t.method(\"%s\",(%s (R%s::*)(", meth->GetName(), meth->GetReturnTypeName(), cname.Data());
         for (auto j = 0; j < args->GetSize(); j++) {
            fWrapCode += fCppyy.GetMethodArg(i, j)->GetTitle();
            if (j != args->GetSize() - 1)
               fWrapCode += ",";
         }
         if (fCppyy.IsConstMethod(meth)) {
            fWrapCode += Form(")const)&%s::%s)\n", cname.Data(), meth->GetName());
         } else
            fWrapCode += Form("))&%s::%s)\n", cname.Data(), meth->GetName());
      }
   }
   return fWrapCode;
}
