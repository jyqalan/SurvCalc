// const char* time_stamp = "$Date: 2008/08/14 04:53:06 $\n";
// const char* parameter_set_cpp_id = "$Id: parameter_set.cpp,v 1.3 2008/08/14 04:53:06 fud Exp $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "parameter_set.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
void decompose(const std::string &parname, std::string &command, std::string &label, std::string &subcommand){
  // extracts the command, label and subcommand out of parname - if not present, return ""
  if (!in(parname,".")){
    command=parname;
    label="";
    subcommand="";
  } else if (!in(parname,"[")){
    command = parname.substr(0,parname.find("."));
    label="";
    subcommand = parname.substr(parname.find(".")+1);
  } else {
    command = parname.substr(0,parname.find("["));
    label = parname.substr(parname.find("[")+1,parname.find("]")-parname.find("[")-1);
    subcommand = parname.substr(parname.find(".")+1);
  }
}

std::string decompose_into_words(const std::string &parname){
  std::string command, label, subcommand, result;
  decompose(parname,command,label,subcommand);
  if (subcommand==""){
    result = "command '@" + command + "'";
  } else if (label==""){
    result = "subcommand '" + subcommand + "' of command '@" + command + "'";
  } else {
    result = "subcommand '" + subcommand + "' of command '@" + command + "' with label '" + label + "'";
  }
  return result;
}

int isws(const std::string &s){
  return (s.find_first_not_of(" \t\n")==std::string::npos);
}

void trim_comment(std::string& line){
  std::string::size_type pos = line.find('#');
  if (pos==0){
    line = "\n";
  } else if (pos != std::string::npos){
    line = line.substr(0,pos);
  }
}

void Parameter_set::empty(){
  DEBUG2("Parameter_set::empty");
  strings = std::map<std::string,std::string>();
  doubles = std::map<std::string,double>();
  vectors = std::map<std::string,dvector >();
  command_counts = std::map<std::string,int>();
}

int Parameter_set::present(const std::string& s){
  // Returns 0 if parameter s is not in the Parameter_set, or a positive value
  // indicating the type of s if it is:
  // 1 = double
  // 2 = vector
  // 4 = string
  // Marks the parameter as accessed.
  DEBUG2("Parameter_set::present "+s);
  parameters_accessed[s] = 1;
  if (in(doubles,s)) return 1;
  if (in(vectors,s)) return 2;
  if (in(strings,s)) return 4;
  return 0;
}

