/// \file
/// \ingroup tutorial_mpi
///
///  Basic checkpoint in a loop
///
/// ~~~{.cpp}
///  rootmpi -np 2 ckploop.C
/// ~~~
///
///
/// \macro_output
/// \macro_code
///
/// \author Omar Zapata

using namespace ROOT::Mpi;

void ckploop()
{
   TEnvironment env; // environment to start communication system
   env.SetCkpCopyType("SINGLE");
   env.CkpInit();
   
   TCheckPoint ckp("loop");
   auto rst = ckp.GetRestarter(); // restarter object to check if we need to read the last checkpoint

   auto chunk = 4000 / COMM_WORLD.GetSize();
   
   for (auto i = 0; i < chunk; i++) {
      // we need to check is we are in recovery mode
      if (rst.IsRequired()) {
         rst.Restart();                   // require to load the last checkpoint
         auto ckpfile = rst.GetCkpFile(); // file with the last checkpoint saved
         ckpfile->ReadVar<Int_t>("i", i); // getting the last i saved
         rst.Complete();                  // required to tell to the system that the checkpoint was successfully loaded.
      }
      // hard work here

      // conditional to try to save the checkpoint every number of iterations
      // and in the last iteratino
      if (i % 100 == 0 || i == (chunk - 1)) {
         cout << "Processing iteration " << i << "..."
              << endl; // printing tghe interation where we are doing the checkpoint
         // ask SCR whether we need to checkpoint
         if (ckp.IsRequired()) {
            ckp.Start(); // require to create a new temporal file where we will to save the checkpoint
            auto ckpfile = ckp.GetCkpFile(); // file to save the data
            ckpfile->WriteVar("i", i);       // saving the data
            ckp.Complete();                  // require to tell to the system that the recovery was successfully saved.
         }
      }
      gSystem->Sleep(10); // just to give some time kill the process and test the rcovery mode.
   }
   env.CkpFinalize(); // require to finalize the environment from SCR
}
