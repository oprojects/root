//
// This tutorials demonstrate how to store and restore simple vectors
// in a TTree
//

#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TH1F.h"
#include "TBenchmark.h"
#include "TRandom.h"
#include "TSystem.h"

#ifdef __MAKECINT__
#pragma link C++ class vector<float>+;
#endif

void write()
{
   TFile f("tree.root","recreate");
   TTree t1("t1","a simple Tree with simple variables");
   Float_t px, py, pz;
   t1.Branch("px",&px,"px/F");
   t1.Branch("py",&py,"py/F");
   t1.Branch("pz",&pz,"pz/F");
   
}


void read()
{

   TFile *f = TFile::Open("hvector.root","READ");

   if (!f) { return; }

   TTree *t; f->GetObject("tvec",t);

   std::vector<float> *vpx = 0;

  // Create a new canvas.
   TCanvas *c1 = new TCanvas("c1","Dynamic Filling Example",200,10,700,500);
   c1->SetFillColor(42);
   c1->GetFrame()->SetFillColor(21);
   c1->GetFrame()->SetBorderSize(6);
   c1->GetFrame()->SetBorderMode(-1);

   const Int_t kUPDATE = 1000;

   TBranch *bvpx = 0;
   t->SetBranchAddress("vpx",&vpx,&bvpx);


   // Create one histograms
   TH1F *h = new TH1F("h","This is the px distribution",100,-4,4);
   h->SetFillColor(48);

   for (Int_t i = 0; i < 25000; i++) {

      Long64_t tentry = t->LoadTree(i);
      bvpx->GetEntry(tentry);

      for (UInt_t j = 0; j < vpx->size(); ++j) {

         h->Fill(vpx->at(j));

      }
      if (i && (i%kUPDATE) == 0) {
         if (i == kUPDATE) h->Draw();
         c1->Modified();
         c1->Update();
         if (gSystem->ProcessEvents())
            break;
      }
   }

   // Since we passed the address of a local variable we need
   // to remove it.
   t->ResetBranchAddresses();
}


void hvector()
{
   gBenchmark->Start("hvector");

   write();
   read();

   gBenchmark->Show("hvector");
}
