// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* survey_cpp_id = "$Id: survey.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "survey.h"



Survey::Survey(std::string _task,std::string input_slc,
			std::string stratum_input_file,std::string station_input_file, std::string catch_input_file, std::string lgth_input_file,std::string subcatch_input_file,std::string station_catch_input_file,
			std::string stratum_output_file,std::string station_output_file, std::string catch_output_file, std::string lgth_output_file,std::string subcatch_output_file,std::string station_catch_output_file,std::string stratum_catch_output_file,bool _z_flag)
{
	DEBUG0("Survey::Survey");
	p.read_file(input_slc);
	p_DM = new DataManager(_task,p,stratum_input_file,station_input_file,catch_input_file,lgth_input_file,subcatch_input_file,station_catch_input_file,
							stratum_output_file,station_output_file,catch_output_file,lgth_output_file,subcatch_output_file,station_catch_output_file,stratum_catch_output_file,_z_flag);
	task =p_DM->task;
	LF_scaling=p_DM->LF_scaling;
	type=p_DM->type;

	// check preferences
	if(p.get_command_count("preferences")){
		preferences_labels=p.get_command_labels("preferences");
		if(preferences_labels.size()==1 && preferences_labels[0]==""){
			//if(p_DM->trips.size()!=1)
				//fatal("Ambiguous command preferences (no label)");
		} else {
			for(int i=0;i<preferences_labels.size();i++){
				if(!in(p_DM->trips,preferences_labels[i]))
					fatal("The label "+preferences_labels[i]+" of preferences is not one of the defined trips.");
			}
		}
	}
	// check constant_speed
	if(p.get_command_count("constant_speed")){
		constant_speed_labels=p.get_command_labels("constant_speed");
		if(constant_speed_labels.size()==1 && constant_speed_labels[0]==""){
			//if(p_DM->trips.size()!=1)
				//fatal("Ambiguous command constant_speed (no label)");
		} else {
			for(int i=0;i<constant_speed_labels.size();i++){
				if(!in(p_DM->trips,constant_speed_labels[i]))
					fatal("The label "+constant_speed_labels[i]+" of constant_speed is not one of the defined trips.");
			}
		}
	}
	// check constant_doorspread
	if(p.get_command_count("constant_doorspread")){
		constant_doorspread_labels=p.get_command_labels("constant_doorspread");
		if(constant_doorspread_labels.size()==1 && constant_doorspread_labels[0]==""){
			//if(p_DM->trips.size()!=1)
				//fatal("Ambiguous command constant_doorspread (no label)");
		} else {
			for(int i=0;i<constant_doorspread_labels.size();i++){
				if(!in(p_DM->trips,constant_doorspread_labels[i]))
					fatal("The label "+constant_doorspread_labels[i]+" of constant_doorspread is not one of the defined trips.");
			}
		}
	}

	// check sub_populations
	if(p.get_command_count("sub_populations")){
		sub_populations_labels=p.get_command_labels("sub_populations");
		if(sub_populations_labels.size()==1 && sub_populations_labels[0]==""){
			if(p_DM->species.size()!=1)
				fatal("Ambiguous command @sub_populations (no label)");
		} else {
			for(int i=0;i<sub_populations_labels.size();i++){
				if(!in(p_DM->species,sub_populations_labels[i]))
					fatal("The label "+sub_populations_labels[i]+" of sub_populations is not one of the defined species.");
			}
		}
	}
}


void Survey::print()
{
	for(int i=0;i<trips.size();i++)
		trips[i]->print();
}

void Survey::do_input()
{
	p_DM->input(p,catch_weight_calculated(),catch_weight_recorded(),sub_populations_calculated());
	p_DM->modify(p);
	p_DM->extend(p);
	for(int i=0;i<p_DM->trips.size();i++)
		trips.push_back(new Trip(p_DM->trips[i],this,p_DM));
}

