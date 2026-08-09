// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* datamanager_cpp_id = "$Id: datamanager.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "datamanager.h"


DataManager::DataManager(std::string _task,Parameter_set& p,std::string stratum_input_file,std::string station_input_file, std::string catch_input_file, std::string lgth_input_file,std::string subcatch_input_file,std::string station_catch_input_file,
             std::string stratum_output_file,std::string station_output_file, std::string catch_output_file, std::string lgth_output_file, std::string subcatch_output_file,std::string station_catch_output_file,std::string stratum_catch_output_file,bool _z_flag):
            t_stratum(this,stratum_input_file,stratum_output_file),
            t_station(this,station_input_file,station_output_file),
            t_catch(this,catch_input_file,catch_output_file),
            t_lgth(this,lgth_input_file,lgth_output_file),
            t_subcatch(this,subcatch_input_file,subcatch_output_file),
            t_station_catch(this,station_catch_input_file,station_catch_output_file),
            t_stratum_catch(this,S_NULL,stratum_catch_output_file),
            t_lw_coeff(this,S_NULL)
{
  DEBUG0("DataManager::DataManager");
  task=_task;

  catch_kg_km2_supplied = station_catch_input_file != "";
  if(task=="calc_biomass_and_LFs"){
    LF_scaling=p.get_string("LF_scaling","numbers_in_population");
    if(LF_scaling!="numbers_in_population")
      fatal("LF_scaling must be 'numbers_in_population' for run-time task calc_biomass_and_LFs ('-B')");
  } else if(task=="calc_LFs"){
    LF_scaling=p.get_string("LF_scaling");
    if(catch_kg_km2_supplied){
      if(LF_scaling!="unscaled" && LF_scaling!="numbers_per_1000" && LF_scaling!="numbers_per_tow")
        fatal("LF_scaling must be 'unscaled','numbers_per_1000','numbers_per_tow',or for run-time task 'calc_LFs' ('-l') with catch rates supplied in input file.");
    } else {
      if(LF_scaling!="unscaled" && LF_scaling!="numbers_per_1000" && LF_scaling!="numbers_per_tow" && LF_scaling!="numbers_per_hour" && LF_scaling!="numbers_per_km2" )
        fatal("LF_scaling must be 'unscaled','numbers_per_1000','numbers_per_tow','numbers_per_hour',or 'numbers_per_km2' for run-time task 'calc_LFs' ('-l')");
    }
  } else {
    LF_scaling="";
    if(p.present("LF_scaling"))
      warning("@LF_scaling will be ignored for run-time task "+task);
  }
  type=p.get_string("survey_type","trawl");
  if(type!="trawl" && type!="pot")
    fatal("survey_type must be either 'trawl' or 'pot'.");

  trips=p.get_string_vector("trips");
  if(trips!=unique(trips))
    fatal("@trips contain replicated trip codes.");
  std::vector<std::string> labels = p.get_command_labels("species");
  if(labels.size()==0)
    fatal("You must define @species command blocks");
  for(int i=0;i<labels.size();i++){
    if(labels[i]==""){
      //if(trips.size()!=1) fatal("Ambiguous @species command.");
    } else if(!in(trips,labels[i])) {
      fatal("The label "+labels[i]+" of @species is not one of the defined trips.");
    }
  }
  for(int i=0;i<trips.size();i++){
    std::string command;
    if(labels.size()==1 && labels[0]=="") {
        command = "species.";
    } else {
      command = "species["+trips[i]+"].";
    }
    std::vector<std::string> species_by_trip=p.get_string_vector(command+"codes");
    if(species_by_trip!=unique(species_by_trip))
      fatal("species["+trips[i]+"].codes contain replicated species codes");
    for(int j=0;j<species_by_trip.size();j++){
      if(!in(species,species_by_trip[j])){
        species.push_back(species_by_trip[j]);
      }
      trip_species_labels.push_back(trips[i]+"_"+species_by_trip[j]);
    }
    trip_species.insert(make_pair(trips[i],species_by_trip));
  }

  output_stratum=stratum_output_file!="";
  output_station=station_output_file!="";
  output_catch=catch_output_file!="";
  output_lgth=lgth_output_file!="";
  output_subcatch=subcatch_output_file!="";
  output_station_catch=station_catch_output_file!="";
  output_stratum_catch=stratum_catch_output_file!="";

  input_mode = stratum_input_file!="" || station_input_file !="" ||catch_input_file !="" || lgth_input_file !=""? "from_file":"";
  z_flag = _z_flag;
}

