// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* trip_cpp_id = "$Id: trip.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "survey.h"

Trip::Trip(std::string _trip_code,Survey* _p_survey,DataManager* p_DM):constant_speed(D_NULL),constant_doorspread(D_NULL)
{
	DEBUG0("Trip::Trip");
	path=" trip "+trip_code;

	p_survey=_p_survey;
	trip_code=_trip_code;
	
	if(p_survey->p_DM->catch_kg_km2_supplied){
		if(p_survey->preferences_labels.size()!=0)
			warning("@preferences is ignored’ as catch rates are supplied in the input file'");
		if(p_survey->constant_speed_labels.size()!=0)
			warning("@constant_speed is ignored’ as catch rates are supplied in the input file'");
		if(p_survey->constant_doorspread_labels.size()!=0)
			warning("@constant_doorspread is ignored’ as catch rates are supplied in the input file'");
	} else {
		//Check preferences
		if(p_survey->preferences_labels.size()>0){
			std::string command(p_survey->preferences_labels[0]==""?"preferences.":"preferences["+trip_code+"].");
			if(!(p_survey->task=="calc_LFs" && p_survey->LF_scaling=="unscaled")){
				preferences_catch_weight=p_survey->p.get_string_vector(command+"catch_weight");
				if(preferences_catch_weight!=unique(preferences_catch_weight))
					fatal(command+"catch_weight contain replicated elements.");
				for(int i=0;i<preferences_catch_weight.size();i++){
					if(preferences_catch_weight[i]!="recorded" && preferences_catch_weight[i]!="calculated") 
						fatal(command+"catch_weight must be a vector of 'recorded' and 'calculated'");
				}
			} else {
				if(p_survey->p.present(command+"catch_weight"))
					warning(command+"catch_weight is ignored’ for run-time task 'calc_LFs' with LF_scaling being 'unscaled'");
			}
			if(p_survey->p.present(command+"distance_towed") && p_survey->type=="pot")
				fatal("You must not define "+command+"distance_towed for a survey of type 'pot'");
			if(!(p_survey->task=="calc_LFs" && p_survey->LF_scaling!="numbers_per_km2") && p_survey->type!="pot"){
				preferences_distance_towed=p_survey->p.get_string_vector(command+"distance_towed");
				if(preferences_distance_towed!=unique(preferences_distance_towed))
					fatal(command+"distance_towed contain replicated elements.");
				for(int i=0;i<preferences_distance_towed.size();i++){
					if(preferences_distance_towed[i]!="recorded_distance" && preferences_distance_towed[i]!="recorded_speed*time" 
						&& preferences_distance_towed[i]!="constant_speed*time" &&  preferences_distance_towed[i]!="from_lat_long") 
						fatal(command+"distance_towed must be a vector of 'recorded_distance','recorded_speed*time','constant_speed*time', and 'from_lat_long'");
				}
			} else {
				if(p_survey->p.present(command+"distance_towed"))
					warning(command+"distance_towed is ignored’ for run-time task 'calc_LFs' with LF_scaling not being 'numbers_per_km2'");
			}
			if(p_survey->p.present(command+"width_swept") && p_survey->type=="pot")
				fatal("You must not define "+command+"width_swept for a survey of type 'pot'");	
			if(!(p_survey->task=="calc_LFs" && p_survey->LF_scaling!="numbers_per_km2")&& p_survey->type!="pot"){
				preferences_width_swept=p_survey->p.get_string_vector(command+"width_swept");
				if(preferences_width_swept!=unique(preferences_width_swept))
					fatal(command+"width_swept contain replicated elements.");
				for(int i=0;i<preferences_width_swept.size();i++){
					if(preferences_width_swept[i]!="recorded_doorspread" && preferences_width_swept[i]!="recorded_wingspread" && preferences_width_swept[i]!="constant_doorspread") 
						fatal(command+"width_swept must be a vector of 'recorded_doorspread', 'recorded_wingspread' and 'constant_doorspread'");
				}	
			} else {
				if(p_survey->p.present(command+"width_swept"))
					warning(command+"width_swept is ignored’ for run-time task 'calc_LFs' with LF_scaling not being 'numbers_per_km2'");
			}
		} else if(!(p_survey->task=="calc_LFs" && p_survey->LF_scaling=="unscaled")){
				fatal("You must define @preferences for run-time task "+p_survey->task);
		}
		//Check constant_speed
		if(p_survey->constant_speed_labels.size()>0){
			if(p_survey->type=="pot")
				fatal("You must not define constant_speed for a survey of type 'pot'");	
			if(p_survey->constant_speed_labels[0]=="" || in(p_survey->constant_speed_labels,trip_code)){
				std::string command(p_survey->constant_speed_labels[0]==""?"constant_speed.":"constant_speed["+trip_code+"].");
				constant_speed=p_survey->p.get_double(command+"value");
				if(constant_speed<=0) fatal(command+"value must be positive");
			}
		}
		//Check constant_doorspread
		if(p_survey->constant_doorspread_labels.size()>0){
			if(p_survey->type=="pot")
				fatal("You must not define constant_doorspread for a survey of type 'pot'");	
			if(p_survey->constant_doorspread_labels[0]=="" || in(p_survey->constant_doorspread_labels,trip_code)){
				std::string command(p_survey->constant_doorspread_labels[0]==""?"constant_doorspread.":"constant_doorspread["+trip_code+"].");
				constant_doorspread=p_survey->p.get_double(command+"value");
				if(constant_doorspread<=0) fatal(command+"value must be positive");
			}
		}

		std::string temp = "constant_speed*time";
		if(in(preferences_distance_towed,temp) && constant_speed==D_NULL)
			fatal("You have defined 'constant_speed*time' in the 'distance_towed' of 'preferences' command for "+trip_code+", but not provided the corresponding @constant_speed");

		temp = "constant_doorspread";
		if(in(preferences_width_swept,temp) && constant_doorspread==D_NULL)
			fatal("You have defined 'constant_doorspread' in the 'width_swept' of 'preferences' command for "+trip_code+", but not provided the corresponding @constant_doorspread");
	}
	for(int i=0;i<p_DM->trip_species[trip_code].size();i++){
		species.push_back(new Species(p_DM->trip_species[trip_code][i],this,p_DM));
	}

		
}

