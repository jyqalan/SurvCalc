// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* species_cpp_id = "$Id: species.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "survey.h"

Species::Species(std::string _species_code,Trip* _p_trip,DataManager* p_DM)
{
	DEBUG0("Species::Species");
	path=" species "+_species_code+" trip "+_p_trip->trip_code;
	p_trip=_p_trip;

	species_code= _species_code;
	std::vector<std::string> _strata=p_DM->t_stratum.select_stratum(this->p_trip->trip_code);
	std::vector<double> _area_km2=p_DM->t_stratum.select_area_km2(this->p_trip->trip_code);

	//create mean_weights
	std::vector<double> _a=p_DM->t_lw_coeff.select_lw_coeff_a(p_trip->trip_code, species_code,D_NULL);
	std::vector<double> _b=p_DM->t_lw_coeff.select_lw_coeff_b(p_trip->trip_code, species_code,D_NULL);
	std::vector<double> _c=p_DM->t_lw_coeff.select_lw_coeff_c(p_trip->trip_code, species_code,D_NULL);
	std::vector<double> _a_male=p_DM->t_lw_coeff.select_lw_coeff_a(p_trip->trip_code, species_code,1);
	std::vector<double> _b_male=p_DM->t_lw_coeff.select_lw_coeff_b(p_trip->trip_code, species_code,1);
	std::vector<double> _c_male=p_DM->t_lw_coeff.select_lw_coeff_c(p_trip->trip_code, species_code,1);
	std::vector<double> _a_female=p_DM->t_lw_coeff.select_lw_coeff_a(p_trip->trip_code, species_code,2);
	std::vector<double> _b_female=p_DM->t_lw_coeff.select_lw_coeff_b(p_trip->trip_code, species_code,2);
	std::vector<double> _c_female=p_DM->t_lw_coeff.select_lw_coeff_c(p_trip->trip_code, species_code,2);
	std::vector<double> _a_unsexed=p_DM->t_lw_coeff.select_lw_coeff_a(p_trip->trip_code, species_code,3);
	std::vector<double> _b_unsexed=p_DM->t_lw_coeff.select_lw_coeff_b(p_trip->trip_code, species_code,3);
	std::vector<double> _c_unsexed=p_DM->t_lw_coeff.select_lw_coeff_c(p_trip->trip_code, species_code,3);

	if(_a.size()> 0 || _a_male.size()> 0 || _a_female.size()> 0 || _a_unsexed.size()> 0){
		if(_a.size()>0 && (_a_male.size()> 0 || _a_female.size()> 0 || _a_unsexed.size()> 0)){
			fatal("You must provide lw_coeff either for male and female combined (a) or seperately (a_male,a_female,a_unsexed)");
		}
		if(_a.size()>0){
			mean_weights.push_back(Length_based_mean_weight(_a[0],_b[0],_c[0]));
			mean_weights.push_back(Length_based_mean_weight(_a[0],_b[0],_c[0]));
			mean_weights.push_back(Length_based_mean_weight(_a[0],_b[0],_c[0]));
		} else if(_a_male.size()> 0 && _a_female.size()> 0){
			mean_weights.push_back(Length_based_mean_weight(_a_male[0],_b_male[0],_c_male[0]));
			mean_weights.push_back(Length_based_mean_weight(_a_female[0],_b_female[0],_c_female[0]));
			if(_a_unsexed.size()> 0)
				mean_weights.push_back(Length_based_mean_weight(_a_unsexed[0],_b_unsexed[0],_c_unsexed[0]));
			else 
				mean_weights.push_back(Length_based_mean_weight(_a_male[0],_b_male[0],_c_male[0],_a_female[0],_b_female[0],_c_female[0]));
		} else {
			fatal("You must provide lw_coeff for both male (a_male) and female(a_female)");

		}
	}


	if(mean_weights.size()==0){
		std::string temp="calculated";
		if( p_trip->p_survey->task=="calc_biomass_and_LFs" )
			fatal("You must define length_weight relationship for species "+species_code+" trip "+p_trip->trip_code);
		if((p_trip->p_survey->task=="calc_biomass" || p_trip->p_survey->task=="phase_2_calc" || p_trip->p_survey->task=="output_for_catch_at_age")
			&& p_trip->preferences_catch_weight.size()==1 && in(p_trip->preferences_catch_weight,temp))
			fatal("You must define length_weight relationship for species "+species_code+" trip "+p_trip->trip_code);
		if((p_trip->p_survey->task=="calc_biomass" || p_trip->p_survey->task=="phase_2_calc")
			&& p_trip->p_survey->sub_populations_calculated()>0)
			fatal("You must define length_weight relationship for species "+species_code+" trip "+p_trip->trip_code);
		if(p_trip->p_survey->task=="calc_LFs" && p_trip->p_survey->LF_scaling !="unscaled"
			&& p_trip->preferences_catch_weight.size()==1 && in(p_trip->preferences_catch_weight,temp))
			fatal("You must define length_weight relationship for species "+species_code+" trip "+p_trip->trip_code);
	}

	for(int i=0; i<_strata.size();i++)
		strata.push_back(new Stratum(_strata[i],_area_km2[i],(!p_DM->trip_species_stratum_areal_availability.empty())?p_DM->trip_species_stratum_areal_availability[p_trip->trip_code][_species_code][_strata[i]]:D_NULL,
		(!p_DM->trip_species_stratum_population_area.empty())?p_DM->trip_species_stratum_population_area[p_trip->trip_code][_species_code][_strata[i]]:D_NULL,this,p_DM));

	if((p_DM->t_lgth.select_lgth(_p_trip->trip_code,_species_code)).size()==0){
		Lmin=-1;Lmax=-1;
	} else{
		Lmin=min(p_DM->t_lgth.select_lgth(_p_trip->trip_code,_species_code));
		Lmax=max(p_DM->t_lgth.select_lgth(_p_trip->trip_code,_species_code))+1;
	}
	if(p_trip->p_survey->sub_populations_labels.size()>0){
		if(p_trip->p_survey->task!="calc_biomass" && p_trip->p_survey->task!="calc_biomass_and_LFs" && p_trip->p_survey->task!="phase_2_calc"){
			warning("@sub_populations is ignored’ for run-time task "+p_trip->p_survey->task);
		} else {
			if((p_trip->p_survey->sub_populations_labels[0]=="")||in(p_trip->p_survey->sub_populations_labels,species_code)){
				if(p_DM->t_lgth.select_lgth(p_trip->trip_code,species_code).size()==0){
					fatal("You have defined @sub_populations for species "+species_code+" trip "+p_trip->trip_code+", but there is no lgth data");
				}
				std::string command(p_trip->p_survey->sub_populations_labels[0]==""?"sub_populations.":"sub_populations["+species_code+"].");
				std::vector<std::string> labels;
				std::vector<std::string> sexes;
				std::vector<double> _Lmin;
				std::vector<double> _Lmax;

				labels=p_trip->p_survey->p.get_string_vector(command+"labels");
				sexes=p_trip->p_survey->p.get_string_vector(command+"sexes");
				if(sexes.size()!=labels.size())
					fatal("The size of "+command+"sexes must be equal to the size of "+command+"labels");
				for(int i=0;i<sexes.size();i++){
					if(sexes[i]!="all" && sexes[i]!="female" && sexes[i]!="male" && sexes[i]!="unsexed")
						fatal(command+"sexes contains "+sexes[i]+",which is not one of 'all','female','male' and 'unsexed'.");
				}
				if(p_trip->p_survey->p.present(command+"Lmin")){
					_Lmin=p_trip->p_survey->p.get_double_vector(command+"Lmin");
					if(_Lmin.size()!=labels.size())
					fatal("The size of "+command+"Lmin must be equal to the size of "+command+"labels");
					for(int i=0;i<_Lmin.size();i++){
						if(_Lmin[i]<0)
							fatal(command+"Lmin must not be negative");
						if(_Lmin[i]!=int(_Lmin[i]))
							fatal(command+"Lmin must all be integer");
					} 
				} else {
					for(int i=0;i<labels.size();i++){
						_Lmin.push_back(Lmin);
					}
				}
				if(p_trip->p_survey->p.present(command+"Lmax")){
					_Lmax=p_trip->p_survey->p.get_double_vector(command+"Lmax");
					if(_Lmax.size()!=labels.size())
						fatal("The size of "+command+"Lmax must be equal to the size of "+command+"labels");
					for(int i=0;i<_Lmax.size();i++){
						if(_Lmax[i]<0)
							fatal(command+"Lmax must not be negative");
						if(_Lmax[i]!=int(_Lmax[i]))
							fatal(command+"Lmax must all be integer");
					} 
				} else {
					for(int i=0;i<labels.size();i++){
						_Lmax.push_back(Lmax);
					}
				}
				for(int i=0;i<_Lmin.size();i++){
					if(_Lmin[i]>_Lmax[i]) 
						fatal(command+"Lmin must be less than "+command+"Lmax");
				}
				for(int i=0;i<labels.size();i++){
					sub_populations.push_back(Sub_population(labels[i],sexes[i],(int)_Lmin[i],(int)_Lmax[i]));
				}
				p_trip->p_survey->p_DM->species_sub_populatons.insert(make_pair(species_code,labels));
			}
		}
	}

	projected_cvs_requested = false;
	if(p_trip->p_survey->p.present("projected_cvs.strata") || p_trip->p_survey->p.present("projected_cvs.extra_stations")){
		if(p_trip->p_survey->task!="calc_biomass" && p_trip->p_survey->task!="calc_biomass_and_LFs"){
			warning("@projected_cvs is ignored’ for run-time task "+p_trip->p_survey->task);
		} else {
			strata_proj = p_trip->p_survey->p.get_string_vector("projected_cvs.strata");
			n_stations_proj = p_trip->p_survey->p.get_double_vector("projected_cvs.extra_stations");
			if(strata_proj != unique(strata_proj))
				fatal("projected_cvs.strata contains duplicated strata");
			if(strata_proj.size() != n_stations_proj.size())
				fatal("projected_cvs.strata and projected_cv.extra_stationsare not of the same size.");
			for(int i=0;i<strata_proj.size();i++){
				bool exist = false;
				for(int j=0;j<strata.size();j++){
					if(strata_proj[i] == strata[j]->stratum) {
						exist =true; break;
					}
				}
				if(!exist) fatal(strata_proj[i]+" is not a valid stratum");
				if(n_stations_proj[i]<=0 || n_stations_proj[i] != int(n_stations_proj[i] ))
					fatal("projected_cvs.extra_stations must be positive integers");
			}
			projected_cvs_requested = true;
		}
	} 

	UN=0;
	SN = 0;
	SN_prime=0;	
	TN_prime=0;	
	cv_TN_prime =0;

	B=D_NULL;	
	se_B=D_NULL;	
	cv_B=D_NULL;	
	n=D_NULL;	
	m=D_NULL;	
	CF=D_NULL;	

	G=0;
    phase_2_requested=false;
}