void DataManager::input(Parameter_set& p,bool catch_weight_calculated,bool catch_weight_recorded,bool sub_populations_calculated){
  DEBUG0("DataManager::input");

  if(task=="calc_LFs" || task=="calc_biomass" || task=="calc_biomass_and_LFs" ||task=="phase_2_calc" || task=="output_t_table"  ||task=="output_for_catch_at_age"){
    t_stratum.input(p);
    if(!catch_kg_km2_supplied)
      t_station.input(p);
    else
      t_station_catch.input(p);
    if(!(catch_kg_km2_supplied ||(task=="calc_LFs" && LF_scaling=="unscaled")))
      t_catch.input(p);
    if(task=="calc_LFs" || task=="calc_biomass_and_LFs" || task=="output_t_table" ||task=="output_for_catch_at_age")
      t_lgth.input(p);
    else if(task=="phase_2_calc" || task=="calc_biomass"){
      if(sub_populations_calculated || ((!catch_kg_km2_supplied) && catch_weight_calculated))
        t_lgth.input(p);
    }
    if(task=="calc_biomass_and_LFs" || task=="output_for_catch_at_age"){
      t_lw_coeff.input(p);
    }
    else if( task=="calc_biomass" ||task=="phase_2_calc"){
      if(sub_populations_calculated || ((!catch_kg_km2_supplied) && catch_weight_calculated))
        t_lw_coeff.input(p);
    } else if(task=="calc_LFs" && LF_scaling!="unscaled"){
      if((!catch_kg_km2_supplied) && catch_weight_calculated){
        t_lw_coeff.input(p);
      }
    }
    if(task=="output_t_table"  ||(task=="output_for_catch_at_age" && catch_weight_recorded && t_lgth.subcatch_defined())){
      t_subcatch.input(p);
    }
  } else if(task=="output_lw_coeff"){
    t_lw_coeff.input(p);
  }
}

void DataManager::output(Parameter_set& p,Output_precision& output_precision){
  DEBUG0("DataManager::output");
  if(task == "output_lw_coeff") {
    t_lw_coeff.output(p,output_precision);
  } else if(task=="calc_LFs" || task=="calc_biomass" || task=="calc_biomass_and_LFs" ||task=="phase_2_calc" || task=="output_t_table"){
    if(output_stratum)
      t_stratum.output(p,output_precision);
    if(output_station)
      t_station.output(p,output_precision);
    if(output_catch)
      t_catch.output(p,output_precision);
    if(output_lgth)
      t_lgth.output(p,output_precision);
    if(output_subcatch)
      t_subcatch.output(p,output_precision);
    if(output_station_catch)
      t_station_catch.output(p,output_precision);
    if(output_stratum_catch)
      t_stratum_catch.output(p,output_precision);
  }
}


void DataManager::modify(Parameter_set& p){
  DEBUG0("DataManager::modify");
  if(task=="calc_LFs" || task=="calc_biomass" || task=="calc_biomass_and_LFs" ||task=="phase_2_calc" || task=="output_t_table"  ||task=="output_for_catch_at_age"){
    if(p.get_command_count("change_strata")>0)
      change_strata(p);
    if(p.get_command_count("change_stratum_area")>0)
      change_stratum_area(p);
    if(p.get_command_count("reassign_strata")>0)
      reassign_strata(p);
  }
}

void DataManager::extend(Parameter_set &p){
  DEBUG0("DataManager::extend");
  if(task=="calc_LFs" || task=="calc_biomass" || task=="calc_biomass_and_LFs" ||task=="phase_2_calc" || task=="output_t_table"  ||task=="output_for_catch_at_age"){
    if(p.get_command_count("areal_availability")>0 || p.get_command_count("population_area")>0){
      if(p.get_command_count("areal_availability")>0 && p.get_command_count("population_area")>0)
        fatal("You must not define both @areal_availability and @population_area.");
      if(p.get_command_count("areal_availability")>0)
        set_areal_availability(p);
      if(p.get_command_count("population_area")>0)
        set_population_area(p);
    } else {
      set_areal_availability(p);    //use default areal_availability
    }
    if(type=="trawl"){
      if(p.get_command_count("vulnerability")>0){
        if(catch_kg_km2_supplied)
          warning("@vulnerability ignored as catch rates have been supplied");
        else
          set_vulnerability(p);
      } else {
        if(!catch_kg_km2_supplied)
          set_vulnerability(p); //use default vulnerability
      }
      if(p.get_command_count("vertical_availability")>0){
        if(catch_kg_km2_supplied)
          warning("@vertical_availability ignored as catch rates have been supplied");
        else
          set_vertical_availability(p);
      }
      else {
        if(!catch_kg_km2_supplied)
          set_vertical_availability(p); //use default vertical_availability
      }
      if(p.get_command_count("area_fished")>0)
        fatal("You must not define area_fished for a survey of type 'trawl'.");
	} else {
      if(p.get_command_count("vulnerability")>0)
        fatal("You must not define vulnerability for a survey of type 'pot'.");
      if(p.get_command_count("vertical_availability")>0)
        fatal("You must not define vertical_availability for a survey of type 'pot'.");
      if(p.get_command_count("area_fished")>0){
        if(catch_kg_km2_supplied)
          warning("@area_fished ignored as catch rates have been supplied");
        else
          set_area_fished(p);
      } else {
        if(!catch_kg_km2_supplied)
          fatal("You must define area_fished for a survey of type 'pot'.");
      }
    }
  }
}

