// const char* time_stamp = "$Date: 2008/08/14 04:53:06 $\n";
// const char* sample_cpp_id = "$Id: sample.cpp,v 1.1 2008/08/14 04:53:06 fud Exp $\n";

//############################### INCLUDES ##############################
#include "development.h"
#include "sample.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
Uniform Sample::U=Uniform();

void Sample::initialise(long int RNG_seed){
  DEBUG0("Sample::initialise");
  double newseed=double(abs(RNG_seed+1))/RNG_LONGMAX;
  Random::Set(newseed);
}

vector<unsigned> Sample::get_sample(unsigned number) {
  DEBUG2("Sample::get_sample");
  vector<unsigned> numbers;
  numbers.resize(number);
  for(unsigned i=0; i<number; i++){
    numbers[i] = (unsigned)(floor(U.Next()*number));
  }
  
  return numbers;
}

void Sample::get_sample(vector<vector<double> >& n) {
  DEBUG2("Sample::get_sample");
  vector<double> number;
  for(unsigned i=0; i<n.size(); i++) {
    for(unsigned j=0; j<n[i].size(); j++) {
      number.push_back(n[i][j]);
    }
  }
  number=get_sample(number);
  int counter=0;
  for(unsigned i=0; i<n.size(); i++) {
    for(unsigned j=0; j<n[i].size(); j++) {
      n[i][j]=number[counter];
      counter++;
    }
  }
}


vector<double> Sample::get_sample(vector<double> n) {
  DEBUG2("Sample::get_sample");
  double total=sum(n);
  unsigned counter=0;
  vector<double> probs=n, result;
  probs[0]=probs[0]/total;
  for(unsigned i=1; i<probs.size(); i++) {probs[i]=probs[i]/total+probs[i-1];}
  result.resize(probs.size());
  for(unsigned i=0; i<total; i++){
    counter=0;
    double select = U.Next();
    while(counter<probs.size()) {
      if(probs[counter]>select) {
        result[counter]++;
        break;
      }
      counter++;
    }
  }
  return result;
}

Sample::Sample(){
  DEBUG2("Sample::Sample");
}

Sample::~Sample(){
  DEBUG1("~Sample");
}

//############################## END OF SAMPLE.cpp ##############################
