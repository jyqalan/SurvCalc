// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* development_h_id = "$Id: development.h 2237 2008-09-08 00:33:48Z fud $\n";

#if !defined(DEVELOPMENT)
#define DEVELOPMENT


//############################### INCLUDES ##############################
#include "betadiff/betadiff.h"
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <list>
#include <iomanip>
#include <algorithm>
#include <cctype>


#ifdef __MINGW32__
#include <process.h>
#ifndef _CONSOLE
  extern "C" {
  char *getlogin (void);
  int w32_gethostname(char* name, size_t size);
  }
#endif
#else
  #include <sys/utsname.h>
  #include <sys/times.h>
  #include <unistd.h>
#endif

#include <time.h>
#include <sstream>
using namespace std;


//############################## CONSTANTS ##############################
#define PI 3.14159265358979
#define S_NULL ""   //NULL for string
#define D_NULL -99999 //NULL for double
#define MIN180  10800000.0  /*180 degrees *1000*/
#define MINx1000(X) ((((X/10000)*60)+(X-((X/10000)*10000))/100.0)*1000.0)
#define HALFPI 11.0/7.0

//########################### RETURN VALUES FOR SEABIRD ######################
#define STATUS_FATAL_ERROR      11
// a Betadiff error is 12 - see the Betadiff and ADOL-C code
#define STATUS_OK       0

//############################## ERROR HANDLING ##############################
static void fatal(const std::string& problem){
 cerr << "\nError: " << problem << '\n';
 exit(STATUS_FATAL_ERROR);
}
static void warning(const std::string& problem){
 cerr << "\nWARNING: " << problem << endl;
 //cout << "\nWARNING: " << problem << endl;
}

//############################## FUNCTION DEBUGGING ##############################
#if defined(_D0_)
void DEBUG0(const std::string& function_name);
#define DEBUG1(fn) ;
#define DEBUG2(fn) ;
#elif defined(_D1_)
void DEBUG0(const std::string& function_name);
void DEBUG1(const std::string& function_name);
#define DEBUG2(fn) ;
#elif defined(_D2_)
void DEBUG0(const std::string& function_name);
void DEBUG1(const std::string& function_name);
void DEBUG2(const std::string& function_name);
#else
#define DEBUG0(fn) ;
#define DEBUG1(fn) ;
#define DEBUG2(fn) ;
#endif

//############################## FUNCTIONS TO SUPPLEMENT THE STL ##############################
template<class C>
void concatenate(std::vector<C>& v1,const std::vector<C>& v2){
  v1.insert(v1.end(),v2.begin(),v2.end());
}

template<class C,class D>
int in(const std::list<C>& l,const D& d){
  C c(d);
  return(std::find(l.begin(),l.end(),c) != l.end());
}

template<class C,class D>
int in(const std::map<C,D>& m,const C& c){
  return(m.find(c) != m.end());
}

static int in(const std::string& s, const std::string& t){
  return (s.find(t)!=string::npos);
}

template<class C>
int in(const std::vector<C>& v,const C& c){
  for (int i=0; i<v.size(); i++){
    if (v[i]==c) return 1;}
  return 0;
}

template<class C,class D>
int pos(const std::vector<C>& v,const D& d){
  for (int i=0; i<v.size(); i++){
    if (v[i]==d) return i;}
  return -1;
}

template<class C>
C max(const std::vector<C>& v){
  C c = v[0];
  if (v.size()>1){
    for (int i=1; i<v.size(); i++){
      c = (c < v[i]) ? v[i] : c;
    }
  }
  return c;
}

static std::string itos(int i){
  char temp[10];
  sprintf(temp,"%d",i);
  return std::string(temp);
}

static std::string dtos(double d){
  char temp[10];
  sprintf(temp,"%g",d);
  return std::string(temp);
}

static int stoi(const std::string& s, std::string error_msg=std::string("no default"), std::string error_msg2=std::string("no default")){
  int val;
  istringstream s_stream((s+" ").c_str());
  s_stream >> val;
  if (!(s_stream.good())){
        if (error_msg=="no default"){
      fatal("stoi: Cannot convert value " + s + " to integer");
    } else {
          fatal(error_msg);
    }
  }
  std::string dummy;
  s_stream >> dummy;
  if (s_stream.good()){
        if (error_msg=="no default"){
      fatal("stoi: Two or more entries in " + s);
    } else {
          fatal(error_msg2);
    }
  }
  return val;
}

static int stoi(char* c, std::string error_msg=std::string("no default"), std::string error_msg2=std::string("no default")){
  return stoi(std::string(c),error_msg, error_msg2);
}



