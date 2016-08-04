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
       
       class OptionMap
       {
       protected:
           TString fName;
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
                   oss<<fInternalMap[fInternalKey];
                   oss>>result;
                   return result;
               }
               template<class T> T GetValue()
               {
                   T result;
                   std::stringstream oss;
                   oss<<fInternalMap[fInternalKey];
                   oss>>result;
                   return result;
               }
           };
           Binding fBinder;
       public:
           OptionMap(const TString name="Option"):fName(name),fLogger(name.Data()),fBinder(fOptMap,""){}
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
           
           void Print() const
           {
               MsgLogger Log(fLogger);
               for(auto &item:fOptMap)
               {
                   Log<<kINFO<<item.first.Data()<<": "<<item.second.Data()<<Endl;
               }
           }

           template<class T> T GetValue(const TString key)
           {
               T result;
               std::stringstream oss;
               oss<<fOptMap[key];
               oss>>result;
               return result;
           }
           
           template<class T> T GetValue(const TString key) const
           {
               T result;
               std::stringstream oss;
               oss<<fOptMap.at(key);
               oss>>result;
               return result;
           }
           
       };
       
}

#endif