void DataManager::empty(){
  DEBUG0("DataManager::extend");
  t_stratum.empty();
  t_station.empty();
  t_catch.empty();
  t_lgth.empty();
  t_subcatch.empty();
  t_stratum_catch.empty();
  t_station_catch.empty();
}

void DataManager::change_strata(Parameter_set& p){
  DEBUG0("DataManager::change_strata");
  std::vector<std::string> labels=p.get_command_labels("change_strata");
  //cout<<"labels:"<<labels<<" "<<labels.size()<<"\n";
  if(labels.size()==1 && labels[0]==""){
    if(trips.size()>1)
      fatal("You must define @change_strata with a trip_code as the label if there are more than two trips.");
    std::string command = "change_strata.";
    std::vector<std::string> from=p.get_string_vector(command+"from");
    std::vector<std::string> to=p.get_string_vector(command+"to");
    if(from.size()!=to.size())
      fatal("The size of "+command+"from is not equal to that of "+command+"to.");
    for(int j=0;j<from.size();j++){
      t_stratum.modify_stratum(trips[0],from[j],to[j]);
      t_station.modify_stratum(trips[0],from[j],to[j]);
      t_catch.modify_stratum(trips[0],from[j],to[j]);
      t_lgth.modify_stratum(trips[0],from[j],to[j]);
    }
  } else {
    for(int i=0; i<labels.size();i++){
      std::string command = "change_strata["+labels[i]+"].";
      if(!in(trips,labels[i]))
        fatal("The label of "+command+" is not one of the specified trips");
      std::vector<std::string> from=p.get_string_vector(command+"from");
      std::vector<std::string> to=p.get_string_vector(command+"to");
      if(from.size()!=to.size())
        fatal("The size of "+command+"from is not equal to that of "+command+"to.");
      for(int j=0;j<from.size();j++){
        t_stratum.modify_stratum(labels[i],from[j],to[j]);
        t_station.modify_stratum(labels[i],from[j],to[j]);
        t_catch.modify_stratum(labels[i],from[j],to[j]);
        t_lgth.modify_stratum(labels[i],from[j],to[j]);
      }
    }
  }
}

void DataManager::change_stratum_area(Parameter_set& p){
  DEBUG0("DataManager::change_stratum_area");
  std::vector<std::string> labels=p.get_command_labels("change_stratum_area");
  //cout<<"labels:"<<labels<<" "<<labels.size()<<"\n";
  if(labels.size()==1 && labels[0]==""){
    if(trips.size()>1)
      fatal("You must define change_stratum_area with a trip_code as the label if there are more than two trips.");
    std::string command = "change_stratum_area.";
    std::vector<std::string> strata=p.get_string_vector(command+"strata");
    std::vector<double> areas=p.get_double_vector(command+"new_areas");
    if(areas.size()!=strata.size())
      fatal("The size of "+command+"areas is not equal to that of "+command+"strata.");
    for(int j=0;j<strata.size();j++){
      t_stratum.modify_area_km2(trips[0],strata[j],areas[j]);
    }
  } else {
    for(int i=0; i<labels.size();i++){
      std::string command = "change_stratum_area["+labels[i]+"].";
      if(!in(trips,labels[i]))
        fatal("The label of "+command+" is not one of the specified trips");
      std::vector<std::string> strata=p.get_string_vector(command+"strata");
      std::vector<double> areas=p.get_double_vector(command+"new_areas");
      if(areas.size()!=strata.size())
        fatal("The size of "+command+"areas is not equal to that of "+command+"strata.");
      for(int j=0;j<strata.size();j++){
        t_stratum.modify_area_km2(labels[i],strata[j],areas[j]);
      }
    }
  }
}

