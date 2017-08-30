#include <Mpi/TCheckPoint.h>
#include <Mpi/TEnvironment.h>
#include <Mpi/TIntraCommunicator.h>

using namespace ROOT::Mpi;

//______________________________________________________________________________
TCheckPoint::TCheckPoint(const TString name, const TString path) : fName(name), fPath(path)
{
   if (!gSystem->AccessPathName(path.Data())) {
      gSystem->mkdir(path.Data(), kTRUE);
   }
   gSystem->Setenv("ROOT_JOBID", Form("%d", COMM_WORLD.GetRank()));
   gSystem->Setenv("ROOT_NODELIST", TEnvironment::GetProcessorName().Data());
   //     if(COMM_WORLD.IsMainProcess())
   gSystem->Setenv("SCR_CLUSTER_NAME", TEnvironment::GetProcessorName().Data());
   auto cpkpath = CkpPathName();
   if (!gSystem->AccessPathName(cpkpath.Data())) {
      gSystem->mkdir(cpkpath.Data(), kTRUE);
   }
   COMM_WORLD.Barrier();
}

//______________________________________________________________________________
TString TCheckPoint::CkpPathName()
{
   Char_t file[SCR_MAX_FILENAME];
   SCR_Route_file(fName.Data(), file);

   return file;
}

//______________________________________________________________________________
void TCheckPoint::Start()
{
   SCR_Start_checkpoint();
}

//______________________________________________________________________________
void TCheckPoint::Complete(Int_t valid)
{
   SCR_Complete_checkpoint(valid);
}

//______________________________________________________________________________
Int_t TCheckPoint::NeedCheckPoint()
{
   Int_t flag = 0;
   SCR_Need_checkpoint(&flag);
   return flag;
}
