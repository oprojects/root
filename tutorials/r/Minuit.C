
#include "TRInterface.h"
#include "TH1.h"
#include "TF1.h"
#include "TCanvas.h"
#include "Math/MinimizerOptions.h"

//R example to fit with Minuit 
// x <- seq(1,50, 0.1)
// a <- 1e-3
// b <- 3 
// y <- a * x ^ b
// my.data <- data.frame(x,y)
// nls( y ~ a * x ^ b, data = my.data, start = list(a = 0.8e-2, b = 2))//


ROOT::R::TRFunctionImport *rfun=NULL;

//example wraping function from R in C
Double_t fitfun(Double_t *x, Double_t *par)//declaring c function that calls R's function
{
    return (Double_t)(*rfun)(x[0],par[0],par[1]);
    return par[0]*pow(x[0],par[1]);
}

void Minuit()
{
     ROOT::R::TRInterface &r=ROOT::R::TRInterface::Instance();//Creating R Instance
     
     r<<"rfun<-function(x,a,b){a*x^b}";//creating the R function
     
     rfun=new ROOT::R::TRFunctionImport("rfun");//importing our function from R
     TVirtualFitter::SetDefaultFitter("Minuit");     
     ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit");//setting fitter
     
     
     TCanvas *c1 = new TCanvas("c1","the rfun fit canvas",500,400);
     TH1D *histo = new TH1D("hfitfun","Fitting Data from R",200,1.0,50.0);

     //filling the histogram
     for(Double_t i=1.0;i<50.0;i=i+0.1)//similar to x <- seq(1,50, 0.1)
     {
         histo->Fill((Double_t)(*rfun)(i,1e-3,3));//a=1e-3, b=3 (data generation to fit the function)
     }

      // Creates a Root function x[1:50] nparameters=2
     TF1 *func = new TF1("fitfun",fitfun,1,50,2);
  
     // Sets initial values and parameter names
     func->SetParameters(0.8e-2,2);//a=0.8e-2, b=2
     func->SetParNames("a","b");
     
     histo->Fit(func,"E");//fit the function to the histogram data
     
     //Plotting the results
     histo->Draw();     
}
