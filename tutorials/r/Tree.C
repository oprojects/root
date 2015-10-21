#include<TRInterface.h>
 
using namespace ROOT::R;

void write()
{
   //create the file, the Tree and a few branches
   TFile f("tree.root","recreate");
   TTree t("tvec","a simple Tree with simple variables");
   Float_t px, py, pz;
   t.Branch("px",&px,"px/F");
   t.Branch("py",&py,"py/F");
   t.Branch("pz",&pz,"pz/F");

   //fill the tree
   std::cout<<Form("px\t\tpy\t\tpz")<<std::endl;
   for (Int_t i=0;i<10;i++) {
     gRandom->Rannor(px,py);
     pz = px*px + py*py;
     std::cout<<Form("%f\t%f\t%f",px,py,pz)<<std::endl;
     t.Fill();
  }

  //save the Tree header. The file will be automatically closed
  //when going out of the function scope
  t.Write();   
  f.Close();
}

void read()
{
    TRInterface &r=TRInterface::Instance();
    TFile *f = TFile::Open("tree.root","READ");

    if (!f) { return; }
    TTree *t; 
    f->GetObject("tvec",t);
       

    Float_t px,py,pz;
    t->SetBranchAddress("px",&px);
    t->SetBranchAddress("py",&py);
    t->SetBranchAddress("pz",&pz);
    
    Long64_t nentries=t->GetEntries();
    std::vector<float> vpx;
    std::vector<float> vpy;
    std::vector<float> vpz;

    for(Long64_t i=0;i<nentries;i++)
    {
        t->GetEntry(i);
        vpx.push_back(px);
        vpy.push_back(py);
        vpz.push_back(pz);
    }
    t->ResetBranchAddresses();
    f->Close();
    
    TRDataFrame tvec;
    tvec["px"]=vpx;
    tvec["py"]=vpy;
    tvec["pz"]=vpz;

    r["tvec"]<<tvec;
    r<<"print(tvec)";
}

void Tree()
{
    std::cout<<"creating tree.root"<<std::endl;
    write();
    std::cout<<"reading tree.root and maping TTree tvec in DataFrame"<<std::endl;
    read();
}