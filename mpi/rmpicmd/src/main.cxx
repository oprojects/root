#include <Mpi/TCmdExecutor.h>
#include <iostream>

Int_t main(const Int_t argc, const Char_t *argv[])
{
   ROOT::Mpi::TCmdExecutor exec(argc, argv);
   auto status = exec.Execute();
   return status == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