void DataManager::reassign_strata(Parameter_set& p){
  DEBUG0("DataManager::reassign_strata");
  std::vector<std::string> labels=p.get_command_labels("reassign_strata");
  //cout<<"labels:"<<labels<<" "<<labels.size()<<"\n";
  if(labels.size()==1 && labels[0]==""){
    if(trips.size()>1)
      fatal("You must define reassign_strata with a trip_code as the label if there are more than two trips.");
    std::string command = "reassign_strata.";
    std::vector<std::string> strata=p.get_string_vector(command+"strata");
    std::vector<double> stations=p.get_double_vector(command+"stations");
    if(stations.size()!=strata.size())
      fatal("The size of "+command+"stations is not equal to that of "+command+"strata.");
    for(int j=0;j<strata.size();j++){
      t_station.modify_station_no_stratum(trips[0],(int) stations[j],strata[j]);
      t_catch.modify_station_no_stratum(trips[0],(int) stations[j],strata[j]);
      t_lgth.modify_station_no_stratum(trips[0],(int) stations[j],strata[j]);
    }
  } else {
    for(int i=0; i<labels.size();i++){
      std::string command = "reassign_strata["+labels[i]+"].";
      if(!in(trips,labels[i]))
        fatal("The label of "+command+" is not one of the specified trips");
      std::vector<std::string> strata=p.get_string_vector(command+"strata");
      std::vector<double> stations=p.get_double_vector(command+"stations");
      if(stations.size()!=strata.size())
        fatal("The size of "+command+"stations is not equal to that of "+command+"strata.");
      for(int j=0;j<strata.size();j++){
        t_station.modify_station_no_stratum(labels[i],(int) stations[j],strata[j]);
        t_catch.modify_station_no_stratum(labels[i],(int) stations[j],strata[j]);
        t_lgth.modify_station_no_stratum(labels[i],(int) stations[j],strata[j]);
      }
    }
  }
}

void DataManager::set_areal_availability(Parameter_set &p){
  DEBUG0("DataManager::set_areal_availability");
  std::vector<std::string> labels=p.get_command_labels("areal_availability");
  for(int i=0;i<labels.size();i++){
    if(labels[i]==""){
      if(!(trips.size()==1 && species.size()==1))
        fatal("Ambiguous @areal_availability command.");
    } else if(!in(trip_species_labels,labels[i])) {
      fatal("The label "+labels[i]+" of @areal_availability is ambiguous.");
    }
  }
  for(int i=0;i<trips.size();i++){
    for(int j=0;j<trip_species[trips[i]].size();j++){
      std::string command;
      if(labels.size()==1 && labels[0]==""){
        command = "areal_availability.";
        /*
      } else if(in(labels,trips[i])+in(labels,trip_species[trips[i]][j])+in(labels,trips[i]+"_"+trip_species[trips[i]][j])>1){
        fatal("Ambiguous 'areal_availability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
      } else if (in(labels,trips[i])){
        if(trip_species[trips[i]].size()>1)
          fatal("Ambiguous 'areal_availability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        command = "areal_availability["+trips[i]+"].";
      } else if(in(labels,trip_species[trips[i]][j])){
        for(int k=0;k<trips.size();k++){
          if(k!=i && in(trip_species[trips[k]],trip_species[trips[i]][j]))
            fatal("Ambiguous 'areal_availability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        }
        command = "areal_availability["+trip_species[trips[i]][j]+"].";
      */
      } else if(in(labels,trips[i]+"_"+trip_species[trips[i]][j])){
        command="areal_availability["+trips[i]+"_"+trip_species[trips[i]][j]+"].";
      } else  {
        std::vector<std::string> strata=t_stratum.select_stratum(trips[i]);
        for(int k=0;k<strata.size();k++){
          trip_species_stratum_areal_availability[trips[i]][trip_species[trips[i]][j]][strata[k]]=1;
        }
        continue;
      }
      double default_value=p.get_double(command+"default_value");
      if(default_value<=0)
        fatal(command+"default_value must be positive.");
      std::vector<std::string> strata=t_stratum.select_stratum(trips[i]);
      std::vector<std::string> values;
      std::vector<std::string> other_strata;
      std::vector<double> other_values;
      if(p.present(command+"other_strata") && p.present(command+"other_values")){
        other_strata=p.get_string_vector(command+"other_strata");
        other_values=p.get_double_vector(command+"other_values");
        if(other_strata.size()!=other_values.size())
          fatal(command+"other_strata and "+command+"other_values not of the same size.");
        if(unique(other_strata)!=other_strata)
          fatal("You have defined duplicated strata in "+command+"other_strata");
        for(int k=0;k<other_strata.size();k++){
          if(!in(strata,other_strata[k])){
            fatal(other_strata[k]+" in "+command+"other_strata does not exist in t_stratum table");
          }
        }
        for(int k=0;k<other_values.size();k++){
          if(other_values[k]<=0){
            fatal(dtos(other_values[k])+" in "+command+"other_values must be positive.");
          }
        }
      } else if(p.present(command+"other_strata") || p.present(command+"other_values")){
        fatal("You must define both "+command+"other_strata and "+command+"other_values, or neither of them.");
      }
      for(int k=0;k<strata.size();k++){
        int position = pos(other_strata,strata[k]);
        if(position==-1)
          trip_species_stratum_areal_availability[trips[i]][trip_species[trips[i]][j]][strata[k]]=default_value;
        else
          trip_species_stratum_areal_availability[trips[i]][trip_species[trips[i]][j]][strata[k]]=other_values[position];
      }
    }
  }
}

