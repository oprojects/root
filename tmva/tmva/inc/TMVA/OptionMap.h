// @(#)root/tmva:$Id$
// Author: Omar Zapata   2016

/*************************************************************************
 * Copyright (C) 2016, Omar Andres Zapata Mesa                           *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_TMVA_OptionMap
#define ROOT_TMVA_OptionMap

#include <sstream>
#include<iostream>
#include<map>

#ifndef ROOT_TNamed
#include<TNamed.h>
#endif

#ifndef ROOT_TMVA_MsgLogger
#include "TMVA/MsgLogger.h"
#endif

namespace TMVA {
       
       /**
        *      \class OptionMapBase
        *         class to storage options for the differents methods
        *         \ingroup TMVA
        */
       
       class OptionMap:public TNamed
       {
       protected:
           std::map<const TString,TString> fOptMap;
           TMVA::MsgLogger fLogger; //
           
           class Binding
           {
           private:
               std::map<const TString,TString> &fInternalMap;
               TString fInternalKey;
           public:
               Binding(std::map<const TString,TString>  &fmap,TString key):fInternalMap(fmap),fInternalKey(key){}
               ~Binding(){}
               void SetKey(TString key){fInternalKey=key;}
               TString GetKey(){return fInternalKey;}
               Binding &operator=(const Binding &obj)
               {
                   fInternalMap = obj.fInternalMap;    
                   fInternalKey = obj.fInternalKey;
                   return *this;
               }
               
               template<class T> Binding& operator=(const T &value)
               {
                   std::stringstream oss;
                   oss<<value;
                   fInternalMap[fInternalKey]=oss.str();
                   return *this;
               }
               
               template<class T> operator T()
               {
                   T result;
                   std::stringstream oss;
                   oss<<fInternalMap[fInternalKey].Data();
                   oss>>result;
                   return result;
               }
           };
           Binding fBinder;
       public:
           OptionMap(const TString name="Option"):TNamed(name.Data(),"OptionMap"),fLogger(name.Data()),fBinder(fOptMap,""){}
           ~OptionMap(){}
           
           Bool_t HasKey(TString key)
           {
               return fOptMap.count( key )==1;
           }
           
           Binding& operator[](TString key)
           {
               fBinder.SetKey(key);
               return fBinder;
           }
           using TNamed::Print;
           virtual void Print()
           {
               for(auto &item:fOptMap)
               {
                   fLogger<<kINFO<<item.first.Data()<<": "<<item.second.Data()<<std::endl;
               }
               fLogger<<Endl;
           }
           
       };
       
}

#endif
