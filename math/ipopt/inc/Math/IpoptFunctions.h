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
      Retrieve the dimension of the constraint function
    */
   virtual unsigned int NDim() const = 0;

   /**
      Retrieve the dimension of the return of constraint function
    */
   virtual unsigned int NDimConstraint() const = 0;

   /**
       Evaluate the function at a point x[].
       Use the pure virtual private method DoEval which must be implemented by the sub-classes
   */
   void operator()(const T *x, T *g) const { DoEval(x, g); }

   /**
      Implementation of the evaluation pf jacobian function. Must be implemented by derived classes
      \param x point to eval
      \param value of the jacobian in the coordinates i,j
      \param icoord coordinate i of the jacobian matrix
      \param jcoord coordinate j of the jacobian matrix
   */
   virtual bool DoJacobian(const double *x, double &value, unsigned int icoord, unsigned int jcoord) const = 0;

private:
   /**
      Implementation of the evaluation function. Must be implemented by derived classes
   */
   virtual void DoEval(const T *x, T *g) const = 0;
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
   ConstraintFunctorHandler(const Func &fun) : fDim(1), fDimConstraint(1), fFunc(fun) {}

   // constructor for multi-dimensional functions w/0 NDim()
   ConstraintFunctorHandler(unsigned int dim, unsigned int dimconstraint, const Func &fun)
      : fDim(dim), fDimConstraint(dimconstraint), fFunc(fun)
   {
   }

   virtual ~ConstraintFunctorHandler() {}

   // copy of the function handler (use copy-ctor)
   ImplFunc *Copy() const { return new ConstraintFunctorHandler(*this); }

   // clone of the function handler (use copy-ctor)
   BaseFunc *Clone() const { return Copy(); }

   // constructor for multi-dimensional functions
   unsigned int NDim() const { return fDim; }

   unsigned int NDimConstraint() const { return fDimConstraint; }

private:
   inline void DoEval(const double *x, double *g) const { fFunc(x, g); }

   inline bool DoJacobian(const double *x, double &value, unsigned int icoord, unsigned int jcoord) const
   {
      return fFunc.DoJacobian(x, value, icoord, jcoord);
   }
   //    inline double DoJacobian(const double *x, unsigned int icoord,unsigned int jcoord) const { return
   //    fFunc.Derivative(x, icoord); }

   unsigned int fDim;
   unsigned int fDimConstraint;
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
   ConstraintFunctor(const PtrObj &p, MemFn memFn, unsigned int dim, unsigned int dimconstraint)
      : fImpl(new MemFunHandler<Functor, PtrObj, MemFn>(dim, dimconstraint, p, memFn))
   {
   }

   /**
      construct from a callable object of multi-dimension
      with the right signature (implementing operator()(double *x)
    */
   template <typename Func>
   ConstraintFunctor(const Func &f, unsigned int dim, unsigned int dimconstraint)
      : fImpl(new ConstraintFunctorHandler<ConstraintFunctor, Func>(dim, dimconstraint, f))
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

   unsigned int NDimConstraint() const { return fImpl->NDimConstraint(); }

private:
   inline void DoEval(const double *x, double *g) const { (*fImpl)(x, g); }
   inline bool DoJacobian(const double *x, double &value, unsigned int icoord, unsigned int jcoord) const
   {
      return fImpl->DoJacobian(x, value, icoord, jcoord);
   }

   std::unique_ptr<Impl> fImpl; // pointer to base functor handler
};
}
}

#endif