void Trip::print()
{	
	cerr<<trip_code<<endl;
	for(int i=0;i<species.size();i++)
		species[i]->print();
	cerr<<preferences_distance_towed<<endl;
	cerr<<preferences_width_swept<<endl;
	cerr<<preferences_catch_weight<<endl;
	cerr<<"constant_speed="<<constant_speed<<" "<<"constant_doorspread="<<constant_doorspread<<endl;

}

void Trip::calc_LFs_ancillary(){
	DEBUG0("Trip::calc_LFs_ancillary");
	for(int i=0;i<species.size();i++){
		species[i]->calc_LFs_ancillary();
	}
}

void Trip::calc_biomass_ancillary(){
	DEBUG0("Trip::calc_biomass_ancillary");
	for(int i=0;i<species.size();i++){
		species[i]->calc_biomass_ancillary();
		if(species[i]->sub_populations.size()>0){
			species[i]->calculate_sub_populations_ancillary();
		}
	}

}


void Trip::calc_biomass_and_LFs_ancillary(){
	DEBUG0("Trip::calc_biomass_ancillary");
	for(int i=0;i<species.size();i++){
		species[i]->calc_biomass_and_LFs_ancillary();
		if(species[i]->sub_populations.size()>0){
			species[i]->calculate_sub_populations_ancillary();
		}
	}

}

void Trip::calc_LFs(){
	DEBUG0("Trip::calc_LFs");
	for(int i=0;i<species.size();i++){
		species[i]->calc_LFs();
	}

}

void Trip::calc_biomass(){
	DEBUG0("Trip::calc_biomass");
	for(int i=0;i<species.size();i++){
		species[i]->calc_biomass();
		if(species[i]->sub_populations.size()>0){
			species[i]->calculate_sub_populations();
		}
	}

}

void Trip::calc_biomass_and_LFs(){
	DEBUG0("Trip::calc_biomass_and_LFs");
	for(int i=0;i<species.size();i++){
		species[i]->calc_biomass_and_LFs();
		if(species[i]->sub_populations.size()>0){
			species[i]->calculate_sub_populations();
		}
	}
}



void Trip::output_LFs(Output_precision& output_precision,ostream& out){

	DEBUG0("Trip::output_LFs");
	out<<"================================================================================\n";
	out<<"Analysis for trip "+trip_code+"\n";
	out<<"================================================================================\n\n";
	print_selection(output_precision,out);
	print_run_parameters(output_precision,out);
	for(int i=0;i<species.size();i++){
		out<<"========================================\n";
		out<<"Estimates for species "+species[i]->species_code+" in trip "+trip_code+"\n";
		out<<"========================================\n\n";
		species[i]->print_diagnostics_LFs(output_precision,out);
		species[i]->print_LF_overall(output_precision,out);
		if(p_survey->p.get_bool("output_tables.LFs_by_stratum",false))
			species[i]->print_LFs_by_stratum(output_precision,out);
		if(p_survey->p.get_bool("output_tables.LFs_by_station",false))
			species[i]->print_LFs_by_station(output_precision,out);
		if(p_survey->p.get_bool("output_tables.Number_measured",false))
			species[i]->print_Number_measured(output_precision,out);
	}	
}

void Trip::output_biomass(Output_precision& output_precision,ostream& out){
	DEBUG0("Trip::output_biomass");
	out<<"===============================================================s=================\n";
	out<<"Analysis for trip "+trip_code+"\n";
	out<<"================================================================================\n\n";
	print_selection(output_precision,out);
	print_run_parameters(output_precision,out);
	for(int i=0;i<species.size();i++){
		out<<"========================================\n";
		out<<"Estimates for species "+species[i]->species_code+" in trip "+trip_code+"\n";
		out<<"========================================\n\n";
		species[i]->print_diagnostics_biomass(output_precision,out);
		species[i]->print_stratum_summary(output_precision,out);
		species[i]->print_biomass(output_precision,out);
		if(p_survey->p.get_bool("output_tables.sub_biomass_by_stratum",false) && species[i]->sub_populations.size()>0)
			species[i]->print_sub_biomass_by_stratum(output_precision,out);
	}
	if(p_survey->p.get_bool("output_tables.biomass_by_species",false) && species.size()>1)
		this->print_biomass_by_species(output_precision,out);
	if(p_survey->p.get_bool("output_tables.biomass_by_species_stratum",false) && species.size()>1)
		this->print_biomass_by_species_stratum(output_precision,out);
	if(p_survey->p.present("projected_cvs.strata") && p_survey->p.present("projected_cvs.extra_stations")){
		this->print_projected_cvs(output_precision,out);
	}

}