void Species::print()
{	
	cerr<<species_code<<endl;
	//for(int i=0;i<mean_weights.size();i++)
		//mean_weights[i].print();
	//for(int i=0;i<sub_populations.size();i++)
		//sub_populations[i].print();

	for(int i=0;i<strata.size();i++)
		strata[i]->print();
}




void Species::calc_LFs_ancillary()
{
	DEBUG0("Species::calc_LFs_ancillary");
	for(int i=0;i<strata.size();i++)
		strata[i]->calc_LFs_ancillary();

	n=0;
	for(int i=0;i<strata.size();i++)
		if(!strata[i]->excluded) n++;

	UN = new dmatrix(1,4,Lmin,Lmax);
	UN->initialize();
	SN = new dmatrix(1,4,Lmin,Lmax);
	SN->initialize();
	anyfish=false;
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded) {
			anyfish=anyfish|strata[i]->anyfish;
		}
	}


	if(p_trip->p_survey->LF_scaling!="unscaled"){	// need to know about catch
		anycatch=false;
		m=0;
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded) {
				anycatch=anycatch|strata[i]->anycatch;
				m+=(!strata[i]->anycatch)|strata[i]->anyfish;
			}
		}
	}

	n_strata_selected=0;n_strata_used=0;n_stations_selected=0;n_stations_used=0;
	for(int i=0;i<strata.size();i++){
		n_strata_selected++;
		if(!strata[i]->excluded) 
			n_strata_used++;
		else 
			strata_not_used.push_back(strata[i]->stratum);

		for(int j=0;j<strata[i]->stations.size();j++){
			if(!strata[i]->excluded){
				if(!strata[i]->stations[j]->excluded)
					n_stations_used++;
				else
					stations_not_used.push_back(strata[i]->stations[j]->station_no);
			} else {
				stations_not_used.push_back(strata[i]->stations[j]->station_no);
			}
			n_stations_selected++;
		}
	}

	
	if(p_trip->p_survey->LF_scaling=="numbers_per_km2" && p_trip->p_survey->type=="trawl" && !p_trip->p_survey->p_DM->catch_kg_km2_supplied){
		mean_d=0;min_d=1e+100;max_d=0;	
		mean_w=0;min_w=1e+100;max_w=0;
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				for(int j=0;j<strata[i]->stations.size();j++){
					if(!strata[i]->stations[j]->excluded){
						mean_d+=strata[i]->stations[j]->d;
						mean_w+=strata[i]->stations[j]->w;
						min_d=min_d<=strata[i]->stations[j]->d? min_d:strata[i]->stations[j]->d;
						min_w=min_w<=strata[i]->stations[j]->w? min_w:strata[i]->stations[j]->w;
						max_d=max_d>=strata[i]->stations[j]->d? max_d:strata[i]->stations[j]->d;
						max_w=max_w>=strata[i]->stations[j]->w? max_w:strata[i]->stations[j]->w;
					}
				}
			}
		}
		mean_d=n_stations_used==0? 0:mean_d/n_stations_used;
		mean_w=n_stations_used==0? 0:mean_w/n_stations_used;
	 }





}

void Species::calc_biomass_ancillary()
{
	DEBUG0("Species::calc_biomass_ancillary");
	for(int i=0;i<strata.size();i++)
		strata[i]->calc_biomass_ancillary();
	n=0;
	for(int i=0;i<strata.size();i++)
		if(!strata[i]->excluded) n++;

	n_strata_selected=0;n_strata_used=0;n_stations_selected=0;n_stations_used=0;
	for(int i=0;i<strata.size();i++){
		n_strata_selected++;
		if(!strata[i]->excluded) 
			n_strata_used++;
		else 
			strata_not_used.push_back(strata[i]->stratum);

		for(int j=0;j<strata[i]->stations.size();j++){
			if(!strata[i]->excluded){
				if(!strata[i]->stations[j]->excluded)
					n_stations_used++;
				else
					stations_not_used.push_back(strata[i]->stations[j]->station_no);
			} else {
				stations_not_used.push_back(strata[i]->stations[j]->station_no);
			}
			n_stations_selected++;
		}
	}

	if(!p_trip->p_survey->p_DM->catch_kg_km2_supplied){
		mean_d=0;min_d=1e+100;max_d=0;	
		mean_w=0;min_w=1e+100;max_w=0;
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				for(int j=0;j<strata[i]->stations.size();j++){
					if(!strata[i]->stations[j]->excluded){
						mean_d+=strata[i]->stations[j]->d;
						mean_w+=strata[i]->stations[j]->w;
						min_d=min_d<=strata[i]->stations[j]->d? min_d:strata[i]->stations[j]->d;
						min_w=min_w<=strata[i]->stations[j]->w? min_w:strata[i]->stations[j]->w;
						max_d=max_d>=strata[i]->stations[j]->d? max_d:strata[i]->stations[j]->d;
						max_w=max_w>=strata[i]->stations[j]->w? max_w:strata[i]->stations[j]->w;
					}
				}
			}
		}
		mean_d=n_stations_used==0? 0:mean_d/n_stations_used;
		mean_w=n_stations_used==0? 0:mean_w/n_stations_used;
	}
 
}

void Species::calc_biomass_and_LFs_ancillary()
{
	DEBUG0("Species::calc_biomass_and_LFs_ancillary");
	for(int i=0;i<strata.size();i++)
		strata[i]->calc_biomass_and_LFs_ancillary();

	n=0;
	for(int i=0;i<strata.size();i++)
		if(!strata[i]->excluded) n++;
	
	UN = new dmatrix(1,4,Lmin,Lmax);
	SN = new dmatrix(1,4,Lmin,Lmax);
	SN_prime = new dmatrix(1,4,Lmin,Lmax);
	TN_prime = new dvector(1,4);	
	cv_TN_prime = new dvector(1,4);

	UN->initialize();
	SN->initialize();
	SN_prime->initialize();
	TN_prime->initialize();
	cv_TN_prime->initialize();

	anyfish=false;
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded) {
			anyfish=anyfish|strata[i]->anyfish;
		}
	}

	if(p_trip->p_survey->LF_scaling!="unscaled"){	// need to know about catch
		anycatch=false;
		m=0;
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded) {
				anycatch=anycatch|strata[i]->anycatch;
				m+=(!strata[i]->anycatch)|strata[i]->anyfish;
			}
		}
	}

	n_strata_selected=0;n_strata_used=0;n_stations_selected=0;n_stations_used=0;
	for(int i=0;i<strata.size();i++){
		n_strata_selected++;
		if(!strata[i]->excluded) 
			n_strata_used++;
		else 
			strata_not_used.push_back(strata[i]->stratum);

		for(int j=0;j<strata[i]->stations.size();j++){
			if(!strata[i]->excluded){
				if(!strata[i]->stations[j]->excluded)
					n_stations_used++;
				else
					stations_not_used.push_back(strata[i]->stations[j]->station_no);
			} else {
				stations_not_used.push_back(strata[i]->stations[j]->station_no);
			}
			n_stations_selected++;
		}
	}

	if(!p_trip->p_survey->p_DM->catch_kg_km2_supplied){	
		mean_d=0;min_d=1e+100;max_d=0;	
		mean_w=0;min_w=1e+100;max_w=0;
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				for(int j=0;j<strata[i]->stations.size();j++){
					if(!strata[i]->stations[j]->excluded){
						mean_d+=strata[i]->stations[j]->d;
						mean_w+=strata[i]->stations[j]->w;
						min_d=min_d<=strata[i]->stations[j]->d? min_d:strata[i]->stations[j]->d;
						min_w=min_w<=strata[i]->stations[j]->w? min_w:strata[i]->stations[j]->w;
						max_d=max_d>=strata[i]->stations[j]->d? max_d:strata[i]->stations[j]->d;
						max_w=max_w>=strata[i]->stations[j]->w? max_w:strata[i]->stations[j]->w;
					}
				}
			}
		}
		mean_d=n_stations_used==0? 0:mean_d/n_stations_used;
		mean_w=n_stations_used==0? 0:mean_w/n_stations_used;
	}
}