void Parameter_set::read_file(const std::string& filename){
  DEBUG0("Parameter_set::read_file");
  empty();
  // read through line by line, discarding comment lines,
  // making keys and inserting into 'strings' and 'parameters_accessed'
  ifstream in_file(filename.c_str());
  if (!in_file) fatal("Could not open file " + filename);
  std::string linebuf,command;
  istringstream *linestr;
  std::string command_line, label, subcommand, arguments, par_name, command_type;
  int can_have_subcommands;
  int length_data_counter=0;
  int age_length_data_counter=0;
  // read to the start of the first command block
  while (1){
    getline(in_file,linebuf);
    trim_comment(linebuf);
    linebuf += '\n';
    if (in(linebuf,"}") && !in(linebuf,"{")) fatal("Found a loose } with no matching { in the input parameter file.");
    while (in(linebuf,"{")){
      while (!in(linebuf,"}")){
        getline(in_file,linebuf);
        trim_comment(linebuf);
        linebuf += '\n';
        if (in(linebuf,"{")) fatal("Found a second { within a {} comment block in the input parameter file.");
        if (in_file.fail()) fatal("Failed to find the end of a comment block in the input parameter file. Check that there is a '}' character to end the comment block.");
      }
      getline(in_file,linebuf);
      trim_comment(linebuf);
      linebuf += '\n';
    }
    if (linebuf[0]=='@') break;
    if (linebuf[0]!=' ' && linebuf[0]!='\t' && linebuf[0]!='\n' && linebuf[0]!='\r') fatal("Found the following line: " + linebuf + " at the start of the input parameter file: was expecting a command block instead.");
    if (in_file.fail()) fatal("Failed to find any command blocks in the input parameter file.");
  }
  while(1){
    linebuf.erase(0,1);  // get rid of the initial '@'
    linestr = new istringstream(linebuf.c_str());
    *linestr >> command;
    command_type = dict.check(command);
    if (in(command_counts,command)){ // this command is already in command_counts
      command_counts[command]++;
    } else { // it's the first time we have seen this command
      command_counts.insert(make_pair(command,1));
    }
    getline(*linestr,command_line);  // this may be either the label or the arguments
    delete linestr;
    linestr = new istringstream((command_line+" ").c_str());
    *linestr >> label;
    if (command_type=="label"){
      if (!(linestr->good())) fatal("Command '@" + command + "' must always have a label.");
      can_have_subcommands=1;
    } else if (command_type=="nolabel"){
      if (linestr->good()) fatal("Command '@" + command + "' may never have a label.");
      label="";
      can_have_subcommands=1;
    } else if (command_type=="autonumber"){
      if (linestr->good()) fatal("Command '@" + command + "' may never have a label.");
      label = itos(command_counts[command]);
      can_have_subcommands=1;
    } else if (command_type=="labelifrepeated"){
      if (command_counts[command]>1){
        if (!(linestr->good()) || in(get_command_labels(command),std::string(""))) fatal("Command '@" + command + "' must have a label if you use it more than once.");
      }
      if (!(linestr->good())) label="";
        can_have_subcommands=1;
    } else if (command_type=="error"){
      can_have_subcommands=1;
    } else {
      // this command has arguments and is not the start of a command block
      if (in(strings,command)) fatal("Have found command '@" + command + "' appears twice in the input parameter file.");
      if (isws(command_line)) fatal("No arguments found for the command '@" + command + "' in the input parameter file.");
      strings.insert(make_pair(command,command_line));
      parameters_accessed[command] = 0;
      can_have_subcommands=0;
    }
    delete linestr;
    getline(in_file,linebuf);
    trim_comment(linebuf);
    linebuf += '\n';
    if (in(linebuf,"}") && !in(linebuf,"{")) fatal("Found a loose } with no matching { in the input parameter file.");
    while (in(linebuf,"{")){
      while (!in(linebuf,"}")){
        getline(in_file,linebuf);
        trim_comment(linebuf);
        linebuf += '\n';
        if (in(linebuf,"{")) fatal("Found a second { within a {} comment block in the input parameter file.");
        if (in_file.fail()) fatal("Failed to find the end of a comment block in the input parameter file. Check that there is a '}' character to end the comment block.");
      }
      getline(in_file,linebuf);
      trim_comment(linebuf);
      linebuf += '\n';
    }
    if (in_file.fail()){
      if (dict.bad_command) fatal("A bad command has been found in the input parameter file.");
      return;
    }
    while(linebuf[0]!='@'){ // until start of next command block
      if (can_have_subcommands){
        linestr = new istringstream(linebuf.c_str());
        *linestr >> subcommand;
        if (linestr->good()){
//          if(!(command=="length_data" || command=="age_data")) {
            dict.check(command,subcommand);
            getline(*linestr,arguments);
            if (isws(arguments)) fatal("No arguments for subcommand '" + subcommand + "' of command '@" + command + "' in the input parameter file.");
            par_name = command;
            if (label != "") par_name += "[" + label + "]";
            par_name += "." + subcommand;
            if (in(strings,par_name))
              fatal("Subcommand '" + subcommand + "' of command '@" + command + "' appears twice in the input parameter file.");
            strings.insert(make_pair(par_name,arguments));
            parameters_accessed[par_name] = 0;
/*          } else if(command=="length_data") {
            getline(*linestr,arguments);
            if (isws(arguments)) fatal("No arguments for subcommand '" + subcommand + "' of command '@" + command + "' in the input parameter file.");
            length_data_counter++;
            if(length_data_counter>100000) fatal("The maximum number of lines of length_data are 100000. This number has been exceeded in the input parameter file.");
            std::ostringstream length_data_counter_prefix;
            length_data_counter_prefix << std::setw(6) << std::setfill('0') << length_data_counter;
            par_name = command + "[" + length_data_counter_prefix.str() + "].data";
            strings.insert(make_pair(par_name,subcommand+arguments));
            parameters_accessed[par_name] = 0;
          } else if(command=="age_length_data") {
            getline(*linestr,arguments);
            if (isws(arguments)) fatal("No arguments for subcommand '" + subcommand + "' of command '@" + command + "' in the input parameter file.");
            age_length_data_counter++;
            if(age_length_data_counter>100000) fatal("The maximum number of lines of age_length_data are 100000. This number has been exceeded in the input parameter file.");
            std::ostringstream age_length_data_counter_prefix;
            age_length_data_counter_prefix << std::setw(6) << std::setfill('0') << age_length_data_counter;
            par_name = command + "[" + age_length_data_counter_prefix.str() + "].data";
            strings.insert(make_pair(par_name,subcommand+arguments));
            parameters_accessed[par_name] = 0;
          }
*/      }
        delete linestr;
      } else if (!can_have_subcommands){
        if (linebuf[0]!=' ' && linebuf[0]!='\t' && linebuf[0]!='\n' && linebuf[0]!='\r')
          fatal("Found the following line: " + linebuf + ", following command '@" + command + "' which may never have subcommands.");
      }
      getline(in_file,linebuf);
      trim_comment(linebuf);
      linebuf += '\n';
      if (in(linebuf,"}") && !in(linebuf,"{")) fatal("Found a loose } with no matching { in the input parameter file.");
      while (in(linebuf,"{")){
        while (!in(linebuf,"}")){
          getline(in_file,linebuf);
          trim_comment(linebuf);
          linebuf += '\n';
          if (in(linebuf,"{")) fatal("Found a second { within a {} comment block in the input parameter file.");
          if (in_file.fail()) fatal("Failed to find the end of a comment block in the input parameter file. Check that there is a '}' character to end the comment block.");
        }
        getline(in_file,linebuf);
        trim_comment(linebuf);
        linebuf += '\n';
      }
      if (in_file.fail()){
        if (dict.bad_command) fatal("A bad command has been found in the input parameter file.");
        return;
      }
    }
  }
}