void Survey::do_output()
{
	Output_precision output_precision(p);

	if(task=="calc_LFs" || task=="calc_biomass" || task=="calc_biomass_and_LFs" ||task=="phase_2_calc" || task=="output_t_table"){
		p_DM->empty();
		for(int i=0;i<trips.size();i++){
			trips[i]->output_t_table();
		}
		p_DM->output(p,output_precision);
	} else if(task=="output_lw_coeff"){
		p_DM->output(p,output_precision);
	}


	if(this->task=="calc_biomass"){
		cout<<"Biomass calculations output starts from here\n\n";
		for(int i=0;i<trips.size();i++){
			trips[i]->output_biomass(output_precision);
		}
		if(p.get_bool("output_tables.biomass_by_species_trip",false))
			this->print_biomass_by_species_trip(output_precision);
		cout<<"Biomass calculations output finishes from here\n\n";
	} else if (this->task=="calc_LFs"){
		cout<<"Length freqency calculations output starts from here\n\n";
		for(int i=0;i<trips.size();i++){
			trips[i]->output_LFs(output_precision);
		}
		cout<<"Length freqency calculations output finishes from here\n\n";
	} else if(this->task=="calc_biomass_and_LFs"){
		cout<<"Biomass and Length freqency calculations output starts from here\n\n";
		for(int i=0;i<trips.size();i++){
			trips[i]->output_biomass_and_LFs(output_precision);
		}
		if(p.get_bool("output_tables.biomass_by_species_trip",false))
			this->print_biomass_by_species_trip(output_precision);
		cout<<"Biomass and Length freqency calculations output finishes from here\n\n";
	} else if(this->task=="output_for_catch_at_age"){
		cout<<"Output for catch_at_age software calculations output starts from here\n\n";
		for(int i=0;i<trips.size();i++){
			trips[i]->output_for_catch_at_age(output_precision);
		}
		cout<<"Output for catch_at_age software calculations output finishes from here\n\n";
	} else if(this->task=="phase_2_calc"){
		cout<<"Phase 2 calculations output starts from here\n\n";
		for(int i=0;i<trips.size();i++){
			trips[i]->output_phase_2(output_precision);
		}
		cout<<"Phase 2 calculations output finishes from here\n\n";
	}
}

void Survey::do_calculation(){
	DEBUG0("Survey::do_LFs_calculation");
	if(task=="calc_LFs"){
		for(int i=0;i<trips.size();i++){
			trips[i]->calc_LFs_ancillary();
			trips[i]->calc_LFs();
		}
	} else if(task=="calc_biomass") {
		if(p.present("projected_cvs.strata") || p.present("projected_cvs.extra_stations")){
			if(trips.size()>1)
				fatal("You cannot do projected_cvs calculations fro multiple trips.");
		}
		for(int i=0;i<trips.size();i++){
			trips[i]->calc_biomass_ancillary();
			trips[i]->calc_biomass();
		}
	} else if (task=="calc_biomass_and_LFs") {
		if(p.present("projected_cvs.strata") || p.present("projected_cvs.extra_stations")){
			if(trips.size()>1)
				fatal("You cannot do projected_cvs calculations fro multiple trips");
		}
		for(int i=0;i<trips.size();i++){
			trips[i]->calc_biomass_and_LFs_ancillary();
			trips[i]->calc_biomass_and_LFs();
		}
	} else if(task=="output_for_catch_at_age"){
		for(int i=0;i<trips.size();i++){
			trips[i]->calc_biomass_and_LFs_ancillary();
		}
	} else if(task=="phase_2_calc"){
		//choose trips & species for phase_2_calc
		if(trips.size()>1)
			fatal("You cannot do phase_2_calc fro multiple trips.");
		std::vector<std::string> labels=p.get_command_labels("phase_2");
		if(labels.size()==0)
			fatal("You must define at least one phase_2 command block for 'phase_2_calc' task'");
		for(int i=0;i<labels.size();i++){
			if(labels[i]==""){
				if(p_DM->species.size()!=1) fatal("Ambiguous 'phase_2' command.");
			} else if(!in(p_DM->species,labels[i])) {
				fatal("The label "+labels[i]+" of phase.2 is not one of the defined species.");
			}
		}
		std::string command;
		for(int i=0; i<trips.size();i++){
			for(int j=0; j<trips[i]->species.size();j++){
				if(labels.size()==1 && labels[0]=="") {
					command = "phase_2.";
				} else if(in(labels,trips[i]->species[j]->species_code)) {
				    command="phase_2["+trips[i]->species[j]->species_code+"].";
			    } else {
				    continue;
			    }
			    trips[0]->species[j]->calc_biomass_ancillary();
			    if(trips[i]->species[i]->sub_populations.size()>0){
			        trips[i]->species[j]->calculate_sub_populations_ancillary();
				}
			    trips[i]->species[j]->phase_2_calc(command);
			}
		}
	}
}


