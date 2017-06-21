#include "TMVA/ParallelExecutor.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TColor.h"
#include "TStyle.h"
#include "TMVA/CrossValidation.h"

void ptmba_testcv(Int_t jobs)
{

    TFile f("dataset.root");

    TMVA::DataLoader *dataloader=(TMVA::DataLoader *)f.Get("dataset");
   
    
    TMVA::CrossValidation *cv=new TMVA::CrossValidation(dataloader);

    cv->BookMethod(TMVA::Types::kBDT, "BDT","!H:!V:NTrees=850:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
   
    cv->SetNumFolds(8);
    TMVA::ParallelExecutor ex;

    if(jobs==0)
    {
     cv->Evaluate();

     TMVA::MsgLogger::EnableOutput();
     TMVA::gConfig().SetSilent(kFALSE);   
     cv->GetResults().Print();
     cv->GetResults().Draw();
    }else if(jobs!=-1){
  
     auto r=ex.Execute(cv,jobs);

     TMVA::MsgLogger::EnableOutput();
     TMVA::gConfig().SetSilent(kFALSE);   
     r.Print();
    }
    
    if(jobs==-1)
    {
        std::vector<int> proc={1,2,3,4};
        TCanvas canvas;
        TH1F hist("cvtimes","CrossValidation Performance MultiProc",4,0,4);
        TLegend leg(0.6,0.6,0.9,0.9);
        leg.SetHeader("CPU Performance Jobs vs Time","C"); 
        for(UInt_t i:proc)
        {
            std::cout<<"Process : "<<i<<std::endl;
            std::cout.flush();
            auto re=ex.Execute(cv,i);
            hist.GetXaxis()->SetBinLabel(i,Form("%i",i));
            hist.SetBinContent(i,re.GetExecutionTime());
            leg.AddEntry(&hist,Form(" %i jobs = %.3f sec",i,re.GetExecutionTime()));
        }
        gStyle->SetOptStat(000000);
        hist.SetBarWidth(0.97);
        hist.Draw();
        //#hist.LabelsOption("v >", "X")
        hist.SetBarWidth(0.97);
        hist.SetFillColor(TColor::GetColor("#006600"));

        hist.GetXaxis()->SetTitle("Number of Jobs");
        hist.GetXaxis()->CenterTitle();
        hist.GetYaxis()->SetTitle("Time (Sec)");
        hist.GetYaxis()->CenterTitle();
        leg.Draw("Same");
        canvas.Draw();
        canvas.SaveAs("time.pdf");
    }
   
  f.Close(); 
}

Int_t main()
{
    std::vector<int> proc={1,2,3,4};
    for(auto &i:proc)
    {
        ptmba_testcv(i);
    }
    return 0;
}