int Parameter_set::get_int(const std::string& s, const int default_val){
  DEBUG2("Parameter_set::get_int");
  int component = present(s);
  if (component==4){
    istringstream arguments((strings[s]+" ").c_str());
    std::string result;
    arguments >> result;
    if (!(arguments.fail())){
      std::string dummy;
      arguments >> dummy;
      if (!(arguments.fail())){
        fatal("For " + decompose_into_words(s) + ", you have supplied more than one value.");
      }
    }
    std::transform(result.begin(),result.end(),result.begin(),(int(*)(int)) tolower);
    int value;
    if (result == "true" || result == "t"){
      value = 1;
    } else if (result == "false" || result == "f"){
      value = 0;
    } else {
          value = stoi(result,"For " + decompose_into_words(s) + ", you did not supply a True/False argument.","For " + decompose_into_words(s) + ", you supplied a number with a decimal point in it: use an integer instead.");
    }
    return value;
  }
  // can't find the parameter
  if (default_val != -999){
    return default_val;
  } else {
    // no default argument supplied
    fatal("SurvCalc needs " + decompose_into_words(s) + ", but does not find it in the input parameter file.");
  }
  return -1; //dummy return value - never required
}

int Parameter_set::get_bool(const std::string& comm, const int default_val){
DEBUG2("Parameter_set::get_bool");
  int component = present(comm);
  if (component==4){
    istringstream arguments((strings[comm]+" ").c_str());
    std::string result;
    arguments >> result;

    if (!(arguments.fail())){
      std::string dummy;
      arguments >> dummy;
      if (!(arguments.fail())){
        fatal("For " + decompose_into_words(comm) + " you have supplied more than one value.");}
    }
    std::transform(result.begin(),result.end(),result.begin(),(int(*)(int)) tolower);
    int value;
    if (result == "true" || result == "t" || result == "1")
      return value = 1;
    else if (result == "false" || result == "f" || result == "0")
      return value = 0;
    else
      fatal("For " + decompose_into_words(comm) + " you did not supply a True/False argument.");
  } else if (component==0){
    if (default_val == 0 || default_val == 1)
      return default_val;
    else fatal("For " + decompose_into_words(comm) + " a default value of 0 or 1 should be supplied.");
  } else
    fatal("There is a problem with the " + decompose_into_words(comm) + " which should take a True/False argument.");
  return -1; //dummy return value - never required
}

