// const char* time_stamp = "$Date: 2008/08/14 04:53:06 $\n";
// const char* dictionary_h_id = "$Id: dictionary.h,v 1.1 2008/08/14 04:53:06 fud Exp $\n";

#if !defined(DICTIONARY)
#define DICTIONARY

#//############################## DICTIONARY OF SurvCalc COMMANDS #####################################

class Table_compare{
// the comparison operator for the map in class Dictionary
public:
  bool operator()(const std::string& x, const std::string& y) const;
};

class Dictionary{
  /*
  This class contains a large data table, with one entry per SurvCalc input command.
  Commands come in three types:
   with_argument: The command must have an argument and never has subcommands. Provide the type of argument it takes.
   with_nolabel: The command never has an argument or label; it would usually have subcommands.
   with_label: The command never has an argument, must have a label, and would usually have subcommands. Provide the legitimate subcommands and the type of each. Use a wildcard ! for subcommands whose names can contain 'any number'
   data lines that can contain anything.
  Commands and subcommands can also be marked as obsolete.
  */
public:
  int bad_command;
  map<std::string,std::string,Table_compare> table;
  // Each table key is constructed as follows:
  // [command], or [command]->[subcommand] (subcommand names can include ! as a wildcard for any number, or ! for data lines),
  // in either case can be suffixed with @OBSOLETE.
  //
  // Each corresponding entry is a string taking one of the following values:
  //  "switch", "int", "constant", "vector", "string", "vector_of_strings" (ie. the command or subcommand takes this kind of argument)
  //  or "nolabel" "label" "data" (see main comment above)
  std::string check(std::string &command, int nofatal=0);
  std::string check(std::string &command, std::string &subcommand, int nofatal=0);
  // Is this (sub)command valid? If so, what is its entry in the table ie. "label" "nolabel" or a data type (see above)
  Dictionary();
};

//############################## END OF DICTIONARY.h #####################################
#endif