static double stod(const std::string& s, std::string error_msg=std::string("no default"), std::string error_msg2=std::string("no default")){
  double val;
  istringstream s_stream((s+" ").c_str());
  s_stream >> val;
  if (!(s_stream.good())){
        if (error_msg=="no default"){
      fatal("stod: Cannot convert value " + s + " to double");
    } else {
          fatal(error_msg);
    }
  }
  std::string dummy;
  s_stream >> dummy;
  if (s_stream.good()){
        if (error_msg=="no default"){
      fatal("stod: Two or more entries in " + s);
    } else {
          fatal(error_msg2);
    }
  }
  return val;
}

static double stod(char* c, std::string error_msg=std::string("no default"), std::string error_msg2=std::string("no default")){
  return stod(std::string(c),error_msg,error_msg2);
}






template<class C>
static vector<C> unique(const vector<C>& v) {
  std::vector<C> temp;
  int counter=0;
  for(unsigned int i=0; i<v.size(); i++) {
    if(temp.size()>0) {
      for(unsigned int j=0; j<temp.size(); j++) if(temp[j]==v[i]) counter++;
    }
    if(counter==0) temp.push_back(v[i]);
    else counter=0;
  }
  return(temp);
}

template<class C>
static int find(const vector<C>& v, C val) {
  for(int i=0; i<v.size(); i++) {
    if(v[i]==val)
    return i;
  }
  return(-1);
}

template<class C>
std::vector<C> intersection(const std::vector<C>& v1, const std::vector<C>& v2){
  std::vector<C> result;
  for (int i=0; i<v1.size(); i++){
    if (in(v2,v1[i])){
      result.push_back(v1[i]);
    }
  }
  return result;
}

static double min(const vector<double>& v) {
  if(v.size()>0) {
    double result=v[0];
    for(unsigned int i=0; i<v.size(); i++) if(v[i]<result) result=v[i];
    return(result);
  } else fatal("Unable to determine the minimum of a null vector");
  return(-1);
}


static int min(const vector<int>& v) {
  if(v.size()>0) {
    int result=v[0];
    for(unsigned int i=0; i<v.size(); i++) if(v[i]<result) result=v[i];
    return(result);
  } else fatal("Unable to determine the minimum of a null vector");
  return(-1);
}

static double max(const vector<double>& v) {
  if(v.size()>0) {
    double result=v[0];
    for(unsigned int i=0; i<v.size(); i++) if(v[i]>result) result=v[i];
    return(result);
  } else fatal("Unable to determine the maximum of a null vector");
  return(-1);
}

static int max(const vector<int>& v) {
  if(v.size()>0) {
    int result=v[0];
    for(unsigned int i=0; i<v.size(); i++) if(v[i]>result) result=v[i];
    return(result);
  } else fatal("Unable to determine the maximum of a null vector");
  return(-1);
}

static double sum(const vector<double>& v) {
  if(v.size()>0) {
    double result=0;
    for(unsigned int i=0; i<v.size(); i++) result+=v[i];
    return(result);
  } else fatal("Unable to determine the sum of a null vector");
  return(-1);
}

static int sum(const vector<int>& v) {
  if(v.size()>0) {
    int result=0;
    for(unsigned int i=0; i<v.size(); i++) result+=v[i];
    return(result);
  } else fatal("Unable to determine the sum of a null vector");
  return(-1);
}

static double mean(const std::vector<double> V){
  double sum = 0;
  for (unsigned i=0; i<V.size(); i++){
    sum += V[i];
  }
  return sum / V.size();
}


template<class C,class D>
ostream& operator<<(ostream& out, const std::map<C,D>& m){
  out << '\n';
  typedef typename std::map<C,D>::const_iterator MAP_IT;
  for (MAP_IT i=m.begin(); i!=m.end(); ++i){
    out << "  " << i->first << '\t' << i->second << '\n';}
  return out;
}

template<class D>
ostream& operator<<(ostream& out, const std::map<std::string,D>& m){
  // if the key is a string, pad it with spaces
  out << '\n';
  typedef typename std::map<std::string,D>::const_iterator MAP_IT;
  for (MAP_IT i=m.begin(); i!=m.end(); ++i){
    out << "  ";
    out.setf(ios::left,ios::adjustfield);
    out << setw(30);
    out << i->first.c_str();
    out << '\t' << i->second << '\n';}
  return out;
}

static ostream& operator<<(ostream& out, const std::map<std::string,std::string>& m){
  // if the key is a string, pad it with spaces
  // if the value is a string, remove leading whitespace
  out << '\n';
  typedef std::map<std::string,std::string>::const_iterator MAP_IT;
  for (MAP_IT i=m.begin(); i!=m.end(); ++i){
    out << "  ";
    out.setf(ios::left,ios::adjustfield);
    out << setw(30);
    out << i->first.c_str();
    std::string value = i->second;
    if (value.find_first_of(" \t") != string::npos){
      value = value.substr(value.find_first_not_of(" \t"));}
    out << '\t' << value << '\n';}
  return out;
}