double Parameter_set::get_double(const std::string& s, const double default_val){
  DEBUG2("Parameter_set::get_double");
  int component = present(s);
  if (component==4){
    return stod(strings[s],"For " + decompose_into_words(s) + ", you did not supply a valid number.","For " + decompose_into_words(s) + ", you supplied two or more values.");
  }
  // can't find the parameter
  if (default_val != -999){
    return default_val;
  } else {
    // no default argument supplied
    fatal("SurvCalc needs " + decompose_into_words(s) + ", but does not find it in the input parameter file.");
  }
  return -1.0; //dummy return value - never required
}

std::vector<double> Parameter_set::get_double_vector(const std::string& s, const std::vector<double>& default_val){
  DEBUG2("Parameter_set::get_double_vector");
  int component = present(s);
  if (component==4){
    std::string temp;
    int count = 0;
    istringstream arguments((strings[s]+" ").c_str());
    arguments >> temp;
    while (!(arguments.fail())){
      count++;
      arguments >> temp;
    }
    vector<double> result(count, 1);
    istringstream arguments2((strings[s]+" ").c_str());
    for (int i=0; i<count; i++){
      arguments2 >> temp;
      result[i] = stod(temp,"catch_at_age found an invalid argument for " + decompose_into_words(s));
    }
    return result;
  }
  // can't find the parameter
  if (default_val.size()!=1 || default_val[0]!=-999){
    return default_val;
  } else {
    // no default argument supplied
    fatal("catch_at_age needs " + decompose_into_words(s) + ", but does not find it in the input parameter file.");
  }
  return std::vector<double>(1,-1.0); //dummy return value - never required
}



std::string Parameter_set::get_string(const std::string& s, const std::string& default_val){
  DEBUG2("Parameter_set::get_string");
  int component = present(s);
  if (component==4){
    std::string word, dummy;
    istringstream arguments((strings[s]+" ").c_str());
    arguments >> word;
    if (!(arguments.fail())){
          arguments >> dummy;
          if (!(arguments.fail())){
                fatal("For " + decompose_into_words(s) + ", you supplied two or more values.");
          }
        }
    return word;
  }
  // can't find the parameter
  if (default_val != "don't use default"){
    return default_val;
  } else {
    // no default argument supplied
    fatal("SurvCalc needs " + decompose_into_words(s) + ", but does not find it in the input parameter file.");
  }
  return "-1"; //dummy return value - never required
}

std::vector<std::string> Parameter_set::get_string_vector(const std::string& s, const std::vector<std::string>& default_val){
  DEBUG2("Parameter_set::get_string_vector");
  int component = present(s);
  if (component==4){
    std::vector<std::string> result;
    istringstream arguments((strings[s]+" ").c_str());
    std::string word;
    while (!(arguments.fail())){
      arguments >> word;
      if (!(arguments.fail())){
                result.push_back(word);
      }
    }
    return result;
  }
  if (default_val != std::vector<std::string>(1,"don't use default")){
    return default_val;
  } else {
    // no default argument supplied
    fatal("SurvCalc needs " + decompose_into_words(s) + ", but does not find it in the input parameter file.");
  }
  return std::vector<string>(1,"-1"); //dummy return value - never required
}

int Parameter_set::get_command_count(const std::string& s){
  // Returns the number of times command 's' occurs in the Parameter_set.
  // So if the Parameter_set contains "fishery[1].name", "fishery[1].area",
  //                                  "fishery[2].name", "fishery[2].area",
  // get_command_count("fishery")==2  (not 4 - "name" and "area" are subcommands)
  DEBUG2("Parameter_set::get_command_count");
  if (in(command_counts,s)){
    return command_counts[s];
  } else {
    return 0;
  }
  return -1; //dummy return value - never required
}

std::vector<std::string> Parameter_set::get_command_labels(const std::string& s){
  // Returns the list of labels with which command 's' occurs in the Parameter_set.
  // So if the Parameter_set contains "fishery[trawl].area", "fishery[longline].area", etc...,
  // the result of get_command_labels("fishery") is a vector containing "trawl", "longline".
  DEBUG2("Parameter_set::get_command_labels");
  std::vector<std::string> result;
  std::string this_parameter, this_command, this_label, this_subcommand;
  typedef std::map<std::string,std::string>::iterator MAP_IT;
  for (MAP_IT i=strings.begin(); i!=strings.end(); ++i){
    this_parameter = i->first;
    decompose(this_parameter, this_command, this_label, this_subcommand);
    if (this_command == s){
      if (!in(result,this_label)){
        result.push_back(this_label);
      }
    }
  }
  return result;
}

