//Code based on Rcpp benchmarks
//http://gallery.rcpp.org/articles/run_sum-benchmark/
#include<TRInterface.h>



std::vector<float> run_sum(std::vector<float> x, int n) {
    
    int sz = x.size();

    std::vector<float> res(sz);
    
    // loop through the vector calling std::accumulate
    for(int i = 0; i < (sz-n+1); i++){
        res[i+n-1] = std::accumulate(x.begin()+i, x.end()-sz+n+i, 0.0);
    }
    
    // pad the first n-1 elements with NA
    std::fill(res.begin(), res.end()-sz+n-1, NA_REAL);
    
	return res;
}

void Benchmark()
{
 ROOT::R::TRInterface &r=ROOT::R::TRInterface::Instance(); 
 r<<"require(rbenchmark)";
// r.SetVerbose(1);
 r["run_sum"]=run_sum;
 r<<"x <- rnorm(100000)";
 r<<"output<-benchmark( run_sum(x, 50), run_sum(x, 100),run_sum(x, 150),run_sum(x, 200),run_sum(x,250),order = NULL)[,1:4]";
 r<<"print(output)";
 
}
