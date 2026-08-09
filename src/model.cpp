// const char* time_stamp = "$Date: 2008/08/19 23:48:18 $\n";
// const char* model_cpp_id = "$Id: model.cpp,v 1.3 2008/08/19 23:48:18 fud Exp $\n";

#include "development.h"
#include "sample.h"
#include "license.txt"
#include "modification.txt"
#include "GetPot/GetPot.hpp"
#include "survey.h"

#ifndef VERSION_H_
  #include "version.h"
#endif

#ifdef _CONSOLE
#include <windows.h>
#endif


int main(int argc, char* argv[])
{
  DEBUG0("main");
  // Enable calcuation of elapsed time
  time_t start_time;
  start_time = time(NULL);
  // Enable calculation of CPU time used (UNIX systems only).
  #ifndef __MINGW32__
  tms cpu_start, cpu_stop;
  times(&cpu_start);
  #endif
  // Version number
  std::string version = "1.2";
  // Print header information
  std::string SurvCalc_header;
  SurvCalc_header += "SurvCalc (Trawl survey analysis program)\n";
  // Print the command line that was invoked
  SurvCalc_header += "Call: ";
  for (int i=0; i<argc; i++){
  SurvCalc_header += argv[i];
  SurvCalc_header += " ";
  }
  SurvCalc_header += "\nDate: ";
  // Print the date and time
  time_t t;
  t = time(NULL);
  SurvCalc_header += ctime(&t);
  // Print the version label, based on svn_version output, from the auto-generated header file 'version.h'
  // giving the latest modification date and time of source files
  SurvCalc_header += "v" + version + "-" + SOURCE_CONTROL_DATE + " (rev." + itos(SOURCE_CONTROL_REVISION) + ")  ";
  SurvCalc_header += "(c) Copyright 2008-" + string(SOURCE_CONTROL_YEAR) + ", NIWA\n";
  if(USER_MODIFICATION_DESCRIPTION.size()>0)
    SurvCalc_header += "WARNING: This is a modified version of SurvCalc. " + USER_MODIFICATION_DESCRIPTION + '\n';

  #ifdef __MINGW32__
    char name[50];
    unsigned long sz=49;
    SurvCalc_header += "User name: " + string(getenv("USERNAME"));
    SurvCalc_header += "\nMachine name: " + string(getenv("COMPUTERNAME"));
    SurvCalc_header += " (Microsoft Windows";
    std::ofstream supress_stderr_file ("nul");
  #else
    SurvCalc_header += "User name: ";
    char* tt = getenv("LOGNAME");
    if (tt != NULL)
      SurvCalc_header += tt;
    else
    {
      SurvCalc_header += "-----";
      cerr << "Warning: SurvCalc was unable to obtain the user name\n";
    }
    SurvCalc_header += "\nMachine name: ";
    struct utsname names;
    uname(&names);
    SurvCalc_header += static_cast<string>(names.nodename) + " (" + static_cast<string>(names.sysname) + " " + static_cast<string>(names.release) + " " + static_cast<string>(names.machine);
    std::ofstream supress_stderr_file ("/dev/null");
  #endif

  std::ostringstream PID;
  PID << getpid();
  SurvCalc_header += ", PID=" + PID.str() + ")";
  SurvCalc_header += "\n\n";
  cout << SurvCalc_header;
  cout.flush();

  int status = STATUS_OK;
  std::string infilename="input.slc";
  std::string task;

  std::string stratum_input_file="",station_input_file="",catch_input_file="",lgth_input_file="",subcatch_input_file="",station_catch_input_file="";
  std::string stratum_output_file="",station_output_file="",catch_output_file="",lgth_output_file="",subcatch_output_file="",station_catch_output_file="",stratum_catch_output_file="",lw_coeff_output_file="";

  GetPot cl(argc,argv);
  // how many tasks were requested? should be exactly 1
  int tasks_requested = cl.search("-b") + cl.search("-l")+cl.search("-B")+cl.search("-2")+cl.search("-o")+cl.search("-p")+cl.search("-c")+cl.search("-h")+cl.search("-L");
  if (tasks_requested > 1){
    fatal("Must specify exactly one of -h, -L, -b, -l, -B, -o, -p, -c, -2");
  } else if (tasks_requested == 0 || (argc==1 || cl.search("-h") || cl.search("-help"))){
    // print a usage message
    cerr << "Usage: SurvCalc [-L] [-h] [-b] [-l] [-B] [-o] [-p] [-c] [-2] [-f filename] [-s outfile] [-t outfile] [-u outfile] [-v outfile] [-w outfile] [-x outfile] [-y outfile] [-S infile] [-T infile] [-U infile] [-V infile] [-W infile] [-X infile] \n\n";
    cerr << "One task from:\n";
    cerr << "  -b  calculate biomass\n";
    cerr << "  -l  calculate LFs\n";
    cerr << "  -B  calculate biomass and LFs\n";
    cerr << "  -2  do calculations for allocating phase-2 stations in a 2-phase survey\n";
    cerr << "  -o  output only files of stratum,station,catch,length, and/or subcatch data \n";
    cerr << "  -p  output only  a table of length-weight coefficients\n";
    cerr << "  -c  output a file for input to catch-at-age\n";
    cerr << "  -h  display help\n";
    cerr << "  -L  display the SurvCalc end user licence\n";

    cerr << "\nAnd optionally:\n";
    cerr << "  -f  the name of the main input file (the default is input.slc)\n";
    cerr << "  -S  read in a stratum flat file\n";
    cerr << "  -T  read in a station flat file\n";
    cerr << "  -U  read in a catch flat file\n";
    cerr << "  -V  read in a length flat file\n";
    cerr << "  -W  read in a subcatch flat file\n";
    cerr << "  -X  read in a station_catch flat file\n";
    cerr << "  -s  output a stratum flat file\n";
    cerr << "  -t  output a station flat file\n";
    cerr << "  -u  output a catch flat file\n";
    cerr << "  -v  output a length flat file\n";
    cerr << "  -w  output a subcatch flat file\n";
    cerr << "  -x  output a station_catch flat file\n";
    cerr << "  -y  output a stratum_catch flat file\n";

    exit(status);
  }
  if (cl.search("-L")) {
    // This section takes the text of LICENSE, and reformats by inserting carriage returns
    // so that it looks nice on a 80 character width screen
    const int window_width=80;
    std::string formated_license;
    int count_char=0, space_pos=0;
    for(unsigned int i=0;i<strlen(LICENSE);i++){
      formated_license.push_back(LICENSE[i]);
      if(count_char < window_width && LICENSE[i]!='\n'){
        if(isspace(LICENSE[i])) space_pos=formated_license.size();
        count_char++;
      } else if(LICENSE[i]=='\n') {
        count_char= 0;
      } else {
        formated_license[space_pos-1]='\n';
        count_char= formated_license.size()-space_pos;
      }
    }
    cerr << formated_license << endl;
    exit(status);
  }


  if(cl.search("-f")) {
    infilename = cl.next("");
    if (infilename == ""){
      fatal("You must specify the name of the input parameter filename as the argument of -f");}
    cout << "The input parameter filename : " << infilename << "\n\n";
  }


  if(cl.search("-S")){
    stratum_input_file= cl.next("");
	
    if (stratum_input_file == "" || stratum_input_file.substr(0,1)=="-")
      stratum_input_file="stratum.in";
  }
 
  if(cl.search("-T")){
    station_input_file= cl.next("");
    if (station_input_file == "" || station_input_file.substr(0,1)=="-")
      station_input_file="station.in";
  }
 
  if(cl.search("-U")){
    catch_input_file= cl.next("");
    if (catch_input_file == "" || catch_input_file.substr(0,1)=="-")
      catch_input_file="catch.in";
  }
  if(cl.search("-V")){
    lgth_input_file= cl.next("");
    if (lgth_input_file == "" || lgth_input_file.substr(0,1)=="-")
      lgth_input_file="lgth.in";
  }
  if(cl.search("-X")){
    station_catch_input_file= cl.next("");
    if (station_catch_input_file == "" || station_catch_input_file.substr(0,1)=="-")
      station_catch_input_file="station_catch.in";
  }
  if(cl.search("-W")){
    subcatch_input_file= cl.next("");
    if (subcatch_input_file == "" || subcatch_input_file.substr(0,1)=="-")
      subcatch_input_file="subcatch.in";
  }

  if(cl.search("-s")){
    stratum_output_file= cl.next("");
    if (stratum_output_file == "" || stratum_output_file.substr(0,1)=="-")
      stratum_output_file="stratum.out";
  }
  if(cl.search("-t")){
    station_output_file= cl.next("");
    if (station_output_file == "" || station_output_file.substr(0,1)=="-")
      station_output_file="station.out";
  }
  if(cl.search("-u")){
    catch_output_file= cl.next("");
    if (catch_output_file == "" || catch_output_file.substr(0,1)=="-")
      catch_output_file="catch.out";
  }
  if(cl.search("-v")){
    lgth_output_file= cl.next("");
    if (lgth_output_file == "" || lgth_output_file.substr(0,1)=="-")
      lgth_output_file="lgth.out";
  }
  if(cl.search("-x")){
    station_catch_output_file= cl.next("");
    if (station_catch_output_file == "" || station_catch_output_file.substr(0,1)=="-")
      station_catch_output_file="station_catch.out";
  }
  if(cl.search("-y")){
    stratum_catch_output_file= cl.next("");
    if (stratum_catch_output_file == "" || stratum_catch_output_file.substr(0,1)=="-")
      stratum_catch_output_file="stratum_catch.out";
  }
  if(cl.search("-w")){
    subcatch_output_file= cl.next("");
    if (subcatch_output_file == "" || subcatch_output_file.substr(0,1)=="-")
      subcatch_output_file="subcatch.out";
  }

  /*
  if(cl.search("-p")){
    lw_coeff_output_file= cl.next("");
    if (lw_coeff_output_file == "" || lw_coeff_output_file.substr(0,1)=="-")
      lw_coeff_output_file="lw_coeff.out";
  }
    */
  if (cl.search("-b")) {
    task = "calc_biomass";
    if(!((stratum_input_file=="" && station_input_file=="" && catch_input_file == "" && lgth_input_file=="" && station_catch_input_file=="") ||
       (stratum_input_file!="" && station_input_file!="" && station_catch_input_file =="" && (catch_input_file != "" || lgth_input_file != "")) ||
	   (stratum_input_file!="" && station_input_file=="" && station_catch_input_file !="" && catch_input_file == "" &&  lgth_input_file == ""))) 
		fatal("For argument -b, you must define arguments -S -T -U -V, or -S -T -U, or -S -T -V, or - S -X, or none of them"); 
    if(subcatch_output_file !="")
    fatal("Argument -b must not be used with -w");
  } else if (cl.search("-l")) {
    task = "calc_LFs";
    if(!((stratum_input_file=="" && station_input_file==""  && catch_input_file == "" && lgth_input_file == "" && station_catch_input_file=="") ||
      (stratum_input_file!="" && station_input_file!="" && station_catch_input_file =="" && lgth_input_file != "" ) ||
      (stratum_input_file!="" && station_input_file=="" && station_catch_input_file !="" && lgth_input_file != "" )))
      fatal("For argument -l, you must define arguments -S -T -V, or -S -T -U -V, or -S -X -V, or none of them.");
    if(subcatch_output_file !="")
    fatal("Argument -l must not be used with -w");
  } else if (cl.search("-B")) {
    task = "calc_biomass_and_LFs";
    if(!((stratum_input_file=="" && station_input_file==""  && catch_input_file == "" && lgth_input_file == "" && station_catch_input_file=="") ||
      (stratum_input_file!="" && station_input_file!="" && station_catch_input_file =="" && lgth_input_file != "" ) ||
      (stratum_input_file!="" && station_input_file=="" && station_catch_input_file !="" && lgth_input_file != "" )))
      fatal("For argument -B, you must define arguments -S -T -V, or -S -T -U -V, or -S -X -V, or none of them.");
    if(subcatch_output_file !="")
    fatal("Argument -B must not be used with -w");
  } else if (cl.search("-o")) {
    task = "output_t_table";
    if(stratum_input_file!="" || station_input_file!="" || catch_input_file != "" || lgth_input_file != "" || station_catch_input_file != "" ||subcatch_input_file != "")
      fatal("Argument -o must not be used with any of arguments -S,-T,-U, -V, -W,and -X");
    if(stratum_output_file=="" && station_output_file=="" && catch_output_file == "" && lgth_output_file == "" && subcatch_output_file =="")
      fatal("For argument -o, you must define at least one of arguments -s,-t,-u,-v, and -w");
    if(stratum_catch_output_file != "" || station_catch_output_file != "")
      fatal("For argument -o, you must not define -x or -y");
  } else if (cl.search("-p")) {
    task = "output_lw_coeff";
    if(stratum_input_file!="" || station_input_file!="" || catch_input_file != "" || lgth_input_file != "" || subcatch_input_file != "" || station_catch_input_file != "" ||
       stratum_output_file!="" || station_output_file!="" || catch_output_file != "" || lgth_output_file != "" || subcatch_output_file != "" || station_catch_output_file != "" || stratum_catch_output_file != "")
      fatal("Argument -p must not be used with any of arguments -S,-T,-U,-V,-W,-X, -s,-t,-u, -v,-w,-x, and -y.");
  } else if (cl.search("-c")) {
    task = "output_for_catch_at_age";
    if(!((stratum_input_file=="" && station_input_file==""  && catch_input_file == "" && lgth_input_file == "" && station_catch_input_file=="" && subcatch_input_file=="") ||
      (stratum_input_file!="" && station_input_file!="" && station_catch_input_file =="" && lgth_input_file != "" ) ||
    (stratum_input_file!="" && station_input_file!="" && station_catch_input_file =="" && lgth_input_file != "" && subcatch_input_file !="") ||
      (stratum_input_file!="" && station_input_file=="" && station_catch_input_file !="" && lgth_input_file != "" && subcatch_input_file=="" )))
      fatal("For argument -c, you must define arguments -S -T -V, or -S -T -U -V, or -S -T -U -V -W or -S -X -V, or none of them.");
    if(stratum_output_file!="" || station_output_file!="" || catch_output_file != "" || lgth_output_file != "" || station_catch_output_file != "" || stratum_catch_output_file != "" ||subcatch_output_file !="")
      fatal("Argument -c must not be used with any of arguments -s,-t,-u, -v, -w, -x, and -y.");
  } else if (cl.search("-2")) {
    task = "phase_2_calc";
    if(!((stratum_input_file=="" && station_input_file=="" && catch_input_file == "" && lgth_input_file=="" && station_catch_input_file=="") ||
       (stratum_input_file!="" && station_input_file!="" && station_catch_input_file =="" && (catch_input_file != "" || lgth_input_file != "")) ||
       (stratum_input_file!="" && station_input_file=="" && station_catch_input_file !="" && catch_input_file == "" )))
        fatal("For argument -2, you must define arguments -S -T -U -V, or -S -T -U, or -S -T -V, or - S -W, or -S -W -V, or none of them");
      if(stratum_output_file!="" || station_output_file!="" || catch_output_file != "" || lgth_output_file != "" || subcatch_output_file != "" ||station_catch_output_file != ""  || stratum_catch_output_file != "")
        fatal("Argument -2 must not be used with any of arguments -s,-t,-u, -v, -w, -x, and -y.");
  }
  if((station_output_file!="" || catch_output_file != "") && (station_catch_output_file!="" ))
    fatal("You must not define arguments -t or -u, and -x at the same time.");
  if(stratum_output_file!="" && stratum_catch_output_file!="" )
    fatal("You must not define arguments -s and -y at the same time.");
  if(station_catch_input_file!="" && catch_output_file!="" )
    fatal("You must not define both arguments -x and -u at the same time (You've provided catch_kg_km2 but asked for catch_kg).");

  Survey survey(task,infilename,stratum_input_file,station_input_file,catch_input_file,lgth_input_file,subcatch_input_file,station_catch_input_file,
              stratum_output_file,station_output_file,catch_output_file,lgth_output_file,subcatch_output_file,station_catch_output_file,stratum_catch_output_file,cl.search("-z"));

  survey.do_input();
  survey.do_calculation();
  survey.do_output();
  cout << endl;

  double elapsed_time = static_cast<double>(time(NULL)-start_time);
  elapsed_time /= 3600.0;
  int P = (int) floor(log10(elapsed_time))+4;
  cerr << "Total elapsed time: " << std::setprecision(P) << elapsed_time << (elapsed_time==1?" hour":" hours") << ".\n";


}


