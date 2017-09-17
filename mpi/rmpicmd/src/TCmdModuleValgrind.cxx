// @(#)root/mpi / Author: Omar.Zapata@cern.ch 2017 http://oproject.org
#include <Mpi/TCmdModuleValgrind.h>
using namespace ROOT::Mpi;
TCmdModuleValgrind::TCmdModuleValgrind(const Int_t argc, const Char_t **argv) : TCmdModuleBase("Valgrind", argc, argv)
{
#if defined(ROOT_MPI_VALGRINDFOUND)
   fExecPath = ROOT_MPI_VALGRIND;
#endif
   fROOTSupFile = Form("--suppressions=%s/etc/valgrind-root.supp", TROOT::GetRootSys().Data());
}

//______________________________________________________________________________
void TCmdModuleValgrind::ParseOptions()
{
   for (auto i = 0; i < fArgc; i++) {
      TString arg = fArgv[i];
      arg.ReplaceAll(" ", "");
      if (arg == "-v" || arg == "-q") {
         fOptions[arg] = "";
         continue;
      }
      // special case(option) to add ROOT suppression file
      if (arg == "--root-suppressions") {
         fOptions[arg] = fROOTSupFile;
         continue;
      }
      if (arg.Contains("=")) {
         auto opt = ParseEqual(arg);
         auto key = opt.first;
         auto value = opt.second;
         if (key == "--tool") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--trace-children") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--trace-children-skip") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--trace-children-skip-by-arg") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--child-silent-after-fork") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--vgdb") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--vgdb-error") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--vgdb-stop-at") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--track-fds") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--time-stamp") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--log-fd") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--log-file") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--log-socket") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xml") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xml-fd") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xml-file") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xml-socket") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xml-user-comment") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--demangle") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--num-callers") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--error-limit") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--error-exitcode") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--default-suppressions") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--suppressions") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--gen-suppressions") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--dsymutil") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--max-stackframe") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--alignment") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--redzone-size") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xtree-memory") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xtree-memory-file") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--fullpath-after") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--smc-check") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--vgdb-poll") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--vgdb-shadow-registers") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--vgdb-prefix") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--run-libc-freeres") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--run-cxx-freeres") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--fair-sched") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--kernel-variant") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--merge-recursive-frames") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--num-transtab-sectors") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--avg-transtab-entry-size") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--show-emwarns") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--require-text-symbol") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--soname-synonyms") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--sigill-diagnostics") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--unw-stack-scan-thresh") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--unw-stack-scan-frames") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--resync-filter") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--max-threads") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--leak-check") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--leak-resolution") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--leak-check-heuristics") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--show-reachable") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--show-reachable") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--show-reachable") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xtree-leak") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--xtree-leak-file") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--undef-value-errors") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--partial-loads-ok") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--freelist-vol") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--freelist-big-blocks") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--workaround-gcc296-bugs") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--ignore-ranges") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--ignore-range-below-sp") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--malloc-fill") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--free-fill") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--keep-stacktraces") {
            fOptions[key] = value;
            continue;
         }
         if (key == "--show-mismatched-frees") {
            fOptions[key] = value;
            continue;
         }
      }
   }
}

//______________________________________________________________________________
TString TCmdModuleValgrind::GetOptionsString()
{
   TString optstr;
   for (auto &opt : fOptions) {
      if (opt.first == "-v" || opt.first == "-q")
         optstr += Form(" %s ", opt.first.Data());
      else if (opt.first == "--root-suppressions")
         optstr += Form(" %s ", opt.second.Data());
      else
         optstr += Form(" %s=%s ", opt.first.Data(), opt.second.Data());
   }
   return optstr;
}