void Parameter_set::put_int(std::string name, int i){
  // insert 'name' = i into the 'strings' compartment of the Parameter_set,
  // update command_counts if necessary
  DEBUG2("Parameter_set::put_int");
  update_command_counts(name);
  std::string val = itos(i);
  strings[name] = val;
}

void Parameter_set::put_double(std::string name, double d){
  // insert 'name' = d into the 'strings' compartment of the Parameter_set,
  // update command_counts if necessary
  DEBUG2("Parameter_set::put_constant");
  update_command_counts(name);
  std::string val = dtos(d);
  strings[name] = val;
}

void Parameter_set::put_double_vector(std::string name, const dvector& v){
  // check that the indices of v start from 1,
  // insert 'name' = v into the 'strings' compartment of the Parameter_set,
  // update command_counts if necessary
  DEBUG2("Parameter_set::put_constant_vector");
  if (v.indexmin() != 1){
    fatal("Only use Parameter_set::put_constant_vector on vectors whose indices start from 1");}
  update_command_counts(name);
  std::string val="";
  for (int i=1; i<=v.size(); i++){
    val += (dtos(v[i]) + " ");}
  strings[name] = val;
}

void Parameter_set::put_string(std::string name, const std::string& s){
  // insert 'name' = s into the 'strings' compartment of the Parameter_set,
  // update command_counts if necessary
  DEBUG2("Parameter_set::put_string");
  update_command_counts(name);
  strings[name] = s;
}

void Parameter_set::put_string_vector(std::string name, std::vector<std::string>& vs){
  // insert 'name' = vs into the 'strings' compartment of the Parameter_set,
  // update command_counts if necessary
  DEBUG2("Parameter_set::put_string_vector");
  update_command_counts(name);
  std::string val="";
  for (unsigned int i=0; i<vs.size(); i++){
    val += (vs[i] + " ");}
  strings[name] = val;
}

void Parameter_set::update_command_counts(std::string& name){
  // Called by all the put_ functions, when they are adding parameter 'name',
  // to update the command counts if necessary.
  // Also converts names of form command[1].subcommand into just command.subcommand.
  DEBUG2("Parameter_set::update_command_counts");
  std::string command, label, subcommand;
  decompose(name, command, label, subcommand);
  if (command_counts[command]==0){
    command_counts[command] = 1;
  } else {
    // what labels does this command have so far?
    // e.g. if we already have fishery[trawl] and fishery[longline],
    // then adding fishery[trawl].area  would require no change to command_counts,
    // but  adding fishery[setnet].area would require command_counts["fishery"] to be incremented,
    // Incidentally, we don't check for fishery[].area, or fishery[6].area when we only
    // have fishery[1].area and fishery[2].area, both of which should probably be errors.
    std::string this_parameter, this_command, this_label, this_subcommand;
    typedef std::map<std::string,std::string>::iterator MAP_IT;
    for (MAP_IT i=strings.begin(); i!=strings.end(); ++i){
      this_parameter = i->first;
      decompose(this_parameter, this_command, this_label, this_subcommand);
      if (this_command == command && this_label == label) return;
    }
    command_counts[command]++; // if we are still here, we have not had this command
                               // with this label before
  }
}

std::vector<std::string> Parameter_set::get_unused(ostream& out){
  // Return a list of the names of the parameters which were read in from the
  //  file and never accessed.
  DEBUG0("Parameter_set::get_unused");
  std::vector<std::string> result;
  typedef std::map<std::string,int>::iterator MAP_IT;
  for (MAP_IT i=parameters_accessed.begin(); i!=parameters_accessed.end(); ++i){
    if (i->second == 0){
      result.push_back(i->first);}
  }
  return result;
}

void Parameter_set::print(ostream& out){
  DEBUG1("Parameter_set::print");
  out << "Parameters: " << strings;
//  out << "Stored as doubles: " << doubles;
//  out << "Stored as vectors: " << vectors;
  out << '\n';
}

Parameter_set::Parameter_set(){
  DEBUG2("Parameter_set::Parameter_set");
  empty();
}

Parameter_set::~Parameter_set(){
  DEBUG1("~Parameter_set");
}

//############################## END OF PARAMETER SET.cpp ##############################