template<class D>
ostream& operator<<(ostream& out, const std::vector<D> v){
  for (int i=0; i<v.size(); i++){
    out << v[i] << ' ';}
  return out;
}

template<class D>
ostream& operator<<(ostream& out, const std::vector<std::vector<D> > v){
  for (int i=0; i<v.size(); i++){
    for (int j=0; j<v[i].size(); j++){
      out << v[i][j] << ' ';}
    out << '\n';
  }
  return out;
}

template<class D>
void print_column(const std::vector<D> v, ostream& out){
  for (int i=0; i<v.size(); i++){
    out << v[i] << '\n';}
}

static std::string date_of(const std::string& cvs_id){
  // grabs the date out of a CVS $Id field
  istringstream i(cvs_id.c_str());
  std::string id, name, version, date;
  i >> id >> name >> version >> date;
  return date;
}

static std::string time_of(const std::string& cvs_id){
  // grabs the time out of a CVS $Id field
  istringstream i(cvs_id.c_str());
  std::string id, name, version, date, time;
  i >> id >> name >> version >> date >> time;
  return time;
}

static void most_recent_timestamp(const vector<string> all_ids, std::string& most_recent_date, std::string& most_recent_time){
  // all_ids -> vector with cvs_id for each file in the program. This vector is read through and the most recent
  // date and time are returned in the variables most_recent_date, most_recent_time
  most_recent_date = date_of(all_ids[0]);
  most_recent_time = time_of(all_ids[0]);
  string current_date;
  string current_time;
  for (unsigned i=1; i<=all_ids.size()-1; i++){
    current_date = date_of(all_ids[i]);
    current_time = time_of(all_ids[i]);
    if (current_date > most_recent_date){
      most_recent_date = current_date;
      most_recent_time = current_time;
    }
    if (current_date == most_recent_date && current_time > most_recent_time){
      most_recent_time = current_time;
    }
  }
}

static std::vector<std::string> string_to_string_vector(const std::string& s){
    std::vector<std::string> result;
    istringstream arguments(s.c_str());
    std::string word;
    while (!(arguments.fail())){
      arguments >> word;
      if (!(arguments.fail())){
                result.push_back(word);
      }
    }
    return result;
}
static std::string string_vector_to_string(const std::vector<std::string>& s){
    std::string result;
  for(unsigned i=0;i<s.size();i++)
    result+=s[i]+" ";
    return result;
}
static double Round(double n, int precision) {
  DEBUG2("Round");
  int sign= n<0? -1:1;
  double new_n=sign*floor(fabs(n)*pow(10,(double)precision)+0.5)/pow(10,(double)precision);
  return(new_n);
}

static int Number_width(double n,std::string type="sig_fig", int precision=6){
  DEBUG2("Number_width");
  //Figure out the width of a number based on sig.fig or dec.place
  int sign= n<0? -1:1;
  //number of digits before the demical point
  int number_width= (int)floor(log10(fabs(n)<1? fabs(n)+1:fabs(n)))+1;
  if(type=="sig_fig"){
    if(precision<=0) fatal("In Function Number_width():precision must be positive for type ='sig_fig'");
    if(number_width < precision) {
      if(n == int(n))
        number_width=precision;
      else if(fabs(n)<1)
        number_width=precision+2; //0.
      else
        number_width=precision+1; //.
    }
  } else if(type=="dec_place"){
    if (precision>0) number_width+=precision+1;
  } else {
    fatal("In Function Number_width():type must be either 'sig_fig' or 'dec_place'");
  }
  if(sign==-1) number_width+=1;
  return number_width;
}


//A wrapper class to output a double to the stream with specified precision
class FMT_double{
private:
  double n;
  std::string type;
  int precision;
public:
  FMT_double(double _n,std::string _type="dec_place",int _precision=2){n=_n; type=_type; precision=_precision;};
  friend ostream& operator<<(ostream& output, const FMT_double& fmt){
                if(fmt.type=="sig_fig"){
                  output<<setprecision(fmt.precision)<<fmt.n;
                } else if(fmt.type=="dec_place"){
                  output<<Round(fmt.n,fmt.precision);
                }
                return output;
  };
  int width() {
    int number_width = (int)fabs(n)>0.0?floor(log10(fabs(n)))+(fabs(n)>1?1:0):1;
    if(number_width>0){
      if(type=="sig_fig"){
        if(number_width < precision) {
          number_width= n == int(n)? precision:precision+1;
        }
      } else if(type=="dec_place"){
        if (precision>0) number_width+=precision+1;
      }
    } else {
      if(type=="sig_fig"){
        number_width=-number_width+precision+1; //.
      } else if(type=="dec_place"){
        if (precision>0) number_width=precision+1;
      }
    }
    if(n<0) number_width+=1;
    return number_width;
  }
};




//############################## END OF DEVELOPMENT.h ##############################
#endif
