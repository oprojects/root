// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#ifndef ROOT_Mpi_TOp
#define ROOT_Mpi_TOp
#include <Rtypes.h>
namespace ROOT {
namespace Mpi {
/**
 * \class TOp
 * template class with helper functions to perform operations in a reduce
 * schema.
 * \see also ROOT::Mpi::SUM ROOT::Mpi::PROD ROOT::Mpi::MIN ROOT::Mpi::MAX
 * \ingroup Mpi
 */

template <typename T>
class TOp {
   T (*fOp)(const T &, const T &);
   T *(*fOpPtr)(const T *, const T *, Int_t);

public:
   TOp(T (*op)(const T &, const T &)) : fOp(op), fOpPtr(nullptr) {}
   TOp(T *(*op)(const T *, const T *, Int_t)) : fOp(nullptr), fOpPtr(op) {}
   TOp(const TOp<T> &op) : fOp(op.fOp), fOpPtr(op.fOpPtr) {}

   TOp<T> &operator=(TOp<T> const &obj)
   {
      fOp = obj.fOp;
      fOpPtr = obj.fOpPtr;
      return *this;
   }

   void SetFunction(T (*op)(const T &, const T &)) { fOp = op; }
   void SetFunction(T *(*op)(const T *, const T *, Int_t)) { fOpPtr = op; }

   Bool_t IsPrtFunction() { return fOpPtr != nullptr ? kTRUE : kFALSE; }
   //______________________________________________________________________________
   /**
    * Method to call the encapsulate function with the operation.
    * \param a object to perform the binary operation
    * \param b object to perform the binary operation
    * \return object with the result of the operation.
    */
   T Call(const T &a, const T &b) const { return fOp(a, b); }
   T *Call(const T *a, const T *b, Int_t count) const { return fOpPtr(a, b, count); }

   T operator()(const T &a, const T &b) const { return fOp(a, b); }
   T *operator()(const T *a, const T *b, Int_t count) const { return fOpPtr(a, b, count); }
};

//______________________________________________________________________________
/**
 * template utility function to perform SUM operation,
 * if a and b are objects then a and b must be overloaded the operator +
 * \return object of TOp with the operation + saved like a function.
 */
template <class T>
TOp<T> SUM()
{
   TOp<T> op([](const T *a, const T *b, Int_t count) {
      std::vector<T> *r = new std::vector<T>;
      for (auto i = 0; i < count; i++)
         r->push_back(a[i] + b[i]);
      return (T *)&(*r)[0];
   });
   op.SetFunction([](const T &a, const T &b) { return a + b; });
   return op;
}

//______________________________________________________________________________
/**
 * template utility function to perform PROD operation,
 * if a and b are objects then a and b must be overloaded the operator *
 * \return object of TOp with the operation * saved like a function.
 */
template <class T>
TOp<T> PROD()
{
   TOp<T> op([](const T *a, const T *b, Int_t count) {
      std::vector<T> *r = new std::vector<T>;
      for (auto i = 0; i < count; i++)
         r->push_back(a[i] * b[i]);
      return (T *)&(*r)[0];
   });
   op.SetFunction([](const T &a, const T &b) { return a * b; });
   return op;
}

//______________________________________________________________________________
/**
 * template utility function to perform MIN operation,
 * if a and b are objects then a and b must be overloaded the operator <
 * \return object of TOp with operation function that compute the min
 * between two values.
 */
template <class T>
TOp<T> MIN()
{
   TOp<T> op([](const T *a, const T *b, Int_t count) {
      std::vector<T> *r = new std::vector<T>;
      for (auto i = 0; i < count; i++)
         r->push_back(a[i] < b[i] ? a[i] : b[i]);
      return (T *)&(*r)[0];
   });
   op.SetFunction([](const T &a, const T &b) { return a < b ? a : b; });
   return op;
}

//______________________________________________________________________________
/**
 * template utility function to perform MAX operation,
 * if a and b are objects then a and b must be overloaded the operator >
 * \return object of TOp with operation function that compute the max
 * between two values.
 */
template <class T>
TOp<T> MAX()
{
   TOp<T> op([](const T *a, const T *b, Int_t count) {
      std::vector<T> *r = new std::vector<T>;
      for (auto i = 0; i < count; i++)
         r->push_back(a[i] > b[i] ? a[i] : b[i]);
      return (T *)&(*r)[0];
   });
   op.SetFunction([](const T &a, const T &b) { return a > b ? a : b; });
   return op;
}

} // end namespace Mpi
} // end namespace ROOT

#endif
