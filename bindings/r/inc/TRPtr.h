// @(#)root/r:$Id$
// Author: Omar Zapata   20/7/2017

/*************************************************************************
 * Copyright (C) 2013-2017, Omar Andres Zapata Mesa                      *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_R_TRPtr
#define ROOT_R_TRPtr

#include <RExports.h>
#include <Rcpp/XPtr.h>
namespace ROOT {
namespace R {
template <class T>
class TRPtr {
private:
   SEXP fObj;      // insternal Rcpp::RObject
   Bool_t fStatus; // status tell if is a valid object
public:
   explicit TRPtr(SEXP ptr) { fObj = Rcpp::XPtr<SEXP>(ptr, R_NilValue, R_NilValue); }
   explicit TRPtr(const T *ptr) : fObj(Rcpp::XPtr<T>(const_cast<T *>(ptr))) {}

   const T &operator[](std::size_t idx) { return Rcpp::XPtr<T>(fObj).get()[idx]; }

   void operator=(T xx) { fObj = Rcpp::XPtr<T>(&xx); }

   void operator=(T *xx) { fObj = Rcpp::XPtr<T>(xx); }

   void operator=(const T *xx) { fObj = Rcpp::XPtr<T>(const_cast<T *>(xx)); }

   T *As() { return Rcpp::XPtr<T>(fObj).get(); }

   operator SEXP() { return fObj; }

   operator SEXP() const { return fObj; }

   operator T *() { return Rcpp::XPtr<T>(fObj).get(); }
};
}
}

#endif