void DataManager::set_population_area(Parameter_set &p){
  DEBUG0("DataManager::set_population_area");
  std::vector<std::string> labels=p.get_command_labels("population_area");
  for(int i=0;i<labels.size();i++){
    if(labels[i]==""){
      if(!(trips.size()==1 && species.size()==1))
        fatal("Ambiguous @population_area command.");
    } else if(!in(trip_species_labels,labels[i])) {
      fatal("The label "+labels[i]+" of @population_area is ambiguous.");
    }
  }
  for(int i=0;i<trips.size();i++){
    for(int j=0;j<trip_species[trips[i]].size();j++){
      std::string command;
      if(labels.size()==1 && labels[0]==""){
        command = "population_area.";
            /*
      } else if(in(labels,trips[i])+in(labels,trip_species[trips[i]][j])+in(labels,trips[i]+"_"+trip_species[trips[i]][j])>1){
        fatal("Ambiguous 'population_area' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
      }
      else if (in(labels,trips[i])){
        if(trip_species[trips[i]].size()>1)
          fatal("Ambiguous 'population_area' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        command = "population_area["+trips[i]+"].";
      } else if(in(labels,trip_species[trips[i]][j])){
        for(int k=0;k<trips.size();k++){
          if(k!=i && in(trip_species[trips[k]],trip_species[trips[i]][j]))
            fatal("Ambiguous 'population_area' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        }
        command = "population_area["+trip_species[trips[i]][j]+"].";\
      */
      } else if(in(labels,trips[i]+"_"+trip_species[trips[i]][j])){
        command="population_area["+trips[i]+"_"+trip_species[trips[i]][j]+"].";
      } else  {
        std::vector<std::string> strata=t_stratum.select_stratum(trips[i]);
        for(int k=0;k<strata.size();k++){
          std::vector<double> default_value = t_stratum.select_area_km2(trips[i],strata[k]);
          if(default_value.size()!=1)
            fatal("Can not locate area_km2 for trip "+trips[i]+" stratum "+strata[k]);
          trip_species_stratum_population_area[trips[i]][trip_species[trips[i]][j]][strata[k]]=default_value[0];
        }
        continue;
      }
      std::vector<std::string> strata=t_stratum.select_stratum(trips[i]);
      std::vector<std::string> values;
      std::vector<std::string> other_strata;
      std::vector<double> other_values;
      if(p.present(command+"other_strata") && p.present(command+"other_values")){
        other_strata=p.get_string_vector(command+"other_strata");
        other_values=p.get_double_vector(command+"other_values");
        if(other_strata.size()!=other_values.size())
          fatal(command+"other_strata and "+command+"other_values not of the same size.");
        if(unique(other_strata)!=other_strata)
          fatal("You have defined duplicated strata in "+command+"other_strata");
        for(int k=0;k<other_strata.size();k++){
          if(!in(strata,other_strata[k])){
            fatal(other_strata[k]+" in "+command+"other_strata does not exist in t_stratum table");
          }
        }
        for(int k=0;k<other_values.size();k++){
          if(other_values[k]<=0){
            fatal(dtos(other_values[k])+" in "+command+"other_values must be positive.");
          }
        }
      } else if(p.present(command+"other_strata") || p.present(command+"other_values")){
        fatal("You must define both "+command+"other_strata and "+command+"other_values, or neither of them.");
      }
      for(int k=0;k<strata.size();k++){
        int position = pos(other_strata,strata[k]);
        if(position==-1){
          std::vector<double> default_value = t_stratum.select_area_km2(trips[i],strata[k]);
          if(default_value.size()!=1)
            fatal("Can not locate area_km2 for trip "+trips[i]+" stratum "+strata[k]);
          else
              trip_species_stratum_population_area[trips[i]][trip_species[trips[i]][j]][strata[k]]=default_value[0];
        } else {
          trip_species_stratum_population_area[trips[i]][trip_species[trips[i]][j]][strata[k]]=other_values[position];
        }
      }
    }
  }
}