void Species::calculate_sub_populations_ancillary()
{
	DEBUG0("Species::calc_biomass_and_LFs_ancillary");
	n_strata_used_sub_population=0;
	n_stations_used_sub_population=0;
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded) {
			strata[i]->calculate_sub_populations_ancillary();
			if(!strata[i]->ex_for_sub_population){
				n_strata_used_sub_population++;
				n_stations_used_sub_population+=strata[i]->n_stations_used;
			}
		}
	}
}

void Species::calc_LFs()
{
	DEBUG0("Species::calc_LFs");
	std::string LF_scaling = p_trip->p_survey->LF_scaling;
	if(anyfish){
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				if(strata[i]->anyfish){
					strata[i]->calc_LFs();
					(*UN)+=*(strata[i]->UN);
					if(LF_scaling=="unscaled"){
						(*SN)+=*(strata[i]->SN);
					} else {
						for(int k=1;k<=3;k++){
							for(int l=(*SN).colmin();l<=(*SN).colmax();l++){
								(*SN)[k][l]+=(*strata[i]->SN)[k][l]/m;
								(*SN)[4][l]+=(*strata[i]->SN)[k][l]/m;
							}
						}
					}
				}
			}
		}
	}

}
void Species::calc_biomass()
{
	DEBUG0("Species::calc_biomass");
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			strata[i]->calc_biomass();
		}
	}

	B=0;
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			B+=strata[i]->B;
		}
		
	}
	se_B=0;
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			se_B+=pow(strata[i]->se_B,2);
		}
	}
	se_B=sqrt(se_B);
	cv_B=B==0?0:se_B/B;

	//Calculated expected cv for B with extra stations;
	if(projected_cvs_requested){
		double se_B_proj(0);
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				int position=pos(strata_proj,strata[i]->stratum);
				se_B_proj += pow(strata[i]->se_B,2)*(strata[i]->n)/(strata[i]->n+(position !=-1? n_stations_proj[position]:0));
			}
		}
		cv_B_proj = B==0? 0:sqrt(se_B_proj)/B;
	}

}
void Species::calc_biomass_and_LFs()
{
	DEBUG0("Species::calc_biomass_and_LFs");
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			strata[i]->calc_biomass_and_LFs();
		}
	}

	B=0;
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			B+=strata[i]->B;
		}
	}
	se_B=0;
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			se_B+=pow(strata[i]->se_B,2);
		}
	}
	se_B=sqrt(se_B);
	cv_B=B==0?0:se_B/B;

	if(anyfish){
		double B_lf=0;
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				if(strata[i]->anyfish){
					(*UN)+=*(strata[i]->UN);
					B_lf+=strata[i]->B;
					for(int k=1;k<=3;k++){
						for(int l=(*SN).colmin();l<=(*SN).colmax();l++)
							(*SN)[k][l]+=(*strata[i]->SN)[k][l];
					}
				}
			}
		}
		double _sum_SN_W=0;
		for(int k=1;k<=3;k++){
			for(int l=(*SN).colmin();l<=(*SN).colmax();l++){
				(*SN)[k][l]=(*SN)[k][l]*B/B_lf;
				(*SN)[4][l]+=(*SN)[k][l]*B/B_lf;
				_sum_SN_W+=(*SN)[k][l]*mean_weights[k-1].get_mean_weight(l);
			}
		}
		
		CF=1000000*B/_sum_SN_W;

		for(int k=1;k<=3;k++){
			for(int l=(*SN_prime).colmin();l<=(*SN_prime).colmax();l++){
				(*SN_prime)[k][l]=(*SN)[k][l]*CF;
				(*SN_prime)[4][l]+=(*SN)[k][l]*CF;
			}
			(*TN_prime)[k]=sum((*SN_prime)[k]);
			(*TN_prime)[4]+=sum((*SN_prime)[k]);
		}

		for(int k=1;k<=4;k++){
			double _sum_se_MTN2_times_K2=0,_sum_MTN_K=0;
			for(int i=0;i<strata.size();i++){
				if(!strata[i]->excluded){
					if(strata[i]->anyfish){
						_sum_se_MTN2_times_K2+=pow((*strata[i]->se_MTN)[k],2)*pow(strata[i]->K,2);
						_sum_MTN_K+=(*strata[i]->MTN)[k]*strata[i]->K;
					}
				}
			}
			(*cv_TN_prime)[k]=_sum_MTN_K ==0? 0:100*sqrt(_sum_se_MTN2_times_K2)/_sum_MTN_K;
		}
	}

	//Calculated expected cv with extra stations;

	//Calculated expected cv with extra stations;
	if(projected_cvs_requested){
		double se_B_proj(0);
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				int position=pos(strata_proj,strata[i]->stratum);
				se_B_proj += pow(strata[i]->se_B,2)*(strata[i]->n)/(strata[i]->n+(position !=-1? n_stations_proj[position]:0));
			}
		}
		cv_B_proj = B==0? 0:sqrt(se_B_proj)/B;
	}

}
void Species::calculate_sub_populations()
{
	DEBUG0("Species::calculate_sub_populations");
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			if(!strata[i]->ex_for_sub_population){
				strata[i]->calculate_sub_populations();
			}
		}
	}

	for(int j=0;j<this->sub_populations.size();j++){
		b.push_back(0);
		se_b.push_back(0);
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				if(!strata[i]->ex_for_sub_population){
					b[j]+=strata[i]->b[j];
					se_b[j]+=pow(strata[i]->se_b[j],2);
				}
			}
		}
		se_b[j]=sqrt(se_b[j]);
		cv_b.push_back(b[j]==0?0:se_b[j]/b[j]);	
	}
	if(projected_cvs_requested){
	   std::vector<double> se_b_proj;
       for(int j=0;j<this->sub_populations.size();j++){
           se_b_proj.push_back(0);
	       for(int i=0;i<strata.size();i++){
		       if(!strata[i]->excluded){
			       if(!strata[i]->ex_for_sub_population){
				       int position=pos(strata_proj,strata[i]->stratum);
				       se_b_proj[j] += pow(strata[i]->se_b[j],2)*(strata[i]->n)/(strata[i]->n+(position !=-1? n_stations_proj[position]:0));
				   }
			   }
		   }
		   cv_b_proj.push_back(b[j]==0? 0:sqrt(se_b_proj[j])/b[j]);
	   }
	}

}


void Species::phase_2_calc(std::string command){
	DEBUG0("Species::phase_2_calc");

	phase_2_requested=true;		//so that it needs output
	int n_stations=p_trip->p_survey->p.get_int(command+"n_stations");
	if(n_stations<=0)
		fatal(command+".n_stations must be a positive integer");
	std::string algorithm=p_trip->p_survey->p.get_string(command+"algorithm","mean_squared");
	if(algorithm!="mean_squared" && algorithm!="variance")
		fatal(command+".algorithm must be either 'mean_squared' or 'variance'");
	G = new dmatrix(1,this->n_strata_used,1,n_stations);
	for(int i=0;i<sub_populations.size();i++)
		g.push_back(dmatrix(1,this->n_strata_used_sub_population,1,n_stations));
	int index_G(0),index_g(0);
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			++index_G;
			for(int j=1;j<=n_stations;j++){
				if(algorithm=="mean_squared")
					(*G)[index_G][j]=pow(strata[i]->a_prime*strata[i]->AD,2)/((strata[i]->n+j-1)*(strata[i]->n+j));
				else
					(*G)[index_G][j]=pow(strata[i]->a_prime*strata[i]->se_AD,2)*strata[i]->n/((strata[i]->n+j-1)*(strata[i]->n+j));
			}
			if(sub_populations.size()>0){
				if(!strata[i]->ex_for_sub_population){
					++index_g;
					for(int j=1;j<=n_stations;j++){
						for(int k=0;k<sub_populations.size();k++){
							if(algorithm=="mean_squared")
								g[k][index_g][j]=pow(strata[i]->a_prime*strata[i]->ad[k],2)/((strata[i]->n+j-1)*(strata[i]->n+j));
							else
								g[k][index_g][j]=pow(strata[i]->a_prime*strata[i]->se_ad[k],2)*strata[i]->n/((strata[i]->n+j-1)*(strata[i]->n+j));
						}
					}
				}
			}
		}
	}
	//normalising
	double max_G1=max(extract_column((*G),1));
	for(int i=1;i<=G->rowmax();i++){
		for(int j=1;j<=G->colmax();j++){
			(*G)[i][j]=100*(*G)[i][j]/max_G1;
		}
	}
	for(int k=0;k<sub_populations.size();k++){
		double max_g1=max(extract_column((g[k]),1));
		for(int i=1;i<=g[k].rowmax();i++){
			for(int j=1;j<=g[k].colmax();j++){
				g[k][i][j]=100*g[k][i][j]/max_g1;
			}
		}
	}
}


