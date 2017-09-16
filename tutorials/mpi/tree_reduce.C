/// \file
/// \ingroup tutorial_mpi
///
/// Example to generated random numbers to fill a TTree in every process
/// and merging the result through a custom reduce operation.
/// to execute this example with 4 processors, do:
///
/// ~~~{.cpp}
///  rootmpi -np 4 tree_reduce.C
/// ~~~
///
///
/// \macro_output
/// \macro_code
///
/// \author Omar Zapata
#include <TMpi.h>
#include <TH1F.h>
#include <TTree.h>
#include <TRandom.h>
using namespace ROOT::Mpi;

TOp<TTree> TMERGE()
{
   return TOp<TTree>([](const TTree *a, const TTree *b, Int_t count) {
      TList *list = new TList;
      list->Add(const_cast<TTree *>(a));
      list->Add(const_cast<TTree *>(b));
      TTree *ntree = TTree::MergeTrees(list);
      ntree->SetName(Form("ntree%d", COMM_WORLD.GetRank()));
      delete list;
      return ntree;
   });
}

void tree_reduce()
{
   TEnvironment env;

   auto root = 0;
   auto rank = COMM_WORLD.GetRank();

   if (COMM_WORLD.GetSize() == 1)
      return; // need at least 2 process

   TTree *tree = new TTree(Form("tree%d", COMM_WORLD.GetRank()), "simple tree");

   Double_t value;
   // create a branch with energy
   tree->Branch("value", &value);

   Int_t nevent = 100000;

   auto chunk = nevent / COMM_WORLD.GetSize();

   for (Int_t iev = 0; iev < chunk; iev++) {
      value = gRandom->Gaus();
      tree->Fill();
   }

   TTree result;

   COMM_WORLD.Reduce(tree, &result, 1, TMERGE, root);

   if (rank == 0) {
      TFile *f = new TFile("tree.root", "RECREATE");
      result.Write();
      f->Close();
      delete f;
   }

   delete tree;
}
int main()
{
   tree_reduce();
   return 0;
}