void DataManager::set_vulnerability(Parameter_set &p){
  DEBUG0("DataManager::set_vulnerability");
  std::vector<std::string> labels=p.get_command_labels("vulnerability");
  for(int i=0;i<labels.size();i++){
    if(labels[i]==""){
      if(!(trips.size()==1 && species.size()==1))
        fatal("Ambiguous @vulnerability command.");
    } else if(!in(trip_species_labels,labels[i])) {
      fatal("The label "+labels[i]+" of @vulnerability is ambiguous.");
    }
  }
  for(int i=0;i<trips.size();i++){
    for(int j=0;j<trip_species[trips[i]].size();j++){
      std::string command;
      if(labels.size()==1 && labels[0]==""){
        command = "vulnerability.";
            /*
      } else if(in(labels,trips[i])+in(labels,trip_species[trips[i]][j])+in(labels,trips[i]+"_"+trip_species[trips[i]][j])>1){
        fatal("Ambiguous 'vulnerability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
      } else if (in(labels,trips[i])){
        if(trip_species[trips[i]].size()>1)
          fatal("Ambiguous 'vulnerability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        command = "vulnerability["+trips[i]+"].";
      } else if(in(labels,trip_species[trips[i]][j])){
        for(int k=0;k<trips.size();k++){
          if(k!=i && in(trip_species[trips[k]],trip_species[trips[i]][j]))
            fatal("Ambiguous 'vulnerability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        }
        command = "vulnerability["+trip_species[trips[i]][j]+"].";
      */
      } else if(in(labels,trips[i]+"_"+trip_species[trips[i]][j])){
        command="vulnerability["+trips[i]+"_"+trip_species[trips[i]][j]+"].";
      } else  {
        std::vector<int> station_no=t_station.select_station_no(trips[i]);
        for(int k=0;k<station_no.size();k++){
          trip_species_station_vulnerability[trips[i]][trip_species[trips[i]][j]][station_no[k]]=1;
        }
        continue;
      }
      double default_value=p.get_double(command+"default_value");
      if(default_value<=0)
        fatal(command+"default_value must be positive.");
      std::vector<int> stations=t_station.select_station_no(trips[i]);
      std::vector<std::string> values;
      std::vector<double> other_stations;
      std::vector<double> other_values;
      if(p.present(command+"other_stations") && p.present(command+"other_values")){
        other_stations=p.get_double_vector(command+"other_stations");
        other_values=p.get_double_vector(command+"other_values");
        if(other_stations.size()!=other_values.size())
          fatal(command+"other_stations and "+command+"other_values not of the same size.");
        if(unique(other_stations)!=other_stations)
          fatal("You have defined duplicated stations in "+command+"other_stations");
        for(int k=0;k<other_stations.size();k++){
          if((int)other_stations[k]!=other_stations[k]){
            fatal(dtos(other_stations[k])+" in "+command+"other_stations is not an integer");
          }
          if(!in(stations,(int)other_stations[k])){
            fatal(dtos(other_stations[k])+" in "+command+"other_stations does not exist in t_station table");
          }
        }
        for(int k=0;k<other_values.size();k++){
          if(other_values[k]<=0){
            fatal(dtos(other_values[k])+" in "+command+"other_values must be positive.");
          }
        }
      } else if(p.present(command+"other_stations") || p.present(command+"other_values")){
        fatal("You must define both "+command+"other_stations and "+command+"other_values, or neither of them.");
      }
      for(int k=0;k<stations.size();k++){
        int position = pos(other_stations,stations[k]);
        if(position==-1)
          trip_species_station_vulnerability[trips[i]][trip_species[trips[i]][j]][stations[k]]=default_value;
        else
          trip_species_station_vulnerability[trips[i]][trip_species[trips[i]][j]][stations[k]]=other_values[position];
      }
    }
  }
}