void Trip::output_biomass_and_LFs(Output_precision& output_precision,ostream& out){
	DEBUG0("Trip::output_biomass_and_LFs");
	out<<"================================================================================\n";
	out<<"Analysis for trip "+trip_code+"\n";
	out<<"================================================================================\n\n";
	print_selection(output_precision,out);
	print_run_parameters(output_precision,out);
	for(int i=0;i<species.size();i++){
		out<<"========================================\n";
		out<<"Estimates for species "+species[i]->species_code+" in trip "+trip_code+"\n";
		out<<"========================================\n\n";
		species[i]->print_diagnostics_biomass_and_LFs(output_precision,out);
		species[i]->print_stratum_summary(output_precision,out);
		species[i]->print_biomass(output_precision,out);
		if(p_survey->p.get_bool("output_tables.sub_biomass_by_stratum",false) && species[i]->sub_populations.size()>0)
			species[i]->print_sub_biomass_by_stratum(output_precision,out);
		species[i]->print_LF_overall(output_precision,out);
		if(p_survey->p.get_bool("output_tables.LFs_by_stratum",false))
			species[i]->print_LFs_by_stratum(output_precision,out);
		if(p_survey->p.get_bool("output_tables.LFs_by_station",false))
			species[i]->print_LFs_by_station(output_precision,out);
		if(p_survey->p.get_bool("output_tables.Number_measured",false))
			species[i]->print_Number_measured(output_precision,out);
		if(p_survey->p.get_bool("output_tables.LF_totals",false))
			species[i]->print_LF_totals(output_precision,out);
	}
	if(p_survey->p.get_bool("output_tables.biomass_by_species",false) && species.size()>1)
		this->print_biomass_by_species(output_precision,out);
	if(p_survey->p.get_bool("output_tables.biomass_by_species_stratum",false) && species.size()>1)
		this->print_biomass_by_species_stratum(output_precision,out);
	if(p_survey->p.present("projected_cvs.strata") && p_survey->p.present("projected_cvs.extra_stations")){
		this->print_projected_cvs(output_precision,out);
	}

}

void Trip::output_for_catch_at_age(Output_precision& output_precision,ostream& out){
	DEBUG0("Trip::output_for_catch_at_age");
	out<<"================================================================================\n";
	out<<"Analysis for trip "+trip_code+"\n";
	out<<"================================================================================\n\n";
	print_selection(output_precision,out);
	print_run_parameters(output_precision,out);
	for(int i=0;i<species.size();i++){
		out<<"========================================\n";
		out<<"Estimates for species "+species[i]->species_code+" in trip "+trip_code+"\n";
		out<<"========================================\n\n";
		species[i]->print_diagnostics_biomass_and_LFs(output_precision,out);
		//output to catch_at_age_file
		std::string output_for_catch_at_age_file;
		int run_no=1;
		while (1){ // loop through potential run numbers
			output_for_catch_at_age_file = "LF"+trip_code+species[i]->species_code+
				(p_survey->p_DM->t_lgth.subcatch_defined(species[i]->species_code)? "sub.":".")+ itos(run_no);
			ifstream out(output_for_catch_at_age_file.c_str());
			if (!out.good()) break; // neither file already exists: use this run number
			else run_no++;
		}
		species[i]->print_for_catch_at_age(output_precision,output_for_catch_at_age_file);
	}
}


void Trip::output_phase_2(Output_precision& output_precision,ostream& out){
	DEBUG0("Trip::output_phase_2");
	out<<"================================================================================\n";
	out<<"Analysis for trip "+trip_code+"\n";
	out<<"================================================================================\n\n";
	print_selection(output_precision,out);
	print_run_parameters(output_precision,out);	
	for(int i=0;i<species.size();i++){
		if(species[i]->phase_2_requested){
			out<<"========================================\n";
			out<<"Estimates for species "+species[i]->species_code+" in trip "+trip_code+"\n";
			out<<"========================================\n\n";
			species[i]->print_diagnostics_biomass(output_precision,out);
			species[i]->print_phase_2(output_precision,out);
		}
	}
}

void Trip::output_t_table(){
	DEBUG0("Trip::output_t_table");

	if(p_survey->p_DM->output_station)
		output_t_station();
	if(p_survey->p_DM->output_stratum)
		output_t_stratum();
	if(p_survey->p_DM->output_catch)
		output_t_catch();
	if(p_survey->p_DM->output_lgth)
		output_t_lgth();
	if(p_survey->p_DM->output_subcatch)
		output_t_subcatch();
	if(p_survey->p_DM->output_station_catch)
		output_t_station_catch();
	if(p_survey->p_DM->output_stratum_catch)
		output_t_stratum_catch();
	
}


