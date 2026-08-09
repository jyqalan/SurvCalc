// const char* time_stamp = "$Date: 2008-08-20 11:48:19 +1200 (Wed, 20 Aug 2008) $\n";
// const char* dictionary_cpp_id = "$Id: dictionary.cpp 240 2008-08-19 23:48:19Z fud $\n";



//############################## DICTIONARY OF SurvCalc COMMANDS #####################################
#include "development.h"
#include "dictionary.h"



//////////////////////////////////////////////////////////////////////////////////////////////////

bool Table_compare::operator()(const std::string& x, const std::string& y) const{

  int match=0;

  unsigned int xp=0, yp=0;

  if (in(x,"!") && !in(y,"!")){ // only x contains a !

  // check a regular-expression match, with ! a wildcard that can replace any number

  // if it matches then return 'not less than' (so not(x<y), not(y<x) -> x==y)

    while (xp != x.length()){

      if (x.substr(xp,1)=="!"){

        if (!in("0123456789",y.substr(yp,1))) break;

        while (in("0123456789",y.substr(yp,1))){

          yp++;

          if (yp == y.length()) break;

        }

      } else if (x.substr(xp,1)==y.substr(yp,1)){

        yp++;

      } else break;

      xp++;

      if (xp==x.length() && yp==y.length()){

        match=1;

        break;

      } else if (xp==x.length() || yp==y.length()) break;

    }

  } else if (in(y,"!") && !in(x,"!")){ // only y contains a !

    while (yp != y.length()){

      if (y.substr(yp,1)=="!"){

        if (!in("0123456789",x.substr(xp,1))) break;

        while (in("0123456789",x.substr(xp,1))){

          xp++;

          if (xp == x.length()) break;

        }

      } else if (y.substr(yp,1)==x.substr(xp,1)){

        xp++;

      } else break;

      yp++;

      if (yp==y.length() && xp==x.length()){

        match=1;

        break;

      } else if (yp==y.length() || xp==x.length()) break;

    }

  }

  if (match) return 0;

  return (x<y); // if we get this far there is no wildcard match - just use ordinary lexicographic comparison

}



// here type = switch/int/double/double_vector/string/string_vector

// use ! in the subcommand name to indicate 'any number'

#define set_command_with_argument(name,type) key=#name; val=#type; table.insert(make_pair(key,val));

#define set_command_with_nolabel(name) key=#name; val="nolabel"; table.insert(make_pair(key,val));

#define set_command_with_label(name) key=#name; val="label"; table.insert(make_pair(key,val));

#define set_command_with_labelifrepeated(name) key=#name; val="labelifrepeated"; table.insert(make_pair(key,val));

#define set_subcommand(command,subcommand,type) key=#command "->" #subcommand; val=#type; table.insert(make_pair(key,val));

#define set_obsolete_command(name) key=#name "@OBSOLETE"; val=""; table.insert(make_pair(key,val));

#define set_obsolete_subcommand(command,subcommand) key=#command "->" #subcommand "@OBSOLETE"; val=""; table.insert(make_pair(key,val));





std::string Dictionary::check(std::string &command, int nofatal){

// checks whether the command is valid, issues an error message if not,

// returns one of the following: "error" "nolabel" "label" "autonumber" or a data type "switch", "int", "constant" etc

  DEBUG2("Dictionary::check(command)");

  std::string key = command;

  if (table.find(key) != table.end()){ // ie. key is in table

    return table[key];

  } else if (table.find(key+"@OBSOLETE") != table.end()){

    cerr << "Command '@" + command + "' in the parameter file is obsolete and not supported in the current version of SurvCalc\n";

    bad_command = 1;

    return "error";

  } else {

    if (!nofatal){

      cerr << "Command '@" + command + "' in the parameter file is not recognised by SurvCalc. Is it misspelt?\n";

      bad_command = 1;

    }

    return "error";

  }

}



std::string Dictionary::check(std::string &command, std::string &subcommand, int nofatal){

// checks whether the subcommand is valid, issues an error message if not,

// returns "error" or a data type "switch", "int", "constant" etc

  DEBUG2("Dictionary::check(subcommand)");

  std::string key = command+"->"+subcommand;

  if (table.find(key) != table.end()){ // ie. key is in table

    return table[key];

  } else if (table.find(key+"@OBSOLETE") != table.end()){

    cerr << "Subcommand '" + subcommand + "' of command '@" + command + "' in the parameter file is obsolete and not supported in the current version of SurvCalc\n";

    bad_command = 1;

    return "error";

  } else {

    if (!nofatal){

      cerr << "Subcommand '" + subcommand + "' of command '@" + command + "' in the parameter file is not recognised by SurvCalc. Is it misspelt or under the wrong command?\n";

      bad_command = 1;

    }

    return "error";

  }

}



