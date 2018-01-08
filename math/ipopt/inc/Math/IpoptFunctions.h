// @(#)root/ipopt:$Id$
// Author: Omar.Zapata@cern.ch Thu Jan 7 2:15:00 2018

/*************************************************************************
 * Copyright (C) 2018, Omar Zapata                                  *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_Math_IpoptFunctions
#define ROOT_Math_IpoptFunctions

#include <Math/IFunction.h>
#include "Math/IFunctionfwd.h"

#include <Math/Functor.h>
#include "Types.h"

namespace ROOT {

namespace Math {

//___________________________________________________________________________________
/**
    Documentation for the abstract class IBaseConstraintFunctionMultiDim.
    Interface (abstract class) for constraint functions  \f$g(x): R^n --> R^m \f$  objects of multi-dimension
    Provides a method to evaluate the function given a vector of coordinate values,
    by implementing operator() (const double *).
    In addition it defines the interface for copying functions via the pure virtual method Clone()
    and the interface for getting the function dimension via the NDim() method.
    Derived classes must implement the pure private virtual method DoEval(const double *) for the
    function evaluation in addition to NDim() and Clone().

    @ingroup  GenFunc
*/

template <class T>
class IBaseConstraintFunctionMultiDimTempl {

public:
   typedef T BackendType;
   typedef IBaseConstraintFunctionMultiDimTempl<T> BaseFunc;

   IBaseConstraintFunctionMultiDimTempl() {}

   /**
      virtual destructor
    */
   virtual ~IBaseConstraintFunctionMultiDimTempl() {}

   /**
       Clone a function.
       Each derived class must implement their version of the Clone method
   */
   virtual IBaseConstraintFunctionMultiDimTempl<T> *Clone() const = 0;

   /**
      Retrieve the dimension of the function
    */
   virtual unsigned int NDim() const = 0;

   /**
       Evaluate the function at a point x[].
       Use the pure virtual private method DoEval which must be implemented by the sub-classes
   */
   T *operator()(const T *x) const { return DoEval(x); }

#ifdef LATER
   /**
      Template method to eveluate the function using the begin of an iterator
      User is responsible to provide correct size for the iterator
   */
   template <class Iterator>
   T operator()(const Iterator it) const
   {
      return DoEval(&(*it));
   }
#endif

private:
   /**
      Implementation of the evaluation function. Must be implemented by derived classes
   */
   virtual T *DoEval(const T *x) const = 0;
};
using IBaseConstraintFunctionMultiDim = IBaseConstraintFunctionMultiDimTempl<double>;
typedef IBaseConstraintFunctionMultiDim IMultiConstraintFunction;

/**
   Constraint Functor Handler class is responsible for wrapping any other functor and pointer to
   free C functions.
   It can be created from any function implementing the correct signature
   corresponding to the requested type
   In the case of one dimension the function evaluation object must implement
   double operator() (double x). If it implements a method:  double Derivative(double x)
   can be used to create a Gradient function type.

   In the case of multi-dimension the function evaluation object must implement
   double operator()(const double *x). If it implements a method:
   double Derivative(const double *x, int icoord)
   can be used to create a Gradient function type.

   @ingroup  Functor_int

*/
template <class ParentFunctor, class Func>
class ConstraintFunctorHandler : public ParentFunctor::Impl {

   typedef typename ParentFunctor::Impl ImplFunc;
   typedef typename ImplFunc::BaseFunc BaseFunc;
   // typedef typename ParentFunctor::Dim Dim;

public:
   // constructor for 1d functions
   ConstraintFunctorHandler(const Func &fun) : fDim(1), fFunc(fun) {}

   // constructor for multi-dimensional functions w/0 NDim()
   ConstraintFunctorHandler(unsigned int dim, const Func &fun) : fDim(dim), fFunc(fun) {}

   virtual ~ConstraintFunctorHandler() {}

   // copy of the function handler (use copy-ctor)
   ImplFunc *Copy() const { return new ConstraintFunctorHandler(*this); }

   // clone of the function handler (use copy-ctor)
   BaseFunc *Clone() const { return Copy(); }

   // constructor for multi-dimensional functions
   unsigned int NDim() const { return fDim; }

private:
   //    inline double DoEval (double x) const {
   //       return fFunc(x);
   //    }

   inline double *DoEval(const double *x) const { return fFunc(x); }

   inline double DoDerivative(double x) const { return fFunc.Derivative(x); }

   inline double DoDerivative(const double *x, unsigned int icoord) const { return fFunc.Derivative(x, icoord); }

   unsigned int fDim;
   mutable Func fFunc; // should here be a reference and pass a non-const ref in ctor
};

/**
   Documentation for class Functor class.
   It is used to wrap in a very simple and convenient way multi-dimensional function objects.
   It can wrap all the following types:
   <ul>
   <li> any C++ callable object implemention double operator()( const double *  )
   <li> a free C function of type double ()(const double * )
   <li> a member function with the correct signature like Foo::Eval(const double * ).
       In this case one pass the object pointer and a pointer to the member function (&Foo::Eval)
   </ul>
   The function dimension is required when constructing the functor.

   @ingroup  GenFunc

 */
class ConstraintFunctor : public IBaseConstraintFunctionMultiDim {

public:
   typedef FunctorImpl<IBaseConstraintFunctionMultiDim> Impl;
   typedef IBaseConstraintFunctionMultiDim::BaseFunc ImplBase;

   /**
      Default constructor
   */
   ConstraintFunctor() {}

   /**
       construct from a pointer to member function (multi-dim type)
    */
   template <class PtrObj, typename MemFn>
   ConstraintFunctor(const PtrObj &p, MemFn memFn, unsigned int dim)
      : fImpl(new MemFunHandler<Functor, PtrObj, MemFn>(dim, p, memFn))
   {
   }

   /**
      construct from a callable object of multi-dimension
      with the right signature (implementing operator()(double *x)
    */
   template <typename Func>
   ConstraintFunctor(const Func &f, unsigned int dim)
      : fImpl(new ConstraintFunctorHandler<ConstraintFunctor, Func>(dim, f))
   {
   }

   /**
      Destructor (no operations)
   */
   virtual ~ConstraintFunctor() {}

   /**
      Copy constructor for functor based on ROOT::Math::IMultiGenFunction
   */
   ConstraintFunctor(const ConstraintFunctor &rhs) : ImplBase()
   {
      if (rhs.fImpl)
         fImpl = std::unique_ptr<Impl>((rhs.fImpl)->Copy());
   }
   // need a specialization in order to call base classes and use  clone

   /**
      Assignment operator
   */
   ConstraintFunctor &operator=(const ConstraintFunctor &rhs)
   {
      ConstraintFunctor copy(rhs);
      fImpl.swap(copy.fImpl);
      return *this;
   }

   // clone of the function handler (use copy-ctor)
   ImplBase *Clone() const { return new ConstraintFunctor(*this); }

   // for multi-dimensional functions
   unsigned int NDim() const { return fImpl->NDim(); }

private:
   inline double *DoEval(const double *x) const { return (*fImpl)(x); }
   std::unique_ptr<Impl> fImpl; // pointer to base functor handler
};

//     class IMultiConstraintFunction:public IBaseFunctionMultiDimTempl<double>{};
}
}

#endif
