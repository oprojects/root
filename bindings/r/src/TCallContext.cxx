// Bindings
#include "TRInterface.h"
#include "TCallContext.h"

//- data _____________________________________________________________________
namespace ROOT {
namespace R {

TCallContext::ECallFlags TCallContext::sMemoryPolicy = TCallContext::kUseHeuristics;
// this is just a data holder for linking; actual value is set in RootModule.cxx
TCallContext::ECallFlags TCallContext::sSignalPolicy = TCallContext::kSafe;
}
} // namespace ROOT

////////////////////////////////////////////////////////////////////////////////
/// Set the global memory policy, which affects object ownership when objects
/// are passed as function arguments.

Bool_t ROOT::R::TCallContext::SetMemoryPolicy(ECallFlags e)
{
   if (kUseHeuristics == e || e == kUseStrict) {
      sMemoryPolicy = e;
      return kTRUE;
   }
   return kFALSE;
}

////////////////////////////////////////////////////////////////////////////////
/// Set the global signal policy, which determines whether a jmp address
/// should be saved to return to after a C++ segfault.

Bool_t ROOT::R::TCallContext::SetSignalPolicy(ECallFlags e)
{
   if (kFast == e || e == kSafe) {
      sSignalPolicy = e;
      return kTRUE;
   }
   return kFALSE;
}
