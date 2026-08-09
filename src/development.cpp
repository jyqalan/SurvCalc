// const char* time_stamp = "$Date: 2008-08-14 16:56:06 +1200 (Thu, 14 Aug 2008) $\n";
// const char* development_cpp_id = "$Id: development.cpp 234 2008-08-14 04:56:06Z fud $\n";

#include "development.h"

//############################## FUNCTION DEBUGGING ##############################
#if defined(_D0_)
void DEBUG0(const std::string& function_name){
 cerr << "In function " << function_name << '\n';}
#define DEBUG1(fn) ;
#define DEBUG2(fn) ;
#elif defined(_D1_)
void DEBUG0(const std::string& function_name){
 cerr << "In function " << function_name << '\n';}
void DEBUG1(const std::string& function_name){
 cerr << "In function " << function_name << '\n';}
#define DEBUG2(fn) ;
#elif defined(_D2_)
void DEBUG0(const std::string& function_name){
 cerr << "In function " << function_name << '\n';}
void DEBUG1(const std::string& function_name){
 cerr << "In function " << function_name << '\n';}
void DEBUG2(const std::string& function_name){
 cerr << "In function " << function_name << '\n';}
#else
#define DEBUG0(fn) ;
#define DEBUG1(fn) ;
#define DEBUG2(fn) ;
#endif
// At the start of each function, put either DEBUG0(), DEBUG1(), or DEBUG2(),
//  passing it the function name in quotes.
// Use DEBUG0 for major 'strategic' functions,
//     DEBUG1 for more minor functions,
//     DEBUG2 for functions which are called very frequently.
// For debugging, compile the .C files with -D_D0_, -D_D1_, or -D_D2_.

//############################## RANDOM NUMBER GENERATION ##############################
// Just a couple of auxiliary functions. They work off the Betadiff generators
// so RNG seeds are treated in the same way (which see betadiff.h).
// Watch out, syntax is not the same as S.

double rnorm(double mean, double stdev, long seed) {
  DEBUG2("rnorm");
  dvector rand(1,1);
  rand.fill_randn(seed);
  return (mean + stdev*rand[1]);
}

double rlognorm(double mean, double stdev, long seed){
  // mean and stdev on linear scale
  // converted to mu and sigma on log scale
  DEBUG2("rlognorm");
  double mu, sigma;
  sigma = sqrt(log(1+(stdev/mean)*(stdev/mean)));
  mu = log(mean)-sigma*sigma*0.5;
  dvector rand(1,1);
  rand.fill_randn(seed);
  return (exp(mu + sigma*rand[1]));
}

//############################## VARIANCE/COVARIANCE ESTIMATION ##############################
dmatrix sample_covariance(dmatrix &x){
  // return the variance-covariance matrix of this columnar dataset
  DEBUG2("sample_covariance");
  int vars = x.colsize();
  int n = x.rowsize();
  dmatrix result(1,vars,1,vars);
  dvector means(1,vars);
  for (int i=1; i<=vars; i++){
    double sx=0;
    for (int k=1; k<=n; k++){
      sx += x[k][i];
    }
    means[i] = sx/n;
    double sxx=0;
    for (int k=1; k<=n; k++){
      sxx += pow(x[k][i]-means[i],2);
    }
    double var = sxx / (n-1);
    result[i][i]=var;
    for (int j=1; j<i; j++){
      double sxy=0;
      for (int k=1; k<=n; k++){
        sxy += (x[k][i]-means[i]) * (x[k][j]-means[j]);
      }
      double cov = (sxy/(n-1));
      result[i][j]=cov;
      result[j][i]=cov;
    }
  }
  return result;
}