Dictionary::Dictionary(){

	DEBUG0("Dictionary::Dictionary");
	bad_command = 0;
	std::string key, val;

	set_command_with_argument(survey_type,string)
	set_command_with_argument(trips,string_vector)
	set_command_with_argument(LF_scaling,string)
	set_command_with_labelifrepeated(where)
	set_command_with_labelifrepeated(preferences)
	set_command_with_labelifrepeated(species)
	set_command_with_labelifrepeated(change_strata)
	set_command_with_labelifrepeated(reassign_strata)
	set_command_with_labelifrepeated(new_strata)
	set_command_with_labelifrepeated(change_stratum_area)
	set_command_with_labelifrepeated(areal_availability)
	set_command_with_labelifrepeated(population_area)
	set_command_with_labelifrepeated(vulnerability)
	set_command_with_labelifrepeated(vertical_availability)
	set_command_with_labelifrepeated(area_fished)
	set_command_with_labelifrepeated(lw_coeff)
	set_command_with_labelifrepeated(sub_populations)
	set_command_with_labelifrepeated(constant_speed)
	set_command_with_labelifrepeated(constant_doorspread)
	set_command_with_labelifrepeated(phase_2)

	set_command_with_nolabel(projected_cvs)
	set_command_with_nolabel(output_tables)
	set_command_with_nolabel(output_precision)
	set_command_with_nolabel(input_from_database)
	


	set_subcommand(species,codes,string_vector)
	set_subcommand(change_strata,from,string_vector)
	set_subcommand(change_strata,to,string_vector)
	set_subcommand(reassign_strata,stations,double_vector)
	set_subcommand(reassign_strata,strata,string_vector)
	set_subcommand(new_strata,strata,string_vector)
	set_subcommand(new_strata,areas,double_vector)

	set_subcommand(change_stratum_area,strata,string_vector)
	set_subcommand(change_stratum_area,new_areas,double_vector)

	set_subcommand(areal_availability,default_value,double)
	set_subcommand(areal_availability,other_strata,string_vector)
	set_subcommand(areal_availability,other_values,double_vector)

	set_subcommand(population_area,other_strata,string_vector)
	set_subcommand(population_area,other_values,double_vector)

	set_subcommand(vulnerability,default_value,double)
	set_subcommand(vulnerability,other_stations,double_vector)
	set_subcommand(vulnerability,other_values,double_vector)

	set_subcommand(vertical_availability,default_value,double)
	set_subcommand(vertical_availability,other_stations,double_vector)
	set_subcommand(vertical_availability,other_values,double_vector)

	set_subcommand(area_fished,default_value,double)
	set_subcommand(area_fished,other_stations,double_vector)
	set_subcommand(area_fished,other_values,double_vector)

	set_subcommand(lw_coeff,a,double)
	set_subcommand(lw_coeff,b,double)
	set_subcommand(lw_coeff,c,double)
	set_subcommand(lw_coeff,a_male,double)
	set_subcommand(lw_coeff,b_male,double)
	set_subcommand(lw_coeff,c_male,double)
	set_subcommand(lw_coeff,a_female,double)
	set_subcommand(lw_coeff,b_female,double)
	set_subcommand(lw_coeff,c_female,double)
	set_subcommand(lw_coeff,a_unsexed,double)
	set_subcommand(lw_coeff,b_unsexed,double)
	set_subcommand(lw_coeff,c_unsexed,double)

	set_subcommand(sub_populations,sexes,string_vector);
	set_subcommand(sub_populations,labels,string_vector);
	set_subcommand(sub_populations,Lmin,double_vector);
	set_subcommand(sub_populations,Lmax,double_vector);

	set_subcommand(preferences,distance_towed,string_vector);
	set_subcommand(preferences,width_swept,string_vector);
	set_subcommand(preferences,catch_weight,string_vector);

	set_subcommand(constant_speed,value,double);
	set_subcommand(constant_doorspread,value,double);

	set_subcommand(output_tables,sub_biomass_by_stratum,switch)
	set_subcommand(output_tables,LFs_by_stratum,switch)
	set_subcommand(output_tables,LFs_by_station,switch)
	set_subcommand(output_tables,Number_measured,switch)
	set_subcommand(output_tables,LF_totals,switch)
	set_subcommand(output_tables,biomass_by_species,switch)
	set_subcommand(output_tables,biomass_by_species_stratum,switch)
	set_subcommand(output_tables,biomass_by_species_trip,switch)

	set_subcommand(output_precision,quantity,string_vector)
	set_subcommand(output_precision,type,string)
	set_subcommand(output_precision,precision,double_vector)

	set_subcommand(input_from_database,database,string)
	set_subcommand(input_from_database,hostname,string)
	set_subcommand(input_from_database,user,string)
	set_subcommand(input_from_database,database_name,string)
	set_subcommand(input_from_database,schema,string)
	set_subcommand(input_from_database,t_station_columns,string_vector) // only for station table
	set_subcommand(input_from_database,t_station_table,string)	
	set_subcommand(input_from_database,t_stratum_table,string)			
	set_subcommand(input_from_database,t_catch_table,string)	
	set_subcommand(input_from_database,t_lgth_table,string)	
	set_subcommand(input_from_database,t_subcatch_table,string)	

	set_subcommand(where,t_stratum,string_vector)
	set_subcommand(where,t_station,string_vector)
	set_subcommand(where,t_catch,string_vector)
	set_subcommand(where,t_lgth,string_vector)
	set_subcommand(where,t_subcatch,string_vector)


	set_subcommand(phase_2,n_stations,int)
	set_subcommand(phase_2,algorithm,string)
	
	set_subcommand(projected_cvs,strata,string_vector)
	set_subcommand(projected_cvs,extra_stations,double_vector)

}



#undef set_command_with_argument

#undef set_command_with_nolabel

#undef set_command_with_label

#undef set_subcommand

#undef set_obsolete_command

#undef set_obsolete_subcommand



//############################## END OF DICTIONARY.cpp #####################################