void Survey::print_biomass_by_species_trip(Output_precision& output_precision,ostream& out)
{
	DEBUG0("Survey::print_biomass_by_species_trip");
	out<<"* BIOMASS BY SPECIES AND TRIP\n\n";
	//column width
	int width_trip(7),width_species(7),width_population(10),width_lbound(7),width_biomass(7),width_ubound(7),width_cv(2),width_Nstations(9);
	int sub_population_defined(0);

	for(int i=0;i<trips.size();i++){
		width_trip=fmax(width_trip,trips[i]->trip_code.length());
		for(int j=0;j<trips[i]->species.size();j++){
			width_species=fmax(width_species,trips[i]->species[j]->species_code.length());
			width_ubound=fmax(width_ubound,FMT_double(trips[i]->species[j]->B+2*trips[i]->species[j]->se_B,output_precision.type["biomass"],output_precision.precision["biomass"]).width());
			width_cv=fmax(width_cv,FMT_double(100*trips[i]->species[j]->cv_B,output_precision.type["cv"],output_precision.precision["cv"]).width());
			for(int k=0;k<trips[i]->species[j]->sub_populations.size();k++){
				sub_population_defined = 1;
				width_population=fmax(width_population,trips[i]->species[j]->sub_populations[k].label.length());
				width_ubound=fmax(width_ubound,FMT_double(trips[i]->species[j]->b[k]+2*trips[i]->species[j]->se_b[k],output_precision.type["biomass"],output_precision.precision["biomass"]).width());
				width_cv=fmax(width_cv,FMT_double(100*trips[i]->species[j]->cv_b[k],output_precision.type["cv"],output_precision.precision["cv"]).width());
			}
		}
	}
	width_biomass=width_lbound=width_ubound;
	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_species)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"species"<<" ";
	out<<std::setw(width_trip)<<std::setiosflags(ios::right)<<"trip"<<" ";
	if(sub_population_defined)
		out<<std::setw(width_population)<<std::setiosflags(ios::right)<<"population"<<" ";
	out<<std::setw(width_lbound)<<std::setiosflags(ios::right)<<"lbound"<<" ";
	out<<std::setw(width_biomass)<<"biomass"<<" ";
	out<<std::setw(width_ubound)<<"ubound"<<" ";
	out<<std::setw(width_cv)<<"cv"<<" ";
	out<<std::setw(width_Nstations)<<"Nstations"<<" "<<"\n";
	//content
	for(int s=0;s<p_DM->species.size();s++){
		std::string this_species = p_DM->species[s];
		for(int i=0;i<trips.size();i++){
			for(int j=0;j<trips[i]->species.size();j++){
				if(trips[i]->species[j]->species_code==this_species){
					out<<std::setw(width_species)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<this_species<<" ";
					out<<std::setw(width_trip)<<std::setiosflags(ios::right)<<trips[i]->trip_code<<" ";
					if(sub_population_defined)
						out<<std::setw(width_population)<<std::setiosflags(ios::right)<<"all"<<" ";
					out<<std::setw(width_lbound)<<std::setiosflags(ios::right)<<FMT_double(fmax(0,trips[i]->species[j]->B-2*trips[i]->species[j]->se_B),output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
					out<<std::setw(width_biomass)<<FMT_double(trips[i]->species[j]->B,output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
					out<<std::setw(width_ubound)<<FMT_double(trips[i]->species[j]->B+2*trips[i]->species[j]->se_B,output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
					out<<std::setw(width_cv)<<FMT_double(100*trips[i]->species[j]->cv_B,output_precision.type["cv"],output_precision.precision["cv"])<<" ";
					out<<std::setw(width_Nstations)<<trips[i]->species[j]->n_stations_used<<" "<<"\n";
					for(int k=0;k<trips[i]->species[j]->sub_populations.size();k++){
						out<<std::setw(width_species)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<this_species<<" ";
						out<<std::setw(width_trip)<<std::setiosflags(ios::right)<<trips[i]->trip_code<<" ";
						out<<std::setw(width_population)<<std::setiosflags(ios::right)<<trips[i]->species[j]->sub_populations[k].label<<" ";
						out<<std::setw(width_lbound)<<std::setiosflags(ios::right)<<FMT_double(fmax(0,trips[i]->species[j]->b[k]-2*trips[i]->species[j]->se_b[k]),output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
						out<<std::setw(width_biomass)<<FMT_double(trips[i]->species[j]->b[k],output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
						out<<std::setw(width_ubound)<<FMT_double(trips[i]->species[j]->b[k]+2*trips[i]->species[j]->se_b[k],output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
						out<<std::setw(width_cv)<<FMT_double(100*trips[i]->species[j]->cv_b[k],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
						out<<std::setw(width_Nstations)<<trips[i]->species[j]->n_stations_used_sub_population<<" "<<"\n";
					}
				}
			}
		}
	}

	cout.flags(old_settings);
	out<<"\n";
}

bool Survey::catch_weight_calculated(){
	DEBUG0("Survey::catch_weight_calculated");
	bool result=false;
	for(int i=0;i<preferences_labels.size();i++){
		std::string command(preferences_labels[0]==""?"preferences.":"preferences["+preferences_labels[i]+"].");
		if(p.present(command+"catch_weight")){
			std::string temp="calculated";
			if(in(p.get_string_vector(command+"catch_weight"),temp)){
				result=true;
				break;
			}
		}
	}
	return result;
}

bool Survey::catch_weight_recorded(){
	DEBUG0("Survey::catch_weight_recorded");
	bool result=false;
	for(int i=0;i<preferences_labels.size();i++){
		std::string command(preferences_labels[0]==""?"preferences.":"preferences["+preferences_labels[i]+"].");
		if(p.present(command+"catch_weight")){
			std::string temp="recorded";
			if(in(p.get_string_vector(command+"catch_weight"),temp)){
				result=true;
				break;
			}
		}
	}
	return result;
}

bool Survey::sub_populations_calculated(){
	DEBUG0("Survey::lw_coeff_required");
	bool result=false;
	if(sub_populations_labels.size()>0)
		result=true;
	return result;
}


Survey::~Survey(){
	DEBUG0("Survey::~Survey");
	delete p_DM;
	for(int i=0;i<trips.size();i++)
		delete trips[i];

}




Length_based_mean_weight::Length_based_mean_weight(double _a1,double _b1,double _c1,double _a2,double _b2,double _c2)
{
	DEBUG0("Length_based_mean_weight::Length_based_mean_weight");
	if(_a1==D_NULL || _b1==D_NULL)
		fatal("Length_based_mean_weight parameters a1,b1, and c1 cannot be set to D_NULL");
	a1=_a1;b1=_b1;c1=_c1;
	if(a2!=D_NULL || b2!=D_NULL){
		if(_a1==D_NULL || _b1==D_NULL)
			fatal("Some (but not all) of your Length_based_mean_weight parameters a2,b2, and c2 are D_NULL");
		a2=_a2;b2=_b2;c2=_c2;
	} else {
		a2=D_NULL;b2=D_NULL;c2=D_NULL;
	}
}

double Length_based_mean_weight::get_mean_weight(double l){
	DEBUG2("Length_based_mean_weight::get_mean_weight");
	l=(l==0?l+0.1:l);	//This is nessessary to avoid getting zero weigth for a fish
	if(a2!=D_NULL && b2!=D_NULL){
		return (a1*pow(l,b1+(c1!=D_NULL?c1:0)*log(l))+ a2*pow(l,b2+(c2!=D_NULL?c2:0)*log(l)))/2;;
	} else {
		return a1*pow(l,b1+(c1!=D_NULL?c1:0)*log(l));
	}
}
void Length_based_mean_weight::print(){
cerr<<"a1="<<a1<<" b1="<<b1<<" c1="<<c1<<" a2="<<a2<<" b2="<<b2<<" c2="<<c2<<endl;
}

Sub_population::Sub_population(std::string _label,std::string _sex, int _Lmin, int _Lmax)
{
	DEBUG0("Sub_pupulation::Sub_pupulation");
	label=_label;
	if(_sex=="all"){
		sexes.push_back(0);
		sexes.push_back(1);
		sexes.push_back(2);
	} else if(_sex=="female"){
		sexes.push_back(0);
	} else if(_sex=="male"){
		sexes.push_back(1);
	} else if(_sex=="unsexed"){
		sexes.push_back(2);
	} else {
		fatal("Sub_population::Sub_pupulation: sex has to be one of 'all','female','male', or 'unsexed'.");
	}
	Lmin=_Lmin;
	Lmax=_Lmax;

}

bool Sub_population::in_sub_population(int sex,int L){
	DEBUG2("Sub_pupulation::in_sub_population");
	if(in(sexes,sex)&& Lmin<=L && Lmax>L) {
		return true;
	} else {
		return false;
	}
}
void Sub_population::print(){
	cerr<<"sexes "<<sexes<<" "<<"Lmin "<<Lmin<<" "<<"Lmax "<<Lmax<<endl;
}