void DataManager::set_vertical_availability(Parameter_set &p){
  DEBUG0("DataManager::set_vertical_availability");
  std::vector<std::string> labels=p.get_command_labels("vertical_availability");
  for(int i=0;i<labels.size();i++){
    if(labels[i]==""){
      if(!(trips.size()==1 && species.size()==1))
        fatal("Ambiguous @vertical_availability command.");
    } else if(!in(trip_species_labels,labels[i])) {
      fatal("The label "+labels[i]+" of @vertical_availability is ambiguous.");
    }
  }

  for(int i=0;i<trips.size();i++){
    for(int j=0;j<trip_species[trips[i]].size();j++){
      std::string command;
      if(labels.size()==1 && labels[0]==""){
        command = "vertical_availability.";
            /*
      } else if(in(labels,trips[i])+in(labels,trip_species[trips[i]][j])+in(labels,trips[i]+"_"+trip_species[trips[i]][j])>1){
        fatal("Ambiguous 'vertical_availability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
      } else if (in(labels,trips[i])){
        if(trip_species[trips[i]].size()>1)
          fatal("Ambiguous 'vertical_availability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        command = "vertical_availability["+trips[i]+"].";
      } else if(in(labels,trip_species[trips[i]][j])){
        for(int k=0;k<trips.size();k++){
          if(k!=i && in(trip_species[trips[k]],trip_species[trips[i]][j]))
            fatal("Ambiguous 'vertical_availability' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        }
        command = "vertical_availability["+trip_species[trips[i]][j]+"].";
               */
      } else if(in(labels,trips[i]+"_"+trip_species[trips[i]][j])){
        command="vertical_availability["+trips[i]+"_"+trip_species[trips[i]][j]+"].";
      } else  {
        std::vector<int> station_no=t_station.select_station_no(trips[i]);
        for(int k=0;k<station_no.size();k++){
          trip_species_station_vertical_availability[trips[i]][trip_species[trips[i]][j]][station_no[k]]=1;
        }
        continue;
      }
      double default_value=p.get_double(command+"default_value");
      if(default_value<=0)
        fatal(command+"default_value must be positive.");
      std::vector<int> stations=t_station.select_station_no(trips[i]);
      std::vector<std::string> values;
      std::vector<double> other_stations;
      std::vector<double> other_values;
      if(p.present(command+"other_stations") && p.present(command+"other_values")){
        other_stations=p.get_double_vector(command+"other_stations");
        other_values=p.get_double_vector(command+"other_values");
        if(other_stations.size()!=other_values.size())
          fatal(command+"other_stations and "+command+"other_values not of the same size.");
        if(unique(other_stations)!=other_stations)
          fatal("You have defined duplicated stations in "+command+"other_stations");
        for(int k=0;k<other_stations.size();k++){
          if((int)other_stations[k]!=other_stations[k]){
            fatal(dtos(other_stations[k])+" in "+command+"other_stations is not an integer");
          }
          if(!in(stations,(int)other_stations[k])){
            fatal(dtos(other_stations[k])+" in "+command+"other_stations does not exist in t_station table");
          }
        }
        for(int k=0;k<other_values.size();k++){
          if(other_values[k]<=0){
            fatal(dtos(other_values[k])+" in "+command+"other_values must be positive.");
          }
        }
      } else if(p.present(command+"other_stations") || p.present(command+"other_values")){
        fatal("You must define both "+command+"other_stations and "+command+"other_values, or neither of them.");
      }
      for(int k=0;k<stations.size();k++){
        int position = pos(other_stations,stations[k]);
        if(position==-1)
          trip_species_station_vertical_availability[trips[i]][trip_species[trips[i]][j]][stations[k]]=default_value;
        else
          trip_species_station_vertical_availability[trips[i]][trip_species[trips[i]][j]][stations[k]]=other_values[position];
      }
    }
  }
}

void DataManager::set_area_fished(Parameter_set &p){
  DEBUG0("DataManager::set_area_fished");
  std::vector<std::string> labels=p.get_command_labels("area_fished");
  for(int i=0;i<labels.size();i++){
    if(labels[i]==""){
      if(!(trips.size()==1 && species.size()==1))
        fatal("Ambiguous @area_fished command.");
    } else if(!in(trip_species_labels,labels[i])) {
      fatal("The label "+labels[i]+" of @area_fished is ambiguous.");
    }
  }
  for(int i=0;i<trips.size();i++){
    for(int j=0;j<trip_species[trips[i]].size();j++){
      std::string command;
      if(labels.size()==1 && labels[0]==""){
        command = "area_fished.";
            /*
      } else if(in(labels,trips[i])+in(labels,trip_species[trips[i]][j])+in(labels,trips[i]+"_"+trip_species[trips[i]][j])>1){
        fatal("Ambiguous 'area_fished' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
      } else if (in(labels,trips[i])){
        if(trip_species[trips[i]].size()>1)
          fatal("Ambiguous 'area_fished' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        command = "area_fished["+trips[i]+"].";
      } else if(in(labels,trip_species[trips[i]][j])){
        for(int k=0;k<trips.size();k++){
          if(k!=i && in(trip_species[trips[k]],trip_species[trips[i]][j]))
            fatal("Ambiguous 'area_fished' command label for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
        }
        command = "area_fished["+trip_species[trips[i]][j]+"].";
      */
      } else if(in(labels,trips[i]+"_"+trip_species[trips[i]][j])){
        command="area_fished["+trips[i]+"_"+trip_species[trips[i]][j]+"].";
      } else  {
        fatal("You must define area_fished for trip "+trips[i]+ " and species "+trip_species[trips[i]][j]);
      }
      double default_value=p.get_double(command+"default_value");
      if(default_value<=0)
        fatal(command+"default_value must be positive.");
      std::vector<int> stations=t_station.select_station_no(trips[i]);
      std::vector<std::string> values;
      std::vector<double> other_stations;
      std::vector<double> other_values;
      if(p.present(command+"other_stations") && p.present(command+"other_values")){
        other_stations=p.get_double_vector(command+"other_stations");
        other_values=p.get_double_vector(command+"other_values");
        if(other_stations.size()!=other_values.size())
          fatal(command+"other_stations and "+command+"other_values not of the same size.");
        if(unique(other_stations)!=other_stations)
          fatal("You have defined duplicated stations in "+command+"other_stations");
        for(int k=0;k<other_stations.size();k++){
          if((int)other_stations[k]!=other_stations[k]){
            fatal(dtos(other_stations[k])+" in "+command+"other_stations is not an integer");
          }
          if(!in(stations,(int)other_stations[k])){
            fatal(dtos(other_stations[k])+" in "+command+"other_stations does not exist in t_station table");
          }
        }
        for(int k=0;k<other_values.size();k++){
          if(other_values[k]<=0){
            fatal(dtos(other_values[k])+" in "+command+"other_values be positive.");
          }
        }
      } else if(p.present(command+"other_stations") || p.present(command+"other_values")){
        fatal("You must define both "+command+"other_stations and "+command+"other_values, or neither of them.");
      }
      for(int k=0;k<stations.size();k++){
        int position = pos(other_stations,stations[k]);
        if(position==-1)
          trip_species_station_area_fished[trips[i]][trip_species[trips[i]][j]][stations[k]]=default_value;
        else
          trip_species_station_area_fished[trips[i]][trip_species[trips[i]][j]][stations[k]]=other_values[position];
      }
    }
  }
}