void Species::print_diagnostics_biomass(Output_precision& output_precision,ostream& out)
{
	DEBUG0("Species::print_diagnostics_biomass");
	std::string task= p_trip->p_survey->task;
	out<<"* DIAGNOSTICS\n\n";
	for(int i=0;i<strata.size();i++){
		for(int j=0;j<strata[i]->stations.size();j++){
			for(int k=0;k<strata[i]->stations[j]->diagnostics.size();k++){
				out<<strata[i]->stations[j]->diagnostics[k]<<"\n";
			}
			if(strata[i]->stations[j]->diagnostics.size()>0) out<<"\n";
		}
	}
	for(int i=0;i<strata.size();i++){
		for(int k=0;k<strata[i]->diagnostics.size();k++){
			out<<strata[i]->diagnostics[k]<<"\n";
		}
		if(strata[i]->diagnostics.size()>0) out<<"\n";
	}

	out<<"Number of stations selected by user for "+task+": "<<n_stations_selected<<"\n";
	out<<"Number of stations actually used for"+task+": "<<n_stations_used<<"\n";
	if(stations_not_used.size()>0){
		out<<"Stations not used for "+task+": "<<stations_not_used<<"\n";
		out<<"Number of stations not used: "<<stations_not_used.size()<<"\n";
	}
	out<<"\n";

	out<<"Number of strata selected by user for "+task+": "<<n_strata_selected<<"\n";
	out<<"Number of strata actually used for "+task+": "<<n_strata_used<<"\n";
	if(strata_not_used.size()>0){
		out<<"Strata not used for "+task+": "<<strata_not_used<<"\n";
		out<<"Number of strata not used: "<<strata_not_used.size()<<"\n";
	}
	out<<"\n\n";
	if(!p_trip->p_survey->p_DM->catch_kg_km2_supplied){
		//Find maximum field width
		out<<"* AREA SWEPT STATISTICS\n\n";
		int max_width=FMT_double(min_w,"dec_place",2).width();
		max_width = fmax(max_width,FMT_double(max_w,"dec_place",2).width());
		max_width = fmax(max_width,FMT_double(min_d,"dec_place",2).width());
		max_width = fmax(max_width,FMT_double(max_d,"dec_place",2).width());
		max_width = fmax(4,max_width); //4 is the max header width

		ios_base::fmtflags old_settings = out.flags();
		out<<"                  "<<" "<<std::setw(max_width)<<"mean"<<" "<<std::setw(max_width)<<"min"<<" "<<std::setw(max_width)<<"max"<<" "<<"\n";
		out<<"width_swept_m     "<<" ";
		out<<std::setw(max_width)<<FMT_double(mean_w,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(min_w,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(max_w,"dec_place",2)<<" "<<"\n";
		out<<"distance_towed_km "<<" ";
		out<<std::setw(max_width)<<FMT_double(mean_d*1.852,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(min_d*1.852,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(max_d*1.852,"dec_place",2)<<" "<<"\n";
		cout.flags(old_settings);
		out<<"\n\n";
	}
	
}
void Species::print_diagnostics_LFs(Output_precision& output_precision,ostream& out)
{
	DEBUG0("Species::print_diagnostics_LFs");
	std::string task= p_trip->p_survey->task;

	out<<"* DIAGNOSTICS\n\n";
	for(int i=0;i<strata.size();i++){
		for(int j=0;j<strata[i]->stations.size();j++){
			for(int k=0;k<strata[i]->stations[j]->diagnostics.size();k++){
				out<<strata[i]->stations[j]->diagnostics[k]<<"\n";
			}
			if(strata[i]->stations[j]->diagnostics.size()>0) out<<"\n";
		}
	}
	for(int i=0;i<strata.size();i++){
		for(int k=0;k<strata[i]->diagnostics.size();k++){
			out<<strata[i]->diagnostics[k]<<"\n";
		}
		if(strata[i]->diagnostics.size()>0) out<<"\n";
	}

	out<<"Number of stations selected by user for "+task+": "<<n_stations_selected<<"\n";
	out<<"Number of stations actually used for "+task+": "<<n_stations_used<<"\n";
	if(stations_not_used.size()>0){
		out<<"Stations not used for "+task+": "<<stations_not_used<<"\n";
		out<<"Number of stations not used: "<<stations_not_used.size()<<"\n";
	}
	out<<"Number of strata selected by user for "+task+": "<<n_strata_selected<<"\n";
	out<<"Number of strata actually used for "+task+": "<<n_strata_used<<"\n";
	if(strata_not_used.size()>0){
		out<<"Strata not used for "+task+": "<<strata_not_used<<"\n";
		out<<"Number of strata not used: "<<strata_not_used.size()<<"\n";
	}
	out<<"\n\n";
	if(p_trip->p_survey->LF_scaling=="numbers_per_km2" && p_trip->p_survey->type=="trawl" && !p_trip->p_survey->p_DM->catch_kg_km2_supplied){
		out<<"* AREA SWEPT STATISTICS\n\n";
		int max_width=FMT_double(min_w,"dec_place",2).width();
		max_width = fmax(max_width,FMT_double(max_w,"dec_place",2).width());
		max_width = fmax(max_width,FMT_double(min_d,"dec_place",2).width());
		max_width = fmax(max_width,FMT_double(max_d,"dec_place",2).width());
		max_width = fmax(4,max_width); //4 is the max header width

		ios_base::fmtflags old_settings = out.flags();
		out<<"                  "<<" "<<std::setw(max_width)<<"mean"<<" "<<std::setw(max_width)<<"min"<<" "<<std::setw(max_width)<<"max"<<" "<<"\n";
		out<<"width_swept_m     "<<" ";
		out<<std::setw(max_width)<<FMT_double(mean_w,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(min_w,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(max_w,"dec_place",2)<<" "<<"\n";
		out<<"distance_towed_km "<<" ";
		out<<std::setw(max_width)<<FMT_double(mean_d*1.852,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(min_d*1.852,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(max_d*1.852,"dec_place",2)<<" "<<"\n";
		cout.flags(old_settings);
		out<<"\n\n";
	}
	
}

void Species::print_diagnostics_biomass_and_LFs(Output_precision& output_precision,ostream& out)
{
	DEBUG0("Species::print_diagnostics_biomass_and_LFs");
	std::string task= p_trip->p_survey->task;

	out<<"* DIAGNOSTICS\n\n";
	for(int i=0;i<strata.size();i++){
		for(int j=0;j<strata[i]->stations.size();j++){
			for(int k=0;k<strata[i]->stations[j]->diagnostics.size();k++){
				out<<strata[i]->stations[j]->diagnostics[k]<<"\n";
			}
			if(strata[i]->stations[j]->diagnostics.size()>0) out<<"\n";
		}
	}
	for(int i=0;i<strata.size();i++){
		for(int k=0;k<strata[i]->diagnostics.size();k++){
			out<<strata[i]->diagnostics[k]<<"\n";
		}
		if(strata[i]->diagnostics.size()>0) out<<"\n";
	}
	out<<"Number of stations selected by user for "+task+": "<<n_stations_selected<<"\n";
	out<<"Number of stations actually used for "+task+": "<<n_stations_used<<"\n";
	if(stations_not_used.size()>0){
		out<<"Stations not used for "+task+": "<<stations_not_used<<"\n";
		out<<"Number of stations not used: "<<stations_not_used.size()<<"\n";
	}
	out<<"Number of strata selected by user for "+task+": "<<n_strata_selected<<"\n";
	out<<"Number of strata actually used for "+task+": "<<n_strata_used<<"\n";
	if(strata_not_used.size()>0){
		out<<"Strata not used for "+task+": "<<strata_not_used<<"\n";
		out<<"Number of strata not used: "<<strata_not_used.size()<<"\n";
	}
	out<<"\n\n";

	if(task=="calc_biomass_and_LFs"){
		//output correction faction
		out<<"* STRATUM CORRECTION FACTORS\n\n";
		int width_stratum(7),width_CF(17);
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded & strata[i]->CF!=D_NULL){
				width_stratum=fmax(width_stratum,strata[i]->stratum.length());
				width_CF=fmax(width_CF,FMT_double(strata[i]->CF).width());
			}
		}
		width_CF=fmax(width_CF,FMT_double(CF).width());
		out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
		out<<std::setw(width_CF)<<std::setiosflags(ios::right)<<"correction_factor"<<" "<<"\n";
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded & strata[i]->CF!=D_NULL){
				out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<strata[i]->stratum<<" ";
				out<<std::setw(width_CF)<<std::setiosflags(ios::right)<<FMT_double(strata[i]->CF)<<" "<<"\n";
			}
		}
		out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"all"<<" ";
		out<<std::setw(width_CF)<<std::setiosflags(ios::right)<<FMT_double(CF)<<" "<<"\n";
		out<<"\n\n";
	}

	if(!p_trip->p_survey->p_DM->catch_kg_km2_supplied){
		out<<"* AREA SWEPT STATISTICS\n\n";
		int max_width=FMT_double(min_w,output_precision.type["density"],output_precision.precision["density"]).width();
		max_width = fmax(max_width,FMT_double(max_w,output_precision.type["density"],output_precision.precision["density"]).width());
		max_width = fmax(max_width,FMT_double(min_d,output_precision.type["density"],output_precision.precision["density"]).width());
		max_width = fmax(max_width,FMT_double(max_d,output_precision.type["density"],output_precision.precision["density"]).width());
		max_width = fmax(4,max_width); //4 is the max header width

		ios_base::fmtflags old_settings = out.flags();
		out<<"                  "<<" "<<std::setw(max_width)<<"mean"<<" "<<std::setw(max_width)<<"min"<<" "<<std::setw(max_width)<<"max"<<" "<<"\n";
		out<<"width_swept_m     "<<" ";
		out<<std::setw(max_width)<<FMT_double(mean_w,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(min_w,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(max_w,"dec_place",2)<<" "<<"\n";
		out<<"distance_towed_km "<<" ";
		out<<std::setw(max_width)<<FMT_double(mean_d*1.852,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(min_d*1.852,"dec_place",2)<<" ";
		out<<std::setw(max_width)<<FMT_double(max_d*1.852,"dec_place",2)<<" "<<"\n";
		cout.flags(old_settings);
		out<<"\n\n";
	}
}


void Species::print_stratum_summary(Output_precision& output_precision,ostream& out)
{
	DEBUG0("print_stratum_summary");
	out<<"* STRATUM SUMMARY\n\n";
	//column width
	int width_stratum(7),width_area(4),width_Ntows(5),width_Ntowsbio(8),width_kg_km2(6),width_biomass(7),width_cv_biomass(10);
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			width_stratum=fmax(width_stratum,strata[i]->stratum.length());
			width_area=fmax(width_area,FMT_double(strata[i]->a_prime).width());
			width_Ntows=fmax(width_Ntows,FMT_double(strata[i]->n,"",0).width());
			width_Ntowsbio=fmax(width_Ntowsbio,FMT_double(strata[i]->n_stations_catch,"",0).width());
			width_kg_km2=fmax(width_kg_km2,FMT_double(strata[i]->AD,output_precision.type["density"],output_precision.precision["density"]).width());
			width_biomass=fmax(width_biomass,FMT_double(strata[i]->B,output_precision.type["biomass"],output_precision.precision["biomass"]).width());
			width_cv_biomass=fmax(width_cv_biomass,FMT_double(100*strata[i]->cv_B,output_precision.type["cv"],output_precision.precision["cv"]).width());
		}
	}
	
	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
	out<<std::setw(width_area)<<std::setiosflags(ios::right)<<"area"<<" ";
	out<<std::setw(width_Ntows)<<"Ntows"<<" ";
	out<<std::setw(width_Ntowsbio)<<"Ntows"+species_code<<" ";
	out<<std::setw(width_kg_km2)<<"kg/km2"<<" ";
	out<<std::setw(width_biomass)<<"biomass"<<" ";
	out<<std::setw(width_cv_biomass)<<"cv.biomass"<<" "<<"\n";
	//content
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<strata[i]->stratum<<" ";
			out<<std::setw(width_area)<<std::setiosflags(ios::right)<<FMT_double(strata[i]->a_prime)<<" ";
			out<<std::setw(width_Ntows)<<strata[i]->n<<" ";
			out<<std::setw(width_Ntowsbio)<<strata[i]->n_stations_catch<<" ";
			out<<std::setw(width_kg_km2)<<FMT_double(strata[i]->AD,output_precision.type["density"],output_precision.precision["density"])<<" ";
			out<<std::setw(width_biomass)<<FMT_double(strata[i]->B,output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
			out<<std::setw(width_cv_biomass)<<FMT_double(100*strata[i]->cv_B,output_precision.type["cv"],output_precision.precision["cv"])<<" "<<"\n";
		}
	}
	cout.flags(old_settings);	
	out<<"\n";
}
void Species::print_biomass(Output_precision& output_precision,ostream& out)
{
	DEBUG0("print_biomass");
	out<<"* BIOMASS\n\n";
	//column width
	int width_population(10),width_lbound(7),width_biomass(7),width_ubound(7),width_cv(2),width_Nstations(9);
	width_ubound=fmax(width_ubound,FMT_double(B+2*se_B,output_precision.type["biomass"],output_precision.precision["biomass"]).width());
	width_cv=fmax(width_cv,FMT_double(100*cv_B,output_precision.type["cv"],output_precision.precision["cv"]).width());
	for(int i=0;i<sub_populations.size();i++){
		width_population=fmax(width_population,sub_populations[i].label.length());
		width_ubound=fmax(width_ubound,FMT_double(b[i]+2*se_b[i],output_precision.type["biomass"],output_precision.precision["biomass"]).width());
		width_cv=fmax(width_cv,FMT_double(100*cv_b[i],output_precision.type["cv"],output_precision.precision["cv"]).width());
	}
	width_biomass=width_lbound=width_ubound;
	
	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_population)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"population"<<" ";
	out<<std::setw(width_lbound)<<std::setiosflags(ios::right)<<"lbound"<<" ";
	out<<std::setw(width_biomass)<<"biomass"<<" ";
	out<<std::setw(width_ubound)<<"ubound"<<" ";
	out<<std::setw(width_cv)<<"cv"<<" ";
	out<<std::setw(width_Nstations)<<"Nstations"<<" "<<"\n";
	//content
	out<<std::setw(width_population)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"all"<<" ";
	out<<std::setw(width_lbound)<<std::setiosflags(ios::right)<<FMT_double(fmax(0,B-2*se_B),output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
	out<<std::setw(width_biomass)<<FMT_double(B,output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
	out<<std::setw(width_ubound)<<FMT_double(B+2*se_B,output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
	out<<std::setw(width_cv)<<FMT_double(100*cv_B,output_precision.type["cv"],output_precision.precision["cv"])<<" ";
	out<<std::setw(width_Nstations)<<n_stations_used<<" "<<"\n";

	for(int i=0;i<sub_populations.size();i++){
		out<<std::setw(width_population)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<sub_populations[i].label<<" ";
		out<<std::setw(width_lbound)<<std::setiosflags(ios::right)<<FMT_double(fmax(0,b[i]-2*se_b[i]),output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
		out<<std::setw(width_biomass)<<FMT_double(b[i],output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
		out<<std::setw(width_ubound)<<FMT_double(b[i]+2*se_b[i],output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
		out<<std::setw(width_cv)<<FMT_double(100*cv_b[i],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
		out<<std::setw(width_Nstations)<<n_stations_used_sub_population<<" "<<"\n";
	}

	cout.flags(old_settings);	
	out<<"\n";

}
void Species::print_sub_biomass_by_stratum(Output_precision& output_precision,ostream& out)
{
	DEBUG0("print_sub_biomass_by_stratum");
	out<<"* SUB BIOMASS BY STRATUM\n\n";
	//column width
	int width_stratum(7);
	std::vector<int> width_sub_population,width_cv(3);
	for(int i=0;i<sub_populations.size();i++){
		width_sub_population.push_back(0);
		width_cv.push_back(3);
	}
	for(int i=0;i<sub_populations.size();i++){
		width_sub_population[i]=fmax(width_sub_population[i],sub_populations[i].label.length());
		width_cv[i]=width_sub_population[i]+3;
		for(int j=0;j<strata.size();j++){
			if(!strata[j]->excluded){
				if(!strata[j]->ex_for_sub_population){
					if(i==0) width_stratum=fmax(width_stratum,strata[j]->stratum.length());
					width_sub_population[i]=fmax(width_sub_population[i],FMT_double(strata[j]->b[i],output_precision.type["biomass"],output_precision.precision["biomass"]).width());
					width_cv[i]=fmax(width_cv[i],FMT_double(100*strata[j]->cv_b[i],output_precision.type["cv"],output_precision.precision["cv"]).width());
				}
			}
		}
	}
	//header
	ios_base::fmtflags old_settings = out.flags();
	out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios_base::left)<<"stratum"<<" ";
	for(int i=0;i<sub_populations.size();i++){
		out<<std::setw(width_sub_population[i])<<std::setiosflags(ios::right)<<sub_populations[i].label<<" ";
		out<<std::setw(width_cv[i])<<std::setiosflags(ios::right)<<"cv_"+sub_populations[i].label<<" ";
	}
	out<<"\n";
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			if(!strata[i]->ex_for_sub_population){
				out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios_base::left)<<strata[i]->stratum<<" ";
				for(int j=0;j<sub_populations.size();j++){
					out<<std::setw(width_sub_population[j])<<std::setiosflags(ios::right)<<FMT_double(strata[i]->b[j],output_precision.type["biomass"],output_precision.precision["biomass"])<<" ";
					out<<std::setw(width_cv[j])<<std::setiosflags(ios::right)<<FMT_double(100*strata[i]->cv_b[j],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
				}
				out<<"\n";
			}
		}
	}
	cout.flags(old_settings);
	out<<"\n";
}




void Species::print_LF_overall(Output_precision& output_precision,ostream& out)
{
	DEBUG0("Species::print_LF_overall");
	
	out<<"* LF OVERALL\n\n";
	dmatrix* PN = p_trip->p_survey->task=="calc_biomass_and_LFs" ? SN_prime:SN;

	//width
	int width_L(1),width_Nfemale(7),width_Nmale(5),width_Nunsexed(8),width_Nall(4),width_SNfemale(8),width_SNmale(6),width_SNunsexed(9),width_SNall(5);
	width_L=fmax(width_L,FMT_double(SN->colmax(),"",0).width());
	for(int i=Lmin;i<=Lmax;i++){
		width_Nfemale=fmax(width_Nfemale,FMT_double((*UN)[1][i],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
		width_Nmale=fmax(width_Nmale,FMT_double((*UN)[2][i],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
		width_Nunsexed=fmax(width_Nunsexed,FMT_double((*UN)[3][i],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
		width_Nall=fmax(width_Nall,FMT_double((*UN)[4][i],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
		width_SNfemale=fmax(width_SNfemale,FMT_double((*PN)[1][i],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
		width_SNmale=fmax(width_SNmale,FMT_double((*PN)[2][i],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
		width_SNunsexed=fmax(width_SNunsexed,FMT_double((*PN)[3][i],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
		width_SNall=fmax(width_SNall,FMT_double((*PN)[4][i],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
	}


	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_L)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"L"<<" ";
	out<<std::setw(width_Nfemale)<<std::setiosflags(ios::right)<<"Nfemale"<<" ";
	out<<std::setw(width_Nmale)<<"Nmale"<<" ";
	out<<std::setw(width_Nunsexed)<<"Nunsexed"<<" ";
	out<<std::setw(width_Nall)<<"Nall"<<" ";
	out<<std::setw(width_SNfemale)<<"SNfemale"<<" ";
	out<<std::setw(width_SNmale)<<"SNmale"<<" ";
	out<<std::setw(width_SNunsexed)<<"SNunsexed"<<" ";

	out<<std::setw(width_SNall)<<"SNall"<<" "<<endl;
	//content
	for(int i=Lmin;i<=Lmax;i++){
		out<<std::setw(width_L)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<i<<" ";
		out<<std::setw(width_Nfemale)<<std::setiosflags(ios::right)<<FMT_double((*UN)[1][i],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
		out<<std::setw(width_Nmale)<<FMT_double((*UN)[2][i],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
		out<<std::setw(width_Nunsexed)<<FMT_double((*UN)[3][i],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
		out<<std::setw(width_Nall)<<FMT_double((*UN)[4][i],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
		out<<std::setw(width_SNfemale)<<FMT_double((*PN)[1][i],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
		out<<std::setw(width_SNmale)<<FMT_double((*PN)[2][i],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
		out<<std::setw(width_SNunsexed)<<FMT_double((*PN)[3][i],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
		out<<std::setw(width_SNall)<<FMT_double((*PN)[4][i],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" "<<"\n";
	}
	cout.flags(old_settings);
	out<<"\n";
	
}
void Species::print_LFs_by_stratum(Output_precision& output_precision,ostream& out )
{
	DEBUG0("Species::print_LFs_by_stratum");
	out<<"* LFS BY STRATUM\n\n";

	int width_stratum(7),width_L(1),width_Nfemale(7),width_Nmale(5),width_Nunsexed(8),width_Nall(4),width_SNfemale(8),width_SNmale(6),width_SNunsexed(9),width_SNall(5);
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			dmatrix* PN = p_trip->p_survey->task=="calc_biomass_and_LFs" ? strata[i]->SN_prime:strata[i]->SN;
			width_stratum=fmax(width_stratum,strata[i]->stratum.length());
			width_L=fmax(width_L,FMT_double(strata[i]->SN->colmax(),"",0).width());
			for(int j=Lmin;j<=Lmax;j++){
				width_Nfemale=fmax(width_Nfemale,FMT_double((*(strata[i]->UN))[1][j],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
				width_Nmale=fmax(width_Nmale,FMT_double((*(strata[i]->UN))[2][j],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
				width_Nunsexed=fmax(width_Nunsexed,FMT_double((*(strata[i]->UN))[3][j],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
				width_Nall=fmax(width_Nall,FMT_double((*(strata[i]->UN))[4][j],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
				width_SNfemale=fmax(width_SNfemale,FMT_double((*PN)[1][j],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
				width_SNmale=fmax(width_SNmale,FMT_double((*PN)[2][j],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
				width_SNunsexed=fmax(width_SNunsexed,FMT_double((*PN)[3][j],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
				width_SNall=fmax(width_SNall,FMT_double((*PN)[4][j],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
			}
		}
	}
	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_L)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
	out<<std::setw(width_L)<<std::setiosflags(ios::right)<<"L"<<" ";
	out<<std::setw(width_Nfemale)<<std::setiosflags(ios::right)<<"Nfemale"<<" ";
	out<<std::setw(width_Nmale)<<"Nmale"<<" ";
	out<<std::setw(width_Nunsexed)<<"Nunsexed"<<" ";
	out<<std::setw(width_Nall)<<"Nall"<<" ";
	out<<std::setw(width_SNfemale)<<"SNfemale"<<" ";
	out<<std::setw(width_SNmale)<<"SNmale"<<" ";
	out<<std::setw(width_SNunsexed)<<"SNunsexed"<<" ";
	out<<std::setw(width_SNall)<<"SNall"<<" "<<endl;
	//content
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			dmatrix* PN = p_trip->p_survey->task=="calc_biomass_and_LFs" ? strata[i]->SN_prime:strata[i]->SN;
			for(int j=Lmin;j<=Lmax;j++){
				out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<strata[i]->stratum<<" ";
				out<<std::setw(width_L)<<std::setiosflags(ios::right)<<j<<" ";
				out<<std::setw(width_Nfemale)<<FMT_double((*(strata[i]->UN))[1][j],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
				out<<std::setw(width_Nmale)<<FMT_double((*(strata[i]->UN))[2][j],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
				out<<std::setw(width_Nunsexed)<<FMT_double((*(strata[i]->UN))[3][j],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
				out<<std::setw(width_Nall)<<FMT_double((*(strata[i]->UN))[4][j],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
				out<<std::setw(width_SNfemale)<<FMT_double((*PN)[1][j],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
				out<<std::setw(width_SNmale)<<FMT_double((*PN)[2][j],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
				out<<std::setw(width_SNunsexed)<<FMT_double((*PN)[3][j],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
				out<<std::setw(width_SNall)<<FMT_double((*PN)[4][j],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" "<<"\n";
			}
		}
	}
	cout.flags(old_settings);
	out<<"\n";


}
void Species::print_LFs_by_station(Output_precision& output_precision,ostream& out )
{
	DEBUG0("Species::print_LFs_by_station");
	out<<"* LFS BY STATION\n\n";
	int width_stratum(7),width_station(7),width_L(1),width_Nfemale(7),width_Nmale(5),width_Nunsexed(8),width_Nall(4),width_SNfemale(8),width_SNmale(6),width_SNunsexed(9),width_SNall(5);
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			width_stratum=fmax(width_stratum,strata[i]->stratum.length());
			for(int j=0;j<strata[i]->stations.size();j++){
				width_station=fmax(width_station,itos(strata[i]->stations[j]->station_no).length());
				width_L=fmax(width_L,FMT_double(strata[i]->stations[j]->SN->colmax(),"",0).width());
				for(int k=Lmin;k<=Lmax;k++){
					width_Nfemale=fmax(width_Nfemale,FMT_double((*(strata[i]->stations[j]->UN))[1][k],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
					width_Nmale=fmax(width_Nmale,FMT_double((*(strata[i]->stations[j]->UN))[2][k],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
					width_Nunsexed=fmax(width_Nunsexed,FMT_double((*(strata[i]->stations[j]->UN))[3][k],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
					width_Nall=fmax(width_Nall,FMT_double((*(strata[i]->stations[j]->UN))[4][k],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
					width_SNfemale=fmax(width_SNfemale,FMT_double((*(strata[i]->stations[j]->SN))[1][k],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
					width_SNmale=fmax(width_SNmale,FMT_double((*(strata[i]->stations[j]->SN))[2][k],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
					width_SNunsexed=fmax(width_SNunsexed,FMT_double((*(strata[i]->stations[j]->SN))[3][k],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
					width_SNall=fmax(width_SNall,FMT_double((*(strata[i]->stations[j]->SN))[4][k],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
				}
			}
		}
	}
	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_L)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
	out<<std::setw(width_L)<<std::setiosflags(ios::right)<<"station"<<" ";
	out<<std::setw(width_L)<<"L"<<" ";
	out<<std::setw(width_Nfemale)<<std::setiosflags(ios::right)<<"Nfemale"<<" ";
	out<<std::setw(width_Nmale)<<"Nmale"<<" ";
	out<<std::setw(width_Nunsexed)<<"Nunsexed"<<" ";
	out<<std::setw(width_Nall)<<"Nall"<<" ";
	out<<std::setw(width_SNfemale)<<"SNfemale"<<" ";
	out<<std::setw(width_SNmale)<<"SNmale"<<" ";
	out<<std::setw(width_SNunsexed)<<"SNunsexed"<<" ";
	out<<std::setw(width_SNall)<<"SNall"<<" "<<"\n";
	//content
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			for(int j=0;j<strata[i]->stations.size();j++){
				if(!strata[i]->stations[j]->excluded){
					for(int k=Lmin;k<=Lmax;k++){
						out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<strata[i]->stratum<<" ";
						out<<std::setw(width_station)<<std::setiosflags(ios::right)<<strata[i]->stations[j]->station_no<<" ";
						out<<std::setw(width_L)<<k<<" ";
						out<<std::setw(width_Nfemale)<<std::setiosflags(ios::right)<<FMT_double((*(strata[i]->stations[j]->UN))[1][k],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
						out<<std::setw(width_Nmale)<<FMT_double((*(strata[i]->stations[j]->UN))[2][k],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
						out<<std::setw(width_Nunsexed)<<FMT_double((*(strata[i]->stations[j]->UN))[3][k],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
						out<<std::setw(width_Nall)<<FMT_double((*(strata[i]->stations[j]->UN))[4][k],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
						out<<std::setw(width_SNfemale)<<FMT_double((*(strata[i]->stations[j]->SN))[1][k],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
						out<<std::setw(width_SNmale)<<FMT_double((*(strata[i]->stations[j]->SN))[2][k],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
						out<<std::setw(width_SNunsexed)<<FMT_double((*(strata[i]->stations[j]->SN))[3][k],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
						out<<std::setw(width_SNall)<<FMT_double((*(strata[i]->stations[j]->SN))[4][k],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" "<<"\n";
					}
				}
			}
		}
	}
	cout.flags(old_settings);
	out<<"\n";

}
void Species::print_Number_measured(Output_precision& output_precision,ostream& out )
{
	DEBUG0("Species::print_Number_measured");
	out<<"* NUMBER MEASURED\n\n";

	int width_stratum(7),width_Nfemale(7),width_Nmale(5),width_Nunsexed(8),width_Nall(4);
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			width_stratum=fmax(width_stratum,strata[i]->stratum.length());
		}
	}
	width_Nfemale=fmax(width_Nfemale,FMT_double(sum((*UN)[1]),output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
	width_Nmale=fmax(width_Nmale,FMT_double(sum((*UN)[2]),output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
	width_Nunsexed=fmax(width_Nunsexed,FMT_double(sum((*UN)[3]),output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
	width_Nall=fmax(width_Nall,FMT_double(sum((*UN)[4]),output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());

	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
	out<<std::setw(width_Nfemale)<<std::setiosflags(ios::right)<<"Nfemale"<<" ";
	out<<std::setw(width_Nmale)<<"Nmale"<<" ";
	out<<std::setw(width_Nunsexed)<<"Nunsexed"<<" ";
	out<<std::setw(width_Nall)<<"Nall"<<" "<<"\n";
	//content
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<strata[i]->stratum<<" ";
			out<<std::setw(width_Nfemale)<<std::setiosflags(ios::right)<<FMT_double(sum((*(strata[i]->UN))[1]),output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
			out<<std::setw(width_Nmale)<<FMT_double(sum((*(strata[i]->UN))[2]),output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
			out<<std::setw(width_Nunsexed)<<FMT_double(sum((*(strata[i]->UN))[3]),output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
			out<<std::setw(width_Nall)<<FMT_double(sum((*(strata[i]->UN))[4]),output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" "<<"\n";
		}
	}
	out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"All"<<" ";
	out<<std::setw(width_Nfemale)<<std::setiosflags(ios::right)<<FMT_double(sum((*UN)[1]),output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
	out<<std::setw(width_Nmale)<<FMT_double(sum((*UN)[2]),output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
	out<<std::setw(width_Nunsexed)<<FMT_double(sum((*UN)[3]),output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
	out<<std::setw(width_Nall)<<FMT_double(sum((*UN)[4]),output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" "<<"\n";

	cout.flags(old_settings);
	out<<"\n";


}
void Species::print_LF_totals(Output_precision& output_precision,ostream& out )
{
	DEBUG0("Species::print_LF_totals");
	out<<"* LF TOTALS\n\n";

	int width_stratum(7),width_SNfemale(8),width_SNmale(6),width_SNunsexed(9),width_SNall(5),width_cv_female(9),width_cv_male(7),width_cv_unsexed(10),width_cv_all(6);
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			width_stratum=fmax(width_stratum,strata[i]->stratum.length());
		}
		width_cv_female=fmax(width_cv_female,FMT_double((*(strata[i]->cv_TN_prime))[1],output_precision.type["cv"],output_precision.precision["cv"]).width());
		width_cv_male=fmax(width_cv_male,FMT_double((*(strata[i]->cv_TN_prime))[2],output_precision.type["cv"],output_precision.precision["cv"]).width());
		width_cv_unsexed=fmax(width_cv_unsexed,FMT_double((*(strata[i]->cv_TN_prime))[3],output_precision.type["cv"],output_precision.precision["cv"]).width());
		width_cv_all=fmax(width_cv_all,FMT_double((*(strata[i]->cv_TN_prime))[4],output_precision.type["cv"],output_precision.precision["cv"]).width());
	}
	width_SNfemale=fmax(width_SNfemale,FMT_double((*TN_prime)[1],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
	width_SNmale=fmax(width_SNmale,FMT_double((*TN_prime)[2],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
	width_SNunsexed=fmax(width_SNunsexed,FMT_double((*TN_prime)[3],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
	width_SNall=fmax(width_SNall,FMT_double((*TN_prime)[4],output_precision.type["LF_number"],output_precision.precision["LF_number"]).width());
	width_cv_female=fmax(width_cv_female,FMT_double((*cv_TN_prime)[1],output_precision.type["cv"],output_precision.precision["cv"]).width());
	width_cv_male=fmax(width_cv_male,FMT_double((*cv_TN_prime)[2],output_precision.type["cv"],output_precision.precision["cv"]).width());
	width_cv_unsexed=fmax(width_cv_unsexed,FMT_double((*cv_TN_prime)[3],output_precision.type["cv"],output_precision.precision["cv"]).width());
	width_cv_all=fmax(width_cv_all,FMT_double((*cv_TN_prime)[4],output_precision.type["cv"],output_precision.precision["cv"]).width());

	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
	out<<std::setw(width_SNfemale)<<std::setiosflags(ios::right)<<"SNfemale"<<" ";
	out<<std::setw(width_SNmale)<<"SNmale"<<" ";
	out<<std::setw(width_SNunsexed)<<"SNunsexed"<<" ";
	out<<std::setw(width_SNall)<<"SNall"<<" ";
	out<<std::setw(width_cv_female)<<std::setiosflags(ios::right)<<"cv_female"<<" ";
	out<<std::setw(width_cv_male)<<"cv_male"<<" ";
	out<<std::setw(width_cv_unsexed)<<"cv_unsexed"<<" ";
	out<<std::setw(width_cv_all)<<"cv_all"<<" "<<"\n";

	//content
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<strata[i]->stratum<<" ";
			out<<std::setw(width_SNfemale)<<std::setiosflags(ios::right)<<FMT_double((*(strata[i]->TN_prime))[1],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
			out<<std::setw(width_SNmale)<<FMT_double((*(strata[i]->TN_prime))[2],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
			out<<std::setw(width_SNunsexed)<<FMT_double((*(strata[i]->TN_prime))[3],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
			out<<std::setw(width_SNall)<<FMT_double((*(strata[i]->TN_prime))[4],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
			out<<std::setw(width_cv_female)<<std::setiosflags(ios::right)<<FMT_double((*(strata[i]->cv_TN_prime))[1],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
			out<<std::setw(width_cv_male)<<FMT_double((*(strata[i]->cv_TN_prime))[2],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
			out<<std::setw(width_cv_unsexed)<<FMT_double((*(strata[i]->cv_TN_prime))[3],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
			out<<std::setw(width_cv_all)<<FMT_double((*(strata[i]->cv_TN_prime))[4],output_precision.type["cv"],output_precision.precision["cv"])<<" "<<"\n";

		}
	}
	out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"All"<<" ";
	out<<std::setw(width_SNfemale)<<std::setiosflags(ios::right)<<FMT_double((*TN_prime)[1],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
	out<<std::setw(width_SNmale)<<FMT_double((*TN_prime)[2],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
	out<<std::setw(width_SNunsexed)<<FMT_double((*TN_prime)[3],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
	out<<std::setw(width_SNall)<<FMT_double((*TN_prime)[4],output_precision.type["LF_number"],output_precision.precision["LF_number"])<<" ";
	out<<std::setw(width_cv_female)<<std::setiosflags(ios::right)<<FMT_double((*cv_TN_prime)[1],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
	out<<std::setw(width_cv_male)<<FMT_double((*cv_TN_prime)[2],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
	out<<std::setw(width_cv_unsexed)<<FMT_double((*cv_TN_prime)[3],output_precision.type["cv"],output_precision.precision["cv"])<<" ";
	out<<std::setw(width_cv_all)<<FMT_double((*cv_TN_prime)[4],output_precision.type["cv"],output_precision.precision["cv"])<<" "<<"\n";

	cout.flags(old_settings);
	out<<"\n";

}

void Species::print_for_catch_at_age(Output_precision& output_precision,std::string output_for_catch_at_age_file){
	DEBUG0("Species::~print_for_catch_at_age");
	ofstream out(output_for_catch_at_age_file.c_str());
	out<<"SurvCalc output_for_catch_at_age for species "+species_code+" in trip "+p_trip->trip_code+"\n";
	out<<p_trip->trip_code<<"\n";
	out<<n_strata_used<<"\n";
	out<<mean_weights[0].a1<<" "<<mean_weights[0].b1<<" ";
	out<<mean_weights[1].a1<<" "<<mean_weights[1].b1<<" ";
	if(mean_weights[2].a2==D_NULL && mean_weights[2].b2==D_NULL)
		out<<mean_weights[2].a1<<" "<<mean_weights[2].b1<<" ";
	out<<"\n";
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			out<<strata[i]->stratum<<" ";
			out<<strata[i]->area_km2<<" ";
			out<<strata[i]->n_stations_used<<"\n";
		}
	}
	if(p_trip->p_survey->p_DM->t_lgth.subcatch_defined(species_code)){
		out<<"stratum tow subsample kg/km2 subsample_kg/km2 length no_m no_f no_t\n";
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				for(int j=0;j<strata[i]->stations.size();j++){
					if(!strata[i]->stations[j]->excluded){
						for(int k=0;k<strata[i]->stations[j]->L.size();k++){
							out<<strata[i]->stratum<<" ";
							out<<strata[i]->stations[j]->station_no<<" ";
							out<<strata[i]->stations[j]->L_subcatch_no[k]<<" ";
							out<<FMT_double(strata[i]->stations[j]->AD,output_precision.type["density"],output_precision.precision["density"])<<" ";
							if(strata[i]->stations[j]->AD_subcatch[pos(strata[i]->stations[j]->C_subcatch_no,strata[i]->stations[j]->L_subcatch_no[k])] == D_NULL)
								out <<"NULL"<<" ";
							else
								out<<FMT_double(strata[i]->stations[j]->AD_subcatch[pos(strata[i]->stations[j]->C_subcatch_no,strata[i]->stations[j]->L_subcatch_no[k])],output_precision.type["density"],output_precision.precision["density"])<<" ";
							out<<strata[i]->stations[j]->L[k]<<" ";
							out<<strata[i]->stations[j]->N[1][k]<<" ";
							out<<strata[i]->stations[j]->N[0][k]<<" ";
							out<<strata[i]->stations[j]->N[0][k]+strata[i]->stations[j]->N[1][k]+strata[i]->stations[j]->N[2][k]<<"\n";
						}
					}
				}
			}
		}

	} else {
		out<<"stratum tow kg/km2 length no_m no_f no_t\n";
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				for(int j=0;j<strata[i]->stations.size();j++){
					if(!strata[i]->stations[j]->excluded){
						for(int k=0;k<strata[i]->stations[j]->L.size();k++){
							out<<strata[i]->stratum<<" ";
							out<<strata[i]->stations[j]->station_no<<" ";
							out<<FMT_double(strata[i]->stations[j]->AD,output_precision.type["density"],output_precision.precision["density"])<<" ";
							out<<strata[i]->stations[j]->L[k]<<" ";
							out<<strata[i]->stations[j]->N[1][k]<<" ";
							out<<strata[i]->stations[j]->N[0][k]<<" ";
							out<<strata[i]->stations[j]->N[0][k]+strata[i]->stations[j]->N[1][k]+strata[i]->stations[j]->N[2][k]<<"\n";
						}
					}
				}
			}
		}
	}
	out.close();
}

void Species::print_phase_2(Output_precision& output_precision,ostream& out){
	DEBUG0("Species::~print_phase_2");
	int width_stratum(7),width_G(itos(G->colmax()).length()),width_g(itos(G->colmax()).length());
	int index_G(0),index_g(0);
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
			width_stratum=fmax(width_stratum,strata[i]->stratum.length());
			index_G++;
			for(int j=1;j<=G->colmax();j++){
				width_G=fmax(width_G,FMT_double((*G)[index_G][j],output_precision.type["gain"],output_precision.precision["gain"]).width());
			}
			if(sub_populations.size()>0){
				if(!strata[i]->ex_for_sub_population){
					++index_g;
					for(int k=0;k<sub_populations.size();k++){
						for(int j=1;j<=g[k].colmax();j++){
							width_g=fmax(width_g,FMT_double((g)[k][index_g][j],output_precision.type["gain"],output_precision.precision["gain"]).width());
						}
					}
				}
			}
		}
	}
	ios_base::fmtflags old_settings = out.flags();
	//header
	out<<"* Phase-2 gains for dummies\n\n";
	out<<"The following table (or tables) show the relative gains (in terms of reduction in the variance of biomass estimates\n";
	out<<"that would be expected from the addition of phase-2 stations in each stratum. Each table relates to the biomassof a\n";
	out<<"single species, or a sub-populatin of a species\n\n";
	out<<"Instructions for use: To work out the allocation of, say, 10 phase-2 stations that would be optimal for one particular\n";
	out<<"species (or sub-population) circle the 10 largest numbers in the corresponding table. If there are,say, 3 circled numbers\n";
	out<<"in the table for stratum 0016 that means tht 3 phase-2 stations should be allocated to that stratum. If there is any row of\n";
	out<<"the table in which all the numbers are circled then you should increase phase-2.n_station and run SurvCalc again.\n";
	out<<"See User Manual for more details\n\n";
	out<<"* Gains for "+species_code+"\n\n";
	out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
	for(int i=1;i<=G->colmax();i++){
		out<<std::setw(width_G)<<std::setiosflags(ios::right)<<i<<" ";
	}
	out<<"\n\n";
	index_G=0;
	for(int i=0;i<strata.size();i++){
		if(!strata[i]->excluded){
		index_G++;
		out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<strata[i]->stratum<<" ";
		for(int j=1;j<=G->colmax();j++){
			out<<std::setw(width_G)<<std::setiosflags(ios::right)<<FMT_double((*G)[index_G][j],output_precision.type["gain"],output_precision.precision["gain"])<<" ";
		}
		out<<"\n";
		}
	}
	out<<"\n";
	for(int k=0;k<sub_populations.size();k++){
		out<<"* Gains for sub-population "+sub_populations[k].label+" of "+species_code+"\n\n";
		out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<"stratum"<<" ";
		for(int i=1;i<=g[k].colmax();i++){
			out<<std::setw(width_g)<<std::setiosflags(ios::right)<<i<<" ";
		}
		out<<"\n\n";
		index_g=0;
		for(int i=0;i<strata.size();i++){
			if(!strata[i]->excluded){
				if(!strata[i]->ex_for_sub_population){
					index_g++;
					out<<std::setw(width_stratum)<<std::resetiosflags(ios::right)<<std::setiosflags(ios::left)<<strata[i]->stratum<<" ";
					for(int j=1;j<=g[k].colmax();j++){
						out<<std::setw(width_g)<<std::setiosflags(ios::right)<<FMT_double(g[k][index_g][j],output_precision.type["gain"],output_precision.precision["gain"])<<" ";
					}
					out<<"\n";
				}
			}
		}
		out<<"\n";
	}
	cout.flags(old_settings);
	out<<"\n";
}


Species::~Species()
{
	DEBUG0("Species::~Species");
	if(!UN) delete UN;
	if(!SN) delete SN;
	if(!SN_prime) delete SN_prime;
	if(!TN_prime) delete TN_prime;
	if(!cv_TN_prime) delete cv_TN_prime;
	if(!G) delete G;
	for(int i=0;i<strata.size();i++)
		delete strata[i];
	

}
