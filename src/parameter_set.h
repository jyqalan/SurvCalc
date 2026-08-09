// const char* time_stamp = "$Date: 2008/08/14 04:53:06 $\n";
// const char* parameter_set_id = "$Id: parameter_set.h,v 1.1 2008/08/14 04:53:06 fud Exp $\n";

#if !defined(PARAMETER_SET)
#define PARAMETER_SET

//############################## PARAMETER SETS ##############################

#include "dictionary.h"

// Forward declarations
void decompose(const std::string &parname, std::string &command, std::string &label, std::string &subcommand);
std::string decompose_into_words(const std::string &parname);
int isws(const std::string &s);

//////////////////////////////////////////////////////////////////////////////////
class Parameter_set{
public:
  void read_file(const std::string& filename);
  int get_bool(const std::string& s, const int default_val = -999);
  int get_int(const std::string& s, const int default_val = -999);
  double get_double(const std::string& s, const double default_val = -999);
  std::string get_string(const std::string& s, const std::string& default_val = "don't use default");
  std::vector<double> get_double_vector(const std::string& s, const std::vector<double>& default_val = std::vector<double>(1,-999));
  std::vector<std::string> get_string_vector(const std::string& s, const std::vector<std::string>& default_val = std::vector<std::string>(1,"don't use default"));
  int present(const std::string& s);
  int get_command_count(const std::string& s);
  std::vector<std::string> get_command_labels(const std::string& s);
  void put_int(std::string name, int i);
  void put_double(std::string name, double d);
  void put_string(std::string name, const std::string& s);
  void put_double_vector(std::string name, const dvector& v);
  void put_string_vector(std::string name, std::vector<std::string>& vs);
  std::map<std::string,int> parameters_accessed;
  std::vector<std::string> get_unused(ostream& out = cout);
  void print(ostream& out = cout);
  void empty();
  Parameter_set(const Parameter_set& p){fatal("Never copy-construct a Parameter_set");}
  Parameter_set();
  ~Parameter_set();
  Dictionary& dictionary() {return dict;}
private:
  std::map<std::string,std::string> strings;
  std::map<std::string,double> doubles;
  std::map<std::string,dvector> vectors;
  std::map<std::string,int> command_counts;
  void update_command_counts(std::string& name);
  Dictionary dict;
};

//############################## END OF PARAMETER_SET.h ##############################
#endif