DataManager::T_table::T_table(DataManager* p_DM,std::string input_file,std::string output_file):input_file(input_file),output_file(output_file),p_DM(p_DM){
  DEBUG0("DataManager::T_table::T_table");
}
DataManager::T_table::~T_table(){
  DEBUG0("DataManager::T_table::~T_table");
  if(p_DM->input_mode=="from_DB" && input_file!="" && !p_DM->z_flag){
    #ifndef __MINGW32__
      unlink(input_file.c_str());
    #else
      _unlink(input_file.c_str());
    #endif
  }
}


std::vector<std::string> DataManager::T_table::read_file(ifstream& in){
  DEBUG0("DataManager::T_table::read_file");
  std::vector<std::string> lines;
  while(!in.eof()){
    std::string line;
    getline(in,line);
    if(!in.fail() && !line.empty())
      lines.push_back(line);
  }
  return lines;
}



void DataManager::T_table::output(Parameter_set& p,Output_precision& output_precision){
  DEBUG0("DataManager::T_table::output");
  if(!output_file.empty()){
    ofstream out(output_file.c_str());
    if (!out) fatal("Could not open file " + output_file);
    output_to_file(out,output_precision);
    out.close();
  }
}


Output_precision::Output_precision(Parameter_set &p){
  DEBUG2("Output_precision::Output_precision");
  std::vector<std::string> _quantity;
  std::vector<std::string> _type;
  std::vector<double> _precision;
  if(p.present("output_precision.quantity")){
    _quantity=p.get_string_vector("output_precision.quantity");
    _type=p.get_string_vector("output_precision.type");
    _precision=p.get_double_vector("output_precision.precision");
  }
  if(_quantity.size()!=_type.size() || _quantity.size()!=_precision.size())
    fatal("output_precision.quantity,output_precision.type,and output_precision.precision must be of the same size");
  if(unique(_quantity).size() != _quantity.size())
    fatal("output_precision.quantity contains duplicate values.");
  for(int i=0;i<_quantity.size();i++){
    if(_quantity[i]!="density" && _quantity[i]!="biomass" && _quantity[i]!="LF_number" && _quantity[i]!="cv" && _quantity[i]!="gain")
      fatal("Output_precision.quantity must contain 'density','biomass','LF_number','cv', or 'gain'");
  }
  for(int i=0;i<_type.size();i++){
    if(_type[i]!="sig_fig" && _type[i]!="dec_place")
      fatal("output_precision.type must contain 'sig_fig' or 'dec_place'");
  }
  for(int i=0;i<_precision.size();i++){
    if(_precision[i]!=(int)_precision[i])
      fatal("output_precision.precision must contain integers");
  }
  // construct defaults
  type.insert(make_pair("density","sig_fig"));
  type.insert(make_pair("biomass","sig_fig"));
  type.insert(make_pair("LF_number","sig_fig"));
  type.insert(make_pair("cv","dec_place"));
  type.insert(make_pair("gain","dec_place"));

  precision.insert(make_pair("density",4));
  precision.insert(make_pair("biomass",4));
  precision.insert(make_pair("LF_number",4));
  precision.insert(make_pair("cv",1));
  precision.insert(make_pair("gain",0));

  // set precisions
  for(int i=0;i<_quantity.size();i++){
    type[_quantity[i]]=_type[i];
    precision[_quantity[i]]=_precision[i];
  }
}