void Trip::output_t_stratum()
{
	DEBUG1("Trip::print_t_stratum");
	DataManager* p_DM = p_survey->p_DM;
	for(int i=0;i<species.size();i++){
		for(int j=0;j<species[i]->strata.size();j++){
			Stratum* p_stratum=species[i]->strata[j];
			p_DM->t_stratum.insert(trip_code,p_stratum->stratum,p_stratum->area_km2);
		}
		break;
	}
}


void Trip::output_t_station()
{
	DEBUG1("Trip::print_t_station");
	DataManager* p_DM=p_survey->p_DM;
	std::string task = p_survey->task;
	std::string LF_scaling= p_survey->LF_scaling;
	// print to t_station
	if(p_DM->catch_kg_km2_supplied){
		p_DM->t_station.set_cols(p_DM->t_station_catch.trip_code_col,p_DM->t_station_catch.stratum_col,p_DM->t_station_catch.station_no_col,
			-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
		}

	for(int i=0;i<species.size();i++){
		for(int j=0;j<species[i]->strata.size();j++){
			for(int k=0;k<species[i]->strata[j]->stations.size();k++){
				Station* p_station=species[i]->strata[j]->stations[k];
					p_DM->t_station.insert(trip_code,species[i]->strata[j]->stratum,p_station->station_no,
											p_station->distance,p_station->speed,p_station->dist_doors,p_station->dist_wings,
											p_station->lat_s,p_station->long_s,p_station->lat_f,p_station->long_f,
											p_station->nors_s,p_station->eorw_s,p_station->nors_f,p_station->eorw_f,
											p_station->time_s,p_station->date_s,p_station->time_f,p_station->date_f);
				
			}
		}
		break;
	}
}



void Trip::output_t_catch()
{
	DEBUG1("Trip::print_t_catch");
	DataManager* p_DM=p_survey->p_DM;

	// print to t_catch
	for(int i=0;i<species.size();i++){
		for(int j=0;j<species[i]->strata.size();j++){
			for(int k=0;k<species[i]->strata[j]->stations.size();k++){
				Station* p_station=species[i]->strata[j]->stations[k];
				p_DM->t_catch.insert(trip_code,species[i]->strata[j]->stratum,p_station->station_no,species[i]->species_code,p_station->weight);
			}
		}
	}
}


void Trip::output_t_lgth()
{
	DEBUG1("Species::print_t_lgth");
	DataManager* p_DM=p_survey->p_DM;
	std::string task = p_survey->task;
	std::string LF_scaling= p_survey->LF_scaling;

	// print to t_lgth
	for(int i=0;i<species.size();i++){
		for(int j=0;j<species[i]->strata.size();j++){
			for(int k=0;k<species[i]->strata[j]->stations.size();k++){
				Station* p_station=species[i]->strata[j]->stations[k];
				for(int l=0;l<p_station->lgth.size();l++){
					if(p_station->lgth_subcatch_no.size()<=0){
						p_DM->t_lgth.insert(trip_code,species[i]->strata[j]->stratum,p_station->station_no,species[i]->species_code,
								p_station->lgth[l],p_station->percent_samp[l],p_station->no_a[l],p_station->no_f[l],p_station->no_m[l]);
					} else {
						p_DM->t_lgth.insert(trip_code,species[i]->strata[j]->stratum,p_station->station_no,species[i]->species_code,p_station->lgth_subcatch_no[l],
								p_station->lgth[l],p_station->percent_samp[l],p_station->no_a[l],p_station->no_f[l],p_station->no_m[l]);
					}
				}
			}
		}
	}
}

void Trip::output_t_subcatch()
{
	DEBUG1("Trip::print_t_subcatch");
	DataManager* p_DM=p_survey->p_DM;

	// print to t_catch
	for(int i=0;i<species.size();i++){
		for(int j=0;j<species[i]->strata.size();j++){
			for(int k=0;k<species[i]->strata[j]->stations.size();k++){
				Station* p_station=species[i]->strata[j]->stations[k];
				for(int l=0;l<p_station->subcatch_no.size();l++)
					p_DM->t_subcatch.insert(trip_code,species[i]->strata[j]->stratum,p_station->station_no,species[i]->species_code,p_station->subcatch_no[l],p_station->subcatch_weight[l]);
			}
		}
	}
}
void Trip::output_t_station_catch()
{
	DEBUG1("Trip::print_t_station");
	DataManager* p_DM=p_survey->p_DM;
	std::string task = p_survey->task;
	std::string LF_scaling= p_survey->LF_scaling;

	if(!p_DM->catch_kg_km2_supplied){
		p_DM->t_station_catch.set_cols(p_DM->t_station.trip_code_col,p_DM->t_station.stratum_col,p_DM->t_station.station_no_col,
			p_DM->t_station.distance_col,p_DM->t_station.speed_col,p_DM->t_station.dist_doors_col,p_DM->t_station.dist_wings_col,
			p_DM->t_station.lat_s_col,p_DM->t_station.long_s_col,p_DM->t_station.lat_f_col,p_DM->t_station.long_f_col,
			p_DM->t_station.nors_s_col,p_DM->t_station.eorw_s_col,p_DM->t_station.nors_f_col,p_DM->t_station.eorw_f_col,
			p_DM->t_station.time_s_col,p_DM->t_station.date_s_col,p_DM->t_station.time_f_col,p_DM->t_station.date_f_col);
	}
	// print to t_station
	for(int i=0;i<species[0]->strata.size();i++){
		for(int j=0;j<species[0]->strata[i]->stations.size();j++){
			Station *p_station=species[0]->strata[i]->stations[j];
			std::vector<std::vector<double> > _weight_kg_km2,_C,_LD,_AD;
			if(p_DM->catch_kg_km2_supplied){
				vector<double> _weight_kg_km2;
				for(int k=0;k<p_DM->species.size();k++){
					int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
					if(position==-1)
						_weight_kg_km2.push_back(D_NULL);
					else
						_weight_kg_km2.push_back(species[position]->strata[i]->stations[j]->weight_kg_km2);
				}
				p_DM->t_station_catch.insert(trip_code,p_station->p_stratum->stratum,p_station->station_no,_weight_kg_km2);				
			} else if((p_survey->task == "calc_LFs" && p_survey->LF_scaling=="numbers_per_km2") || (p_survey->task == "calc_biomass") || (p_survey->task == "calc_biomass_and_LFs") || (p_survey->task == "phase_2_calc") || (p_survey->task == "output_for_catch_at_age")){
				if(p_survey->type=="trawl"){
					vector<double> _C,_LD,_AD;
					for(int k=0;k<p_DM->species.size();k++){
						int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
						if(position==-1)
						{	_C.push_back(D_NULL);
							_LD.push_back(D_NULL);
							_AD.push_back(D_NULL);
						} else {
							_C.push_back(species[position]->strata[i]->stations[j]->C);
							_LD.push_back(species[position]->strata[i]->stations[j]->LD);
							_AD.push_back(species[position]->strata[i]->stations[j]->AD);
						}
					}
					if(!p_survey->sub_populations_calculated()){
						p_DM->t_station_catch.insert5(trip_code,p_station->p_stratum->stratum,p_station->station_no,
									p_station->distance,p_station->speed,p_station->dist_doors,p_station->dist_wings,
									p_station->lat_s,p_station->long_s,p_station->lat_f,p_station->long_f,
									p_station->nors_s,p_station->eorw_s,p_station->nors_f,p_station->eorw_f,
									p_station->time_s,p_station->date_s,p_station->time_f,p_station->date_f,
									p_station->d,p_station->w,_C,_LD,_AD);
					} else {
						vector<vector<double> > _c,_ad;
						for(int k=0;k<p_DM->species.size();k++){
							if(in(p_DM->species_sub_populatons,p_DM->species[k])){
								int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
								if(position==-1)
								{	
									vector<double> temp;
									for(int l=0;l<p_DM->species_sub_populatons[p_DM->species[k]].size();l++) temp.push_back(D_NULL);
									_c.push_back(temp);
									_ad.push_back(temp);
								} else if(species[position]->strata[i]->ex_for_sub_population || species[position]->strata[i]->excluded || species[position]->strata[i]->stations[j]->excluded){
									vector<double> temp;
									for(int l=0;l<p_DM->species_sub_populatons[p_DM->species[k]].size();l++) temp.push_back(D_NULL);
									_c.push_back(temp);
									_ad.push_back(temp);
								} else {
									_c.push_back(species[position]->strata[i]->stations[j]->c);
									_ad.push_back(species[position]->strata[i]->stations[j]->ad);
								}
							}
						}

						p_DM->t_station_catch.insert5(trip_code,p_station->p_stratum->stratum,p_station->station_no,
									p_station->distance,p_station->speed,p_station->dist_doors,p_station->dist_wings,
									p_station->lat_s,p_station->long_s,p_station->lat_f,p_station->long_f,
									p_station->nors_s,p_station->eorw_s,p_station->nors_f,p_station->eorw_f,
									p_station->time_s,p_station->date_s,p_station->time_f,p_station->date_f,
									p_station->d,p_station->w,_C,_LD,_AD,_c,_ad);
					}
				} else if(p_survey->type=="pot"){
					vector<double> _C,_AD;
					for(int k=0;k<p_DM->species.size();k++){
						int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
						if(position==-1)
						{	_C.push_back(D_NULL);
							_AD.push_back(D_NULL);
						} else {
							_C.push_back(species[position]->strata[i]->stations[j]->C);
							_AD.push_back(species[position]->strata[i]->stations[j]->AD);
						}
						p_DM->t_station_catch.insert4(trip_code,p_station->p_stratum->stratum,p_station->station_no,
										p_station->distance,p_station->speed,p_station->dist_doors,p_station->dist_wings,
										p_station->lat_s,p_station->long_s,p_station->lat_f,p_station->long_f,
										p_station->nors_s,p_station->eorw_s,p_station->nors_f,p_station->eorw_f,
										p_station->time_s,p_station->date_s,p_station->time_f,p_station->date_f,
										p_station->area_fished,_C,_AD);
					}
				}
			} else if (p_survey->task == "calc_LFs" && p_survey->LF_scaling=="unscaled"){
				p_DM->t_station_catch.insert(trip_code,p_station->p_stratum->stratum,p_station->station_no,
										p_station->distance,p_station->speed,p_station->dist_doors,p_station->dist_wings,
										p_station->lat_s,p_station->long_s,p_station->lat_f,p_station->long_f,
										p_station->nors_s,p_station->eorw_s,p_station->nors_f,p_station->eorw_f,
										p_station->time_s,p_station->date_s,p_station->time_f,p_station->date_f);
			} else if(p_survey->task == "calc_LFs" && p_survey->LF_scaling=="numbers_per_hour"){
					vector<double> _C;
					for(int k=0;k<p_DM->species.size();k++){
						int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
						if(position==-1) _C.push_back(D_NULL);
						else _C.push_back(species[position]->strata[i]->stations[j]->C);
					}
					p_DM->t_station_catch.insert3(trip_code,p_station->p_stratum->stratum,p_station->station_no,
										p_station->distance,p_station->speed,p_station->dist_doors,p_station->dist_wings,
										p_station->lat_s,p_station->long_s,p_station->lat_f,p_station->long_f,
										p_station->nors_s,p_station->eorw_s,p_station->nors_f,p_station->eorw_f,
										p_station->time_s,p_station->date_s,p_station->time_f,p_station->date_f,
										p_station->t,_C);
			} else {
					vector<double> _C;
					for(int k=0;k<p_DM->species.size();k++){
						int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
						if(position==-1) _C.push_back(D_NULL);
						else _C.push_back(species[position]->strata[i]->stations[j]->C);
					}
					
					p_DM->t_station_catch.insert2(trip_code,p_station->p_stratum->stratum,p_station->station_no,
										p_station->distance,p_station->speed,p_station->dist_doors,p_station->dist_wings,
										p_station->lat_s,p_station->long_s,p_station->lat_f,p_station->long_f,
										p_station->nors_s,p_station->eorw_s,p_station->nors_f,p_station->eorw_f,
										p_station->time_s,p_station->date_s,p_station->time_f,p_station->date_f,
										_C);

			}
		}
	}
}

void Trip::output_t_stratum_catch()
{
	DEBUG1("Trip::print_t_stratum");
	DataManager* p_DM = p_survey->p_DM;
	p_DM->t_stratum_catch.set_cols(p_DM->t_stratum.trip_code_col,p_DM->t_stratum.stratum_col,p_DM->t_stratum.area_km2_col);
	for(int i=0;i<species[0]->strata.size();i++){
		Stratum* p_stratum=species[0]->strata[i];
		if(p_DM->catch_kg_km2_supplied){
			vector<double> _AD;
			for(int k=0;k<p_DM->species.size();k++){
				int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
				if(position==-1)
					_AD.push_back(D_NULL);
				else
					_AD.push_back(species[position]->strata[i]->AD);
			}
			p_DM->t_stratum_catch.insert3(trip_code,p_stratum->stratum,p_stratum->area_km2,_AD);
		} else if((p_survey->task == "calc_LFs" && p_survey->LF_scaling=="numbers_per_km2") || (p_survey->task == "calc_biomass") || (p_survey->task == "calc_biomass_and_LFs")|| (p_survey->task == "phase_2_calc") || (p_survey->task == "output_for_catch_at_age")){
			if(p_survey->type=="trawl"){
				vector<double> _C,_LD,_AD;
				for(int k=0;k<p_DM->species.size();k++){
					int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
					if(position==-1)
					{	_C.push_back(D_NULL);
						_LD.push_back(D_NULL);
						_AD.push_back(D_NULL);
					} else {
						_C.push_back(species[position]->strata[i]->C);
						_LD.push_back(species[position]->strata[i]->LD);
						_AD.push_back(species[position]->strata[i]->AD);
					}
				}
				p_DM->t_stratum_catch.insert2(trip_code,p_stratum->stratum,p_stratum->area_km2,_C,_LD,_AD);
			} else if(p_survey->type=="pot"){
				vector<double> _C,_AD;
				for(int k=0;k<p_DM->species.size();k++){
					int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
					if(position==-1)
					{	_C.push_back(D_NULL);
						_AD.push_back(D_NULL);
					} else {
						_C.push_back(species[position]->strata[i]->C);
						_AD.push_back(species[position]->strata[i]->AD);
					}
				}
				p_DM->t_stratum_catch.insert2(trip_code,p_stratum->stratum,p_stratum->area_km2,_C,_AD);
			}
		} else if (p_survey->task == "calc_LFs" && p_survey->LF_scaling=="unscaled"){
			p_DM->t_stratum_catch.insert2(trip_code,p_stratum->stratum,p_stratum->area_km2);
		} else {
			vector<double> _C;
			for(int k=0;k<p_DM->species.size();k++){
				int position=pos(p_DM->trip_species[trip_code],p_DM->species[k]);
				if(position==-1)
				{	_C.push_back(D_NULL);
				} else {
					_C.push_back(species[position]->strata[i]->C);
				}
			}
			p_DM->t_stratum_catch.insert2(trip_code,p_stratum->stratum,p_stratum->area_km2,_C);
		}
	}
}


void Trip::print_biomass_by_species(Output_precision& output_precision,ostream& out )
{
	DEBUG0("Trip::print_biomass_by_species");
	out<<"* BIOMASS BY SPECIES\n\n";
	//column width
	int width_species(7),width_lbound(7),width_biomass(7),width_ubound(7),width_cv(2),width_Nstations(9);
	for(int i=0;i<species.size();i++){
		width_species=fmax(width_species,species[i]->species_code.length());
		width_ubound=fmax(width_ubound,FMT_double(species[i]->B+2*species[i]->se_B,output_precision.type["biomass"],output_precision.precision["biomass"]).width());
		width_cv=fmax(width_cv,FMT_double(100*species[i]->cv_B,output_precision.type["cv"],output_precision.precision["cv"]).width());
	}
	width_biomass=width_lbound=width_ubound;
	
	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_species)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"species"<<" ";
	out<<std::setw(width_lbound)<<std::setiosflags(ios::right)<<"lbound"<<" ";
	out<<std::setw(width_biomass)<<"biomass"<<" ";
	out<<std::setw(width_ubound)<<"ubound"<<" ";
	out<<std::setw(width_cv)<<"cv"<<" ";
	out<<std::setw(width_Nstations)<<"Nstations"<<" "<<"\n";
	//content
	for(int i=0;i<species.size();i++){
		out<<std::setw(width_species)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<species[i]->species_code<<" ";
		out<<std::setw(width_lbound)<<std::setiosflags(ios::right)<<FMT_double(fmax(0,species[i]->B-2*species[i]->se_B),output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
		out<<std::setw(width_biomass)<<FMT_double(species[i]->B,output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
		out<<std::setw(width_ubound)<<FMT_double(species[i]->B+2*species[i]->se_B,output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
		out<<std::setw(width_cv)<<FMT_double(100*species[i]->cv_B,output_precision.type["cv"],output_precision.precision["cv"])<<" ";
		out<<std::setw(width_Nstations)<<species[i]->n_stations_used<<" "<<"\n";
	}
	out<<"\n";
}
void Trip::print_biomass_by_species_stratum(Output_precision& output_precision,ostream& out )
{
	DEBUG0("Trip::print_biomass_by_species_stratum");
	out<<"* BIOMASS BY SPECIES AND STRATUM\n\n";
	int width_stratum(7);
	std::vector<int> width_B;
	std::vector<int> width_cv_B;
	for(int i=0;i<species.size();i++){
		width_B.push_back(species[i]->species_code.length());
		width_cv_B.push_back(species[i]->species_code.length()+3);
	}
	for(int i=0;i<species[0]->strata.size();i++){
		width_stratum=fmax(width_stratum,species[0]->strata[i]->stratum.length());
		for(int j=0;j<species.size();j++){	//All species must have the same strata
			if(!species[j]->strata[i]->excluded){
				width_B[j]=fmax(width_B[j],FMT_double(species[j]->strata[i]->B,output_precision.type["biomass"],output_precision.precision["biomass"]).width());
				width_cv_B[j]=fmax(width_cv_B[j],FMT_double(100*species[j]->strata[i]->cv_B,output_precision.type["cv"],output_precision.precision["cv"]).width());
			}
		}
	}


	out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
	for(int i=0;i<species.size();i++){
		out<<std::setw(width_B[i])<<std::setiosflags(ios::right)<<species[i]->species_code<<" ";
		out<<std::setw(width_cv_B[i])<<"cv_"+species[i]->species_code<<" ";	
	}
	out<<"\n";
	for(int i=0;i<species[0]->strata.size();i++){
		cout<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<species[0]->strata[i]->stratum<<" ";
		for(int j=0;j<species.size();j++){	//All species must have the same strata
			if(!species[j]->strata[i]->excluded){
				out<<std::setw(width_B[j])<<std::setiosflags(ios::right)<<FMT_double(species[j]->strata[i]->B,output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
				out<<std::setw(width_cv_B[j])<<FMT_double(100*species[j]->strata[i]->cv_B,output_precision.type["cv"],output_precision.precision["cv"])<<" ";
			} else {
				out<<std::setw(width_B[j])<<std::setiosflags(ios::right)<<" NULL"<<" ";
				out<<std::setw(width_cv_B[j])<<"NULL"<<" ";
			}
		}
		out<<"\n";
	}
	out<<"\n";
}

void Trip::print_projected_cvs(Output_precision& output_precision,ostream& out){
	DEBUG0("Trip::print_projected_cvs");
	out<<"* PROJECTED CVS\n\n";
	//column width
	
	int width_species(7),width_projected_cv(12),width_population(10);
	int sub_population_defined(0);
	for(int i=0;i<species.size();i++){
		width_species=fmax(width_species,species[i]->species_code.length());
		width_projected_cv=fmax(width_projected_cv,FMT_double(100*species[i]->cv_B_proj,output_precision.type["cv"],output_precision.precision["cv"]).width());
		for(int j=0;j<species[i]->sub_populations.size();j++){
			sub_population_defined=1;
     		width_projected_cv=fmax(width_projected_cv,FMT_double(100*species[i]->cv_b_proj[j],output_precision.type["cv"],output_precision.precision["cv"]).width());
			width_population=fmax(width_population,species[i]->sub_populations[j].label.length());
		}
	}
	
	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_species)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"species"<<" ";
	if(sub_population_defined)
	    out<<std::setw(width_population)<<std::setiosflags(ios::right)<<"population"<<" ";
	out<<std::setw(width_projected_cv)<<std::setiosflags(ios::right)<<"projected_cv"<<"\n";
	//content
	for(int i=0;i<species.size();i++){
		out<<std::setw(width_species)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<species[i]->species_code<<" ";
	   if(sub_population_defined)
		    out<<std::setw(width_population)<<std::setiosflags(ios::right)<<"all"<<" ";
	   out<<std::setw(width_projected_cv)<<std::setiosflags(ios::right)<<FMT_double(100*species[i]->cv_B_proj,output_precision.type["cv"],output_precision.precision["cv"])<<" "<<"\n";
	   for(int j=0;j<species[i]->sub_populations.size();j++){
	       out<<std::setw(width_species)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<species[i]->species_code<<" ";
		   out<<std::setw(width_population)<<std::setiosflags(ios::right)<<species[i]->sub_populations[j].label<<" ";
		   out<<std::setw(width_projected_cv)<<std::setiosflags(ios::right)<<FMT_double(100*species[i]->cv_b_proj[j],output_precision.type["cv"],output_precision.precision["cv"])<<" "<<"\n";
	   }
	}
	out<<"\n";

}

void Trip::print_run_parameters(Output_precision& output_precision,ostream& out){
	DEBUG0("Trip::print_run_parameters");
	out<<"* RUN PARAMETERS\n\n";
	if(!preferences_distance_towed.size() || preferences_width_swept.size() || !preferences_catch_weight.size()){
		if(preferences_distance_towed.size()!=0){
			out<<"Distance Towed:\n";
				for(int i=0;i<preferences_distance_towed.size();i++){
					out<<"    "+itos(i+1)+(i==0?"st ":(i==1?"nd ":(i==2?"rd ":"th ")))+ "preference is "+preferences_distance_towed[i]+"\n";
				}
				for(int i=preferences_distance_towed.size()+1;i<=4;i++){
					out<<"    "+itos(i)+(i==0?"st ":(i==1?"nd ":(i==2?"rd ":"th ")))+ "preference is NULL\n";
				}
				out<<"\n";
		}
		if(preferences_width_swept.size()!=0){
			out<<"Width Swept:\n";
				for(int i=0;i<preferences_width_swept.size();i++){
					out<<"    "+itos(i+1)+(i==0?"st ":(i==1?"nd ":(i==2?"rd ":"th ")))+ "preference is "+preferences_width_swept[i]+"\n";
				}
				for(int i=preferences_width_swept.size()+1;i<=2;i++){
					out<<"    "+itos(i)+(i==0?"st ":(i==1?"nd ":(i==2?"rd ":"th ")))+ "preference is NULL\n";
				}
				out<<"\n";
		}
		if(preferences_catch_weight.size()!=0){
			out<<"Catch Weight:\n";
				for(int i=0;i<preferences_catch_weight.size();i++){
					out<<"    "+itos(i+1)+(i==0?"st ":(i==1?"nd ":(i==2?"rd ":"th ")))+ "preference is "+preferences_catch_weight[i]+"\n";
				}
				for(int i=preferences_catch_weight.size()+1;i<=2;i++){
					out<<"    "+itos(i)+(i==0?"st ":(i==1?"nd ":(i==2?"rd ":"th ")))+ "preference is NULL\n";
				}
				out<<"\n";
		}
		out<<"\n";
	}


}
void Trip::print_selection(Output_precision& output_precision,ostream& out){
	DEBUG0("Trip::print_run_parameters");
	cout<<"* SELECTIONS\n\n";
	cout<<"SELECTED STRATA (EFFECTIVE AREAS IN PARENTHESES):\n\n";
	int width_stratum(0),width_area_km2(0);
	for(int i=0;i<species[0]->strata.size();i++){
			width_stratum=fmax(width_stratum,species[0]->strata[i]->stratum.length());
			width_area_km2=fmax(width_area_km2,dtos(species[0]->strata[i]->area_km2).length());
		}
	for(int i=0;i<species[0]->strata.size();i++){
		out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<species[0]->strata[i]->stratum<<std::setiosflags(ios::left)<<" ";
		out<<std::setw(width_area_km2+2)<<std::setiosflags(ios::left)<<"("+dtos(species[0]->strata[i]->area_km2)+")";
		if((i+1) % 8==0)
			out<<"\n";
		else 
			out<<"    ";
	}
	out<<"\n\n";

	cout<<"SELECTED STATIONS (STRATUM CODES IN PARENTHESES):\n\n";
	int width_station=0;
	for(int i=0;i<species[0]->strata.size();i++){
		for(int j=0;j<species[0]->strata[i]->stations.size();j++){
			width_station=fmax(width_station,itos(species[0]->strata[i]->stations[j]->station_no).length());
		}
	}
	int counter=0;
	for(int i=0;i<species[0]->strata.size();i++){
		for(int j=0;j<species[0]->strata[i]->stations.size();j++){
			out<<std::setw(width_station)<<std::setiosflags(ios::right)<<species[0]->strata[i]->stations[j]->station_no<<std::setiosflags(ios::left)<<" ";
			out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"("+species[0]->strata[i]->stratum+")";
			if((counter+1) % 10==0)
				out<<"\n";
			else 
				out<<"    ";
			counter++;
		}
	}
	out<<"\n\n";
}




Trip::~Trip()
{
	DEBUG0("Trip::~Trip");
}

