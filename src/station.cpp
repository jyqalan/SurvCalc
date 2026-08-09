// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* station_cpp_id = "$Id: station.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "survey.h"


Station::Station(int _station_no, double _distance,double _speed,double _dist_doors,double _dist_wings,
			double _lat_s,double _long_s,double _lat_f, double _long_f, 
			std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
			int _time_s,std::string _date_s,int _time_f, std::string _date_f,
			std::vector<double> _weight,double _weight_kg_km2,std::vector<int> _subcatch_no,std::vector<double> _subcatch_weight,
			std::vector<int> _lgth,std::vector<int> _lgth_subcatch_no,std::vector<double> _percent_samp,
			std::vector<int> _no_a,std::vector<int> _no_f,std::vector<int> _no_m,
			double _vulnerability,double _vertical_availability, double _area_fished,
			Stratum* _p_stratum,DataManager* p_DM)
{
	//weight,no_a[i],no_f[i],no_m[i]: NULL converted to 0;
	//lgth[i]: NULL not permitted;
	//subcatch_no,subcatch_weight: The vectors can be empty; 
	//lgth_subcatch_no: The vector can be empty;
	//distance,speed,dist_doors: NULL resulting in an alternative preference, 0 resulting the station excluded;
	//lat_s,long_s,lat_f,long_f: NULL resulting in an alternative preference;

	DEBUG0("Station::Station");
	path="station "+itos(_station_no) +" stratum "+_p_stratum->stratum+" species "+_p_stratum->p_species->species_code+" trip "+_p_stratum->p_species->p_trip->trip_code;
	
	p_stratum=_p_stratum;
	station_no= _station_no;
	distance = _distance;
	speed =_speed;
	dist_doors = _dist_doors;
	dist_wings = _dist_wings;
	lat_s = _lat_s;
	long_s = _long_s;
	lat_f = _lat_f;
	long_f =_long_f;
	nors_s = _nors_s;
	eorw_s =_eorw_s;
	nors_f =_nors_f;
	eorw_f = _eorw_f;
	time_s = _time_s;
	date_s = _date_s;
	time_f = _time_f;
	date_f =_date_f;
	subcatch_no=_subcatch_no;
	subcatch_weight=_subcatch_weight;

	weight=D_NULL;
	for(int i=0;i<_weight.size();i++){
		if(_weight[i]!=D_NULL){
			if(weight==D_NULL) weight=_weight[i];
			else weight=weight+_weight[i];
		}
	}
	weight_kg_km2 = _weight_kg_km2;
	lgth=_lgth;
	lgth_subcatch_no=_lgth_subcatch_no;
	no_a=_no_a;
	no_f=_no_f;
	no_m=_no_m;
	percent_samp=_percent_samp;
	for(int i=0;i<no_a.size();i++){
		no_unsexed.push_back(no_a[i]!=D_NULL?(no_a[i]-(no_f[i]!=D_NULL? no_f[i]:0)-(no_m[i]!=D_NULL? no_m[i]:0)):D_NULL);
	}
	vulnerability = _vulnerability;
	vertical_availability = _vertical_availability; 
	area_fished = _area_fished;

	UN=0;
	SN=0;
	TN=0;
	t=D_NULL;
	d=D_NULL;
	w=D_NULL;
	C=D_NULL;
	AD=D_NULL;
	LD=D_NULL;
	v=D_NULL;
	u_v=D_NULL;
	f=D_NULL;
	excluded = false;

}




void Station::print()
{	
	cerr<<station_no<<endl;
	

}




void Station::calc_LFs_ancillary()
{
	DEBUG0("Station::calc_LFs_ancillary");
	std::string task = p_stratum->p_species->p_trip->p_survey->task;
	if(p_stratum->p_species->Lmin==-1 || p_stratum->p_species->Lmax==-1){
		//fatal(task+": There is no LF data for species "+p_stratum->p_species->species_code+" for trip "+p_stratum->p_species->p_trip->trip_code);
		anyfish=false;
	} else {
		UN = new dmatrix(1,4,p_stratum->p_species->Lmin,p_stratum->p_species->Lmax);
		SN = new dmatrix(1,4,p_stratum->p_species->Lmin,p_stratum->p_species->Lmax);
		TN = new dvector(1,4);
		UN->initialize();
		SN->initialize();
		TN->initialize();
		anyfish=false;
		std::vector<int> _no_f,_no_m,_no_unsexed;
		for(int i=0;i<no_a.size();i++){
			if(no_a[i]!=D_NULL && no_a[i]>0 && percent_samp[i]!=D_NULL && percent_samp[i]>0){
				_no_f.push_back(no_f[i]!=D_NULL?no_f[i]:0);
				_no_m.push_back(no_m[i]!=D_NULL?no_m[i]:0);
				_no_unsexed.push_back(no_unsexed[i]!=D_NULL?no_unsexed[i]:0);
				p.push_back(percent_samp[i]/100);
				L.push_back(lgth[i]);
				anyfish=true;
			}
		}
		if(anyfish){
			N.push_back(_no_f);
			N.push_back(_no_m);
			N.push_back(_no_unsexed);
		}
	}


	if(p_stratum->p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied){
		AD = weight_kg_km2;
		if(AD!=D_NULL && AD!=0) anycatch=true;
		else	anycatch=false;
		if(anyfish && !anycatch) {
			excluded=true; 
			diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":fish were measured but catch weight is zero");	
		}
		if(!anyfish && anycatch){
			diagnostics.push_back("Missing LF data for Station "+itos(station_no)+".");
		}
	} else {
		if(p_stratum->p_species->p_trip->p_survey->LF_scaling!="unscaled"){	// need to know about catch
			bool succeed=false;
			anycatch=false;
			for(int i=0;i<p_stratum->p_species->p_trip->preferences_catch_weight.size();i++){
				if(p_stratum->p_species->p_trip->preferences_catch_weight[i]=="recorded"){
					C=weight;
					succeed=true;
					if(C!=D_NULL && C!=0) anycatch=true; 
					else	anycatch=false;
				} else if(p_stratum->p_species->p_trip->preferences_catch_weight[i]=="calculated"){
				// need length_weight relationship first
					if(anyfish &&  p_stratum->p_species->mean_weights.size()>0){
						for(int j=0;j<p_stratum->p_species->mean_weights.size();j++){
							std::vector<double> W_by_sex;
							for(int k=0;k<L.size();k++)
								W_by_sex.push_back(p_stratum->p_species->mean_weights[j].get_mean_weight(L[k]));
							W.push_back(W_by_sex);
						}
						C=0;
						for(int j=0;j<N.size();j++){
							for(int k=0;k<N[j].size();k++){
								succeed=true;
								C+=0.001*N[j][k]*W[j][k]/p[k];
							}
						}
						if(succeed){
							if(C!=0) anycatch=true; 
							else	anycatch=false;
						} 
					}
				}
				if(succeed){
					if(i!=0) diagnostics.push_back("Catch weight determined using "+itos(i+1)+(i==1?"nd ":"rd ")+"preference for Station "+itos(station_no));
					break;
				} else {
					diagnostics.push_back("Can't get catch weight using " +p_stratum->p_species->p_trip->preferences_catch_weight[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_catch_weight.size()-1?".":". Trying next preference..."));
				}
			}
			if(!succeed){
				excluded=true; 
				diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No catch weight");
			} else {
				if(anyfish && !anycatch) {
					excluded=true; 
					diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":fish were measured but catch weight is zero");
				}
				if(!anyfish && anycatch){
					diagnostics.push_back("Missing LF data for Station "+itos(station_no)+".");
				}
			}
		}
		if(p_stratum->p_species->p_trip->p_survey->LF_scaling=="numbers_per_km2"){
			if(p_stratum->p_species->p_trip->p_survey->type=="trawl"){
				bool succeed=false;	// calculate d;
				for(int i=0;i<p_stratum->p_species->p_trip->preferences_distance_towed.size();i++){			
					if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="recorded_distance"){
						if(distance!=D_NULL){
							d=distance;
							if(d==0){
								diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":distance is zero");
								excluded=true; 
							} else {
								succeed=true;
							}
						}
					} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="recorded_speed*time"){
						t=get_time(time_s,date_s,time_f,date_f);
						if(speed !=D_NULL && t!=D_NULL){
							d=speed*t;
							if(d==0){
								excluded=true; 
								diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":distance is zero");
							} else {
								succeed=true;
							}
						}
					} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="constant_speed*time"){
						t=get_time(time_s,date_s,time_f,date_f);
						if(p_stratum->p_species->p_trip->constant_speed!=D_NULL && t!=D_NULL){
							d=p_stratum->p_species->p_trip->constant_speed*t;
							if(d==0){
								excluded=true; 
								diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":distance is zero");
							} else {
								succeed=true;
							}
						}
					} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="from_lat_long"){
						d=get_distance_from_lat_long(nors_s,eorw_s,nors_f,eorw_f,lat_s,long_s,lat_f,long_f);
						if(d!=D_NULL){
							if(d==0){
								excluded=true; 
								diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":distance is zero");
							} else {
								succeed=true;
							}
						}
					}					
					if(succeed){
						if(i!=0) diagnostics.push_back("Distance towed determined using "+itos(i+1)+(i==1?"nd ":"rd ")+ "preference for Station "+itos(station_no));
						break;
					} else {
						diagnostics.push_back("Can't get distance using " +p_stratum->p_species->p_trip->preferences_distance_towed[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_distance_towed.size()-1?".":". Trying next preference..."));
					}
				}
				if(!succeed){ 
					excluded=true; 
					diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No distance towed");
				}
				succeed=false;	// calculate w;
				for(int i=0;i<p_stratum->p_species->p_trip->preferences_width_swept.size();i++){			
					if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="recorded_doorspread"){
						if(dist_doors!=D_NULL){
							w=dist_doors;
							if(w==0){
								diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": width is zero");
								excluded=true; 
							} else {
								succeed=true;
							}
						}
					} if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="recorded_wingspread"){
						if(dist_wings!=D_NULL){
							w=dist_wings;
							if(w==0){
								diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": width is zero");
								excluded=true; 
							} else {
								succeed=true;
							}
						}
					} else if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="constant_doorspread"){
						w=p_stratum->p_species->p_trip->constant_doorspread;
						succeed=true;
					}
					if(succeed){
						if(i!=0) diagnostics.push_back("Width swepth determined using "+itos(i+1)+(i==1?"nd ":"rd ")+ "preference for Station "+itos(station_no));
						break;
					} else {
						diagnostics.push_back("Can't get width swepth using "+ p_stratum->p_species->p_trip->preferences_width_swept[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_width_swept.size()-1?".":". Trying next preference..."));
					}
				}
				if(!succeed){ 
					excluded=true; 
					diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No width swept");
				}
				v=vulnerability!=D_NULL? vulnerability:1;
				u_v=vertical_availability!=D_NULL?vertical_availability:1;
				f= (d!=D_NULL && w!=D_NULL)? 1852*d*w*v*u_v:D_NULL;
			} else {	//for "pot"
				f=area_fished;
			}
			AD=(C!=D_NULL && f!=D_NULL && f!=0)? 1000000*C/f:D_NULL;
			LD=(C!=D_NULL && d!=D_NULL && d!=0)? 1000*C/(1.852*d*v*u_v):D_NULL;
		} else if(p_stratum->p_species->p_trip->p_survey->LF_scaling=="numbers_per_hour"){
			t=get_time(time_s,date_s,time_f,date_f);
			if(t!=D_NULL){
				if(t==0){
					excluded=true; 
					diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": time is zero");
				}
			} else {
				excluded=true;
				diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No time");
			}
		}
	}
}





void Station::calc_biomass_ancillary()
{
	DEBUG0("Station::calc_biomass_ancillary");
	std::string task = p_stratum->p_species->p_trip->p_survey->task;
	anyfish=false;
	std::vector<int> _no_f,_no_m,_no_unsexed;
	for(int i=0;i<no_a.size();i++){
		if(no_a[i]!=D_NULL && no_a[i]>0 && percent_samp[i]!=D_NULL && percent_samp[i]>0){
			_no_f.push_back(no_f[i]!=D_NULL?no_f[i]:0);
			_no_m.push_back(no_m[i]!=D_NULL?no_m[i]:0);
			_no_unsexed.push_back(no_unsexed[i]!=D_NULL?no_unsexed[i]:0);
			p.push_back(percent_samp[i]/100);
			L.push_back(lgth[i]);
			anyfish=true;
		}
	}
	if(anyfish){
		N.push_back(_no_f);
		N.push_back(_no_m);
		N.push_back(_no_unsexed);
	}

	if(p_stratum->p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied){
		AD = weight_kg_km2;
		if(AD!=D_NULL && AD!=0) anycatch=true;
		else	anycatch=false;
		if(anyfish && !anycatch) {
			excluded=true; 
			diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":fish were measured but catch weight is zero");		
		}
	} else {
		if(anyfish &&  p_stratum->p_species->mean_weights.size()>0){
			for(int j=0;j<p_stratum->p_species->mean_weights.size();j++){
				std::vector<double> W_by_sex;
				for(int k=0;k<L.size();k++)
					W_by_sex.push_back(p_stratum->p_species->mean_weights[j].get_mean_weight(L[k]));
				W.push_back(W_by_sex);
			}
		}
		bool succeed=false;
		anycatch=false;
		for(int i=0;i<p_stratum->p_species->p_trip->preferences_catch_weight.size();i++){
			if(p_stratum->p_species->p_trip->preferences_catch_weight[i]=="recorded"){
				C=weight;
				succeed=true;
				if(C!=D_NULL && C!=0) anycatch=true; 
				else	anycatch=false;
			} else if(p_stratum->p_species->p_trip->preferences_catch_weight[i]=="calculated"){
				// need length_weight relationship first
				if(anyfish &&  p_stratum->p_species->mean_weights.size()>0){
					C=0;
					for(int j=0;j<N.size();j++){
						for(int k=0;k<N[j].size();k++){
							succeed=true;
							C+=0.001*N[j][k]*W[j][k]/p[k];
						}
					}
					if(succeed){
						if(C!=0) anycatch=true; 
						else	anycatch=false;
					}
				}
			}
			if(succeed){
				if(i!=0) diagnostics.push_back("Catch weight determined using "+itos(i+1)+(i==1?"nd ":"rd ")+"preference for Station "+itos(station_no));
				break;
			} else {
				diagnostics.push_back("Can't get catch weight using " +p_stratum->p_species->p_trip->preferences_catch_weight[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_catch_weight.size()-1?".":". Trying next preference..."));
			}
		}
		if(!succeed){ 
			excluded=true; 
			diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No catch weight");
		} else {
			if(anyfish && !anycatch) {
				excluded=true; 
				diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":fish were measured but catch weight is zero");
			}
		}
		if(p_stratum->p_species->p_trip->p_survey->type=="trawl"){
			bool succeed=false;	// calculate d;
			for(int i=0;i<p_stratum->p_species->p_trip->preferences_distance_towed.size();i++){			
				if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="recorded_distance"){
					if(distance!=D_NULL){
						d=distance;
						if(d==0){
							excluded=true;
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":distance is zero");
						} else {
							succeed=true;
						}
					}
				} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="recorded_speed*time"){
					t=get_time(time_s,date_s,time_f,date_f);
					if(speed !=D_NULL && t!=D_NULL){
						d=speed*t;
						if(d==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":distance is zero");
						} else {
							succeed=true;
						}
					}
				} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="constant_speed*time"){
					t=get_time(time_s,date_s,time_f,date_f);
					if(p_stratum->p_species->p_trip->constant_speed!=D_NULL && t!=D_NULL){
						d=p_stratum->p_species->p_trip->constant_speed*t;
						if(d==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":distance is zero");
						} else {
							succeed=true;
						}
					}
				} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="from_lat_long"){
					d=get_distance_from_lat_long(nors_s,eorw_s,nors_f,eorw_f,lat_s,long_s,lat_f,long_f);
					if(d!=D_NULL){
						if(d==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":distance is zero");
						} else {
							succeed=true;
						}
					}
				}
				if(succeed){
					if(i!=0) diagnostics.push_back("Distance towed determined using "+itos(i+1)+(i==1?"nd ":"rd ")+ "preference for Station "+itos(station_no));
					break;
				} else {
					diagnostics.push_back("Can't get distance using " +p_stratum->p_species->p_trip->preferences_distance_towed[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_distance_towed.size()-1?".":". Trying next preference..."));
				}
			}
			if(!succeed){ 
				excluded=true; 
				diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No distance towed");
			}
			succeed=false;	// calculate w;
			for(int i=0;i<p_stratum->p_species->p_trip->preferences_width_swept.size();i++){			
				if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="recorded_doorspread"){
					if(dist_doors!=D_NULL){
						w=dist_doors;
						if(w==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": width is zero");
						} else {
							succeed=true;
						}
					}
				} if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="recorded_wingspread"){
						if(dist_wings!=D_NULL){
							w=dist_wings;
							if(w==0){
								diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": width is zero");
								excluded=true; 
							} else {
								succeed=true;
							}
						}
				} else if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="constant_doorspread"){
					w=p_stratum->p_species->p_trip->constant_doorspread;
					succeed=true;
				}
				if(succeed){
					if(i!=0) diagnostics.push_back("Width swepth determined using "+itos(i+1)+(i==1?"nd ":"rd ")+ "preference for Station "+itos(station_no));
					break;
				} else {
					diagnostics.push_back("Can't get width swepth using "+ p_stratum->p_species->p_trip->preferences_width_swept[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_width_swept.size()-1?".":". Trying next preference..."));
				}
			}
			if(!succeed){ 
				excluded=true; 
				diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No width swept");
			}
			v=vulnerability!=D_NULL? vulnerability:1;
			u_v=vertical_availability!=D_NULL?vertical_availability:1;
			f= (d!=D_NULL && w!=D_NULL)? 1852*d*w*v*u_v:D_NULL;
		} else {	//for "pot"
			f=area_fished;
		}
		AD=(C!=D_NULL && f!=D_NULL && f!=0)? 1000000*C/f:D_NULL;
		LD=(C!=D_NULL && d!=D_NULL && d!=0)? 1000*C/(1.852*d*v*u_v):D_NULL;
	}
}

void Station::calc_biomass_and_LFs_ancillary()
{
	DEBUG0("Station::calc_biomass_and_LFs_ancillary");
	std::string task = p_stratum->p_species->p_trip->p_survey->task;
	if(p_stratum->p_species->Lmin==-1 || p_stratum->p_species->Lmax==-1){
		;//fatal(task+": There is no LF data for species "+p_stratum->p_species->species_code+" for trip "+p_stratum->p_species->p_trip->trip_code);
		anyfish=false;
	} else {
		UN = new dmatrix(1,4,p_stratum->p_species->Lmin,p_stratum->p_species->Lmax);
		SN = new dmatrix(1,4,p_stratum->p_species->Lmin,p_stratum->p_species->Lmax);
		TN = new dvector(1,4);
		UN->initialize();
		SN->initialize();
		TN->initialize();
		anyfish=false;
		std::vector<int> _no_f,_no_m,_no_unsexed;
		for(int i=0;i<no_a.size();i++){
			if(no_a[i]!=D_NULL && no_a[i]>0 && percent_samp[i]!=D_NULL && percent_samp[i]>0){
				_no_f.push_back(no_f[i]!=D_NULL?no_f[i]:0);
				_no_m.push_back(no_m[i]!=D_NULL?no_m[i]:0);
				_no_unsexed.push_back(no_unsexed[i]!=D_NULL?no_unsexed[i]:0);
				p.push_back(percent_samp[i]/100);
				L.push_back(lgth[i]);
				anyfish=true;
				if(task=="output_for_catch_at_age" && p_stratum->p_species->p_trip->p_survey->p_DM->t_lgth.subcatch_defined()){
					L_subcatch_no.push_back(lgth_subcatch_no[i]);
					if(!in(C_subcatch_no,lgth_subcatch_no[i])) C_subcatch_no.push_back(lgth_subcatch_no[i]);
				}
			}
		}
		if(anyfish){
			N.push_back(_no_f);
			N.push_back(_no_m);
			N.push_back(_no_unsexed);
		}
	}

	if(p_stratum->p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied){
		AD = weight_kg_km2;
		if(AD!=D_NULL && AD!=0) anycatch=true;
		else	anycatch=false;
		if(anyfish && !anycatch) {
			excluded=true; 
			diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":fish were measured but catch weight is zero");	
		}
		if(!anyfish && anycatch){
			if(task=="calc_biomass_and_LFs")
				diagnostics.push_back("Missing LF data for Station "+itos(station_no)+". Stratum LF will be scaled to stratum biomass");
		}
	} else {
		if(anyfish &&  p_stratum->p_species->mean_weights.size()>0){
			for(int j=0;j<p_stratum->p_species->mean_weights.size();j++){
				std::vector<double> W_by_sex;
				for(int k=0;k<L.size();k++)
					W_by_sex.push_back(p_stratum->p_species->mean_weights[j].get_mean_weight(L[k]));
					W.push_back(W_by_sex);
				}
		}
		bool succeed=false;
		anycatch=false;
		for(int i=0;i<p_stratum->p_species->p_trip->preferences_catch_weight.size();i++){
			if(p_stratum->p_species->p_trip->preferences_catch_weight[i]=="recorded"){
				C=weight;
				succeed=true;
				if(C!=D_NULL && C!=0) anycatch=true; 
				else	anycatch=false;
			} else if(p_stratum->p_species->p_trip->preferences_catch_weight[i]=="calculated"){
				// need length_weight relationship first
				if(anyfish &&  p_stratum->p_species->mean_weights.size()>0){
					C=0;
					for(int j=0;j<N.size();j++){
						for(int k=0;k<N[j].size();k++){
							succeed=true;
							C+=0.001*N[j][k]*W[j][k]/p[k];
						}
					}
					if(succeed){
						if(C!=0) anycatch=true; 
						else	anycatch=false;
					}

				}

			}
			if(succeed){
				if(i!=0) diagnostics.push_back("Catch weight determined using "+itos(i+1)+(i==1?"nd ":"rd ")+"preference for Station "+itos(station_no));
				break;
			} else {
				diagnostics.push_back("Can't get catch weight using " +p_stratum->p_species->p_trip->preferences_catch_weight[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_catch_weight.size()-1?".":". Trying next preference..."));
			}
		}
		if(!succeed){ 
			excluded=true;	
			diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": No catch weight");
		} else {
			if(anyfish && !anycatch) {
				excluded=true; 
				diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": fish were measured but catch weight is zero");
			}
			if(!anyfish && anycatch){
				if(task=="calc_biomass_and_LFs")
					diagnostics.push_back("Missing LF data for Station "+itos(station_no)+". Stratum LF will be scaled to stratum biomass");
			}
		}
		if(task=="output_for_catch_at_age" && p_stratum->p_species->p_trip->p_survey->p_DM->t_lgth.subcatch_defined()){
			//Here I assume  all subcatches are either recorded or calcuated. It cann't be that one is recorded and the other is calculated
			for(int i=0;i<p_stratum->p_species->p_trip->preferences_catch_weight.size();i++){
				if(p_stratum->p_species->p_trip->preferences_catch_weight[i]=="recorded"){
					for(int j=0;j<C_subcatch_no.size();j++){
						C_subcatch.push_back(D_NULL);
						for(int k=0; k<subcatch_no.size();k++){
							if(subcatch_no[k]==C_subcatch_no[j] && subcatch_weight[k]!=D_NULL){
								if(C_subcatch[j]==D_NULL) C_subcatch[j] = subcatch_weight[k];
								else C_subcatch[j] = C_subcatch[j]+subcatch_weight[k];
							}
						}
					}
					/* Assume subcatch_no are unique
					int position=pos(subcatch_no,C_subcatch_no[j]);
					if(position !=-1) {
						C_subcatch.push_back(subcatch_weight[position]);
					} else {
						C_subcatch.push_back(D_NULL);
					}
					*/ 
					break;
				} else if(p_stratum->p_species->p_trip->preferences_catch_weight[i]=="calculated"){
					if(anyfish &&  p_stratum->p_species->mean_weights.size()>0){
						for(int l=0;l<C_subcatch_no.size();l++){
							C_subcatch.push_back(0);
							for(int j=0;j<N.size();j++){
								for(int k=0;k<N[j].size();k++){
									if(L_subcatch_no[k]==C_subcatch_no[l]){
										C_subcatch[l]+=0.001*N[j][k]*W[j][k]/p[k];
									}
								}
							}
						}
						break;
					}
				}
			}
		}
		if(p_stratum->p_species->p_trip->p_survey->type=="trawl"){
			bool succeed=false;	// calculate d;
			for(int i=0;i<p_stratum->p_species->p_trip->preferences_distance_towed.size();i++){			
				if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="recorded_distance"){
					if(distance!=D_NULL){
						d=distance;
						if(d==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": distance is zero");
						} else {
							succeed=true;
						}
					}
				} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="recorded_speed*time"){
					t=get_time(time_s,date_s,time_f,date_f);
					if(speed !=D_NULL && t!=D_NULL){
						d=speed*t;
						if(d==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": distance is zero");
						} else {
							succeed=true;
						}
					}
				} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="constant_speed*time"){
					t=get_time(time_s,date_s,time_f,date_f);
					if(p_stratum->p_species->p_trip->constant_speed!=D_NULL && t!=D_NULL){
						d=p_stratum->p_species->p_trip->constant_speed*t;
						if(d==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN BIOMASS AND LF ESTIMATES: distance is zero");
						} else {
							succeed=true;
						}
					}
				} else if(p_stratum->p_species->p_trip->preferences_distance_towed[i]=="from_lat_long"){
					d=get_distance_from_lat_long(nors_s,eorw_s,nors_f,eorw_f,lat_s,long_s,lat_f,long_f);
					if(d!=D_NULL){
						if(d==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": distance is zero");
						} else {
							succeed=true;
						}
					}
				}
				if(succeed){
					if(i!=0) diagnostics.push_back("Distance towed determined using "+itos(i+1)+(i==1?"nd ":"rd ")+ "preference for Station "+itos(station_no));
					break;
				} else {
					diagnostics.push_back("Can't get distance using " +p_stratum->p_species->p_trip->preferences_distance_towed[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_distance_towed.size()-1?".":". Trying next preference..."));
				}
			}
			if(!succeed){ 
				excluded=true; 
				diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No distance towed");
			}
			succeed=false;	// calculate w;
			for(int i=0;i<p_stratum->p_species->p_trip->preferences_width_swept.size();i++){			
				if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="recorded_doorspread"){
					if(dist_doors!=D_NULL){
						w=dist_doors;
						if(w==0){
							excluded=true; 
							diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": width is zero");
						} else {
							succeed=true;
						}
					}
				} if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="recorded_wingspread"){
						if(dist_wings!=D_NULL){
							w=dist_wings;
							if(w==0){
								diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+": width is zero");
								excluded=true; 
							} else {
								succeed=true;
							}
						}
				} else if(p_stratum->p_species->p_trip->preferences_width_swept[i]=="constant_doorspread"){
					w=p_stratum->p_species->p_trip->constant_doorspread;
					succeed=true;
				}
				if(succeed){
					if(i!=0) diagnostics.push_back("Width swepth determined using "+itos(i+1)+(i==1?"nd ":"rd ")+ "preference for Station "+itos(station_no));
					break;
				} else {
					diagnostics.push_back("Can't get width swepth using "+ p_stratum->p_species->p_trip->preferences_width_swept[i]+"' preference for Station "+itos(station_no)+(i==p_stratum->p_species->p_trip->preferences_width_swept.size()-1?".":". Trying next preference..."));
				}
			}
			if(!succeed){ 
				excluded=true; 
				diagnostics.push_back("Station "+itos(station_no)+" NOT INCLUDED IN "+task+":No width swept");
			}		
			v=vulnerability!=D_NULL? vulnerability:1;
			u_v=vertical_availability!=D_NULL?vertical_availability:1;
			f= (d!=D_NULL && w!=D_NULL)? 1852*d*w*v*u_v:D_NULL;
		} else {	//for "pot"
			f=area_fished;
		}
		AD=(C!=D_NULL && f!=D_NULL && f!=0)? 1000000*C/f:D_NULL;
		LD=(C!=D_NULL && d!=D_NULL && d!=0)? 1000*C/(1.852*d*v*u_v):D_NULL;
		if(task=="output_for_catch_at_age" && p_stratum->p_species->p_trip->p_survey->p_DM->t_lgth.subcatch_defined()){
			for(int i=0;i<C_subcatch_no.size();i++){
				AD_subcatch.push_back(C_subcatch[i]!=D_NULL && f!=D_NULL && f!=0?1000000*C_subcatch[i]/f:D_NULL);
			}
		}
	}
}

void Station::calculate_sub_populations_ancillary()
{
	DEBUG0("Station::calculate_sub_populations_ancillary");
	if(anyfish){
		for(int i=0;i<p_stratum->p_species->sub_populations.size();i++){
			double _c_sum=0,_C_sum=0;
			for(int j=0;j<N.size();j++){
				for(int k=0;k<N[j].size();k++){
					_C_sum+=N[j][k]*W[j][k]/p[k];
					if(p_stratum->p_species->sub_populations[i].in_sub_population(j,L[k]))
						_c_sum+=N[j][k]*W[j][k]/p[k];
				}
			}
			c.push_back(C*_c_sum/_C_sum);	//_C_sum cannot be zero;
			ad.push_back(1000000*(C*_c_sum/_C_sum)/f);
		}
	} else {
		if(anycatch){
		    diagnostics.push_back("Missing LFs in station "+itos(station_no)+". Catch weights of sub-populations will be estimated.");
			for(int i=0;i<p_stratum->p_species->sub_populations.size();i++){
				double _ad_sum=0,_AD_sum=0;
				for(int j=0;j<p_stratum->stations.size();j++){
					if(!p_stratum->stations[j]->excluded && p_stratum->stations[j]->anyfish){
						_ad_sum+=p_stratum->stations[j]->ad[i];
						_AD_sum+=p_stratum->stations[j]->AD;
					}
				}
				c.push_back(C != D_NULL && _AD_sum!=0? C*_ad_sum/_AD_sum : 0);	//_AD_sum cannot be zero because stratum for which all stations has no LF (anyfish==false) are excluded; but there maybe no catch
				ad.push_back(C != D_NULL && f!= D_NULL && _AD_sum!=0 && f!=0 ? 1000000*(C*_ad_sum/_AD_sum)/f :0);
			}
		} else {
			for(int i=0;i<p_stratum->p_species->sub_populations.size();i++){
				c.push_back(0);
				ad.push_back(0);
			}
		}
	}
}

void Station::calc_LFs()
{
	DEBUG0("Station::calc_LFs");
	if(anyfish){
		for(int k=1;k<=3;k++){
			for(int j=0;j<L.size();j++){
				(*UN)[k][L[j]]+=N[k-1][j];
				(*UN)[4][L[j]]+=N[k-1][j];
			}
		}
		std::string LF_scaling = p_stratum->p_species->p_trip->p_survey->LF_scaling;
		if(LF_scaling=="unscaled"){
			for(int k=1;k<=3;k++){
				for(int j=0;j<L.size();j++){
					(*SN)[k][L[j]]+=N[k-1][j];
					(*SN)[4][L[j]]+=N[k-1][j];
				}
				(*TN)[k]=sum((*SN)[k]);
				(*TN)[4]+=sum((*SN)[k]);
			}
		} else if(LF_scaling=="numbers_per_1000") {
			for(int k=1;k<=3;k++){
				std::vector<double> N_k_times_1_over_p;
				for(int j=0;j<L.size();j++)
					N_k_times_1_over_p.push_back(N[k-1][j]/p[j]);
				double _sum=sum(N_k_times_1_over_p);
				for(int j=0;j<L.size();j++){
					(*SN)[k][L[j]]+=1000*N_k_times_1_over_p[j]/_sum;
					(*SN)[4][L[j]]+=1000*N_k_times_1_over_p[j]/_sum;
				}
				(*TN)[k]=sum((*SN)[k]);
				(*TN)[4]+=sum((*SN)[k]);
			}
		} else if(LF_scaling=="numbers_per_tow") {
			for(int k=1;k<=3;k++){
				for(int j=0;j<L.size();j++){
					(*SN)[k][L[j]]+=N[k-1][j]/p[j];
					(*SN)[4][L[j]]+=N[k-1][j]/p[j];
				}
				(*TN)[k]=sum((*SN)[k]);
				(*TN)[4]+=sum((*SN)[k]);
			}
		} else if(LF_scaling=="numbers_per_hour"){
			for(int k=1;k<=3;k++){
				for(int j=0;j<L.size();j++){
					(*SN)[k][L[j]]+=N[k-1][j]/(p[j]*t);
					(*SN)[4][L[j]]+=N[k-1][j]/(p[j]*t);
				}
				(*TN)[k]=sum((*SN)[k]);
				(*TN)[4]+=sum((*SN)[k]);
			}
		} else if(LF_scaling=="numbers_per_km2"){
			for(int k=1;k<=3;k++){
				for(int j=0;j<L.size();j++){
					(*SN)[k][L[j]]+=1000000*N[k-1][j]/(p[j]*f);
					(*SN)[4][L[j]]+=1000000*N[k-1][j]/(p[j]*f);
				}
				(*TN)[k]=sum((*SN)[k]);
				(*TN)[4]+=sum((*SN)[k]);
			}
		} else if(LF_scaling=="numbers_in_population"){
			for(int k=1;k<=3;k++){
				for(int j=0;j<L.size();j++){
					(*SN)[k][L[j]]+=1000000*N[k-1][j]/(p[j]*f);
					(*SN)[4][L[j]]+=1000000*N[k-1][j]/(p[j]*f);
				}
				(*TN)[k]=sum((*SN)[k]);
				(*TN)[4]+=sum((*SN)[k]);
			}
		} else fatal("'LF_scaling' must be one of 'unscaled','numbers_per_1000','numbers_per_tow','numbers_per_hour','numbers_per_km2',and 'numbers_in_population'.for task 'calc_LFs'");
	}

}
void Station::calc_biomass()
{
	DEBUG0("Station::calc_biomass");

}
void Station::calc_biomass_and_LFs()
{
	DEBUG0("Station::calc_biomass_and_LFs");
	std::string LF_scaling = p_stratum->p_species->p_trip->p_survey->LF_scaling;
	if(anyfish){
		for(int k=1;k<=3;k++){
			for(int j=0;j<L.size();j++){
				(*UN)[k][L[j]]+=N[k-1][j];
				(*UN)[4][L[j]]+=N[k-1][j];
			}
		}
		if(LF_scaling=="numbers_in_population"){
			for(int k=1;k<=3;k++){
				for(int j=0;j<L.size();j++){
					(*SN)[k][L[j]]+=1000000*N[k-1][j]/(p[j]*f);
					(*SN)[4][L[j]]+=1000000*N[k-1][j]/(p[j]*f);
				}
				(*TN)[k]=sum((*SN)[k]);
				(*TN)[4]+=sum((*SN)[k]);
			}
		} else fatal("'LF_scaling' must be 'numbers_in_population'. for task 'calc_biomass_and_LFs'");
	}
}
void Station::calculate_sub_populations()
{
	DEBUG0("Station::calculate_sub_populations");
}


Station::~Station()
{
	DEBUG0("Station::~Station");
	if(!UN) delete UN;
	if(!SN) delete SN;
	if(!TN) delete TN;

}

//The follwing was copied from the trawl survey analysis source code
double get_distance_from_lat_long(std::string nors_s,std::string eorw_s,std::string nors_f,std::string eorw_f,
								double lat_s,double long_s,double lat_f,double long_f){
	DEBUG2("Station::get_distance_from_lat_long");
	double posn,dg,dl,e,course,result;	
	if(lat_s==D_NULL || long_s==D_NULL ||lat_f==D_NULL ||long_f==D_NULL||eorw_s==S_NULL||eorw_f==S_NULL){
		result =D_NULL;
		return result;
	} else {
		lat_s=MINx1000(lat_s)*-1.0;
		lat_f=MINx1000(lat_f)*-1.0;
		long_s =eorw_s!="W"? MINx1000(long_s):MINx1000(long_s)*-1.0;
		long_f =eorw_f!="W"? MINx1000(long_f):MINx1000(long_f)*-1.0;

		dg=long_f-long_s;
		if(dg>MIN180||dg<-1.0*MIN180){
			dg=(long_f<0.0)?MIN180+long_f:MIN180-long_f;
			dg+=(long_s<0.0)?MIN180+long_s:MIN180-long_s;
			if(long_s<0.0) 
				dg*=-1.0;
		}
		dl=lat_f-lat_s;
		e=dg*cos((lat_s+dl/2.0)*HALFPI/5400000.0);
		if(dl!=0){
			course=atan(e/dl);
			result = dl/cos(course);
		} else {
			result=e;
		}
		result=result<0?result/-1000.0:result/1000;
		result*=1.852;
		return result;
	}
}


	

double get_time(int _time_s,std::string _date_s,int _time_f, std::string _date_f){
	DEBUG2("Station::get_time");
	if(_time_s==D_NULL || _date_s==S_NULL || _time_f== D_NULL || _date_f==S_NULL)
		return D_NULL;
	else {
		static MTH month[]={
			"Nil",0,
			"Jan",31,
			"Feb",28,
			"Mar",31,
			"Apr",30,
			"May",31,
			"Jun",30,
			"Jul",31,
			"Aug",31,
			"Sep",30,
			"Oct",31,
			"Nov",30,
			"dec",31
		};
		double result;
		int day_s,day_f,year_s,year_f,num_days;
		std::string mth_s,mth_f,dummy;
		int yr,i,j,k,x,hrs,mins;
		string::size_type loc;
		while((loc = _date_s.find('-',0)) !=std::string::npos){
			_date_s[loc]=' ';
		}
		while((loc = _date_f.find('-',0)) !=std::string::npos){
			_date_f[loc]=' ';
		}
		istringstream _date_s_stream((_date_s+" ").c_str()),_date_f_stream((_date_f+" ").c_str());
		_date_s_stream >> day_s;  if (!_date_s_stream.good()) return D_NULL;
		_date_s_stream >> mth_s;  if (!_date_s_stream.good()) return D_NULL;
		_date_s_stream >> year_s; if (!_date_s_stream.good()) return D_NULL;
		_date_s_stream >> dummy;  if (_date_s_stream.good())  return D_NULL;
		_date_f_stream >> day_f;  if (!_date_f_stream.good()) return D_NULL;
		_date_f_stream >> mth_f;  if (!_date_f_stream.good()) return D_NULL;
		_date_f_stream >> year_f; if (!_date_f_stream.good()) return D_NULL;
		_date_f_stream >> dummy;  if (_date_f_stream.good())  return D_NULL;
		
		//What if
		if(mth_s=="1" || mth_s=="01" ||mth_s=="JAN")
			mth_s="Jan";
		else if(mth_s=="2" || mth_s=="02" ||mth_s=="FEB")
			mth_s="Feb";
		else if(mth_s=="3" || mth_s=="03" ||mth_s=="MAR")
			mth_s="Mar";
		else if(mth_s=="4" || mth_s=="04" ||mth_s=="APR")
			mth_s="Apr";
		else if(mth_s=="5" || mth_s=="05" ||mth_s=="MAY")
			mth_s="May";
		else if(mth_s=="6" || mth_s=="06" ||mth_s=="JUN")
			mth_s="Jun";
		else if(mth_s=="7" || mth_s=="07" ||mth_s=="JUL")
			mth_s="Jul";
		else if(mth_s=="8" || mth_s=="08" ||mth_s=="AUG")
			mth_s="Aug";
		else if(mth_s=="9" || mth_s=="09" ||mth_s=="SEP")
			mth_s="Sep";
		else if(mth_s=="10"  ||mth_s=="OCT")
			mth_s="Oct";
		else if(mth_s=="11" ||mth_s=="NOV")
			mth_s="Nov";
		else if(mth_s=="12" ||mth_s=="DEC")
			mth_s="Dec";

		if(mth_f=="1" || mth_f=="01" ||mth_f=="JAN")
			mth_f="Jan";
		else if(mth_f=="2" || mth_f=="02" ||mth_f=="FEB")
			mth_f="Feb";
		else if(mth_f=="3" || mth_f=="03" ||mth_f=="MAR")
			mth_f="Mar";
		else if(mth_f=="4" || mth_f=="04" ||mth_f=="APR")
			mth_f="Apr";
		else if(mth_f=="5" || mth_f=="05" ||mth_f=="MAY")
			mth_f="May";
		else if(mth_f=="6" || mth_f=="06" ||mth_f=="JUN")
			mth_f="Jun";
		else if(mth_f=="7" || mth_f=="07" ||mth_f=="JUL")
			mth_f="Jul";
		else if(mth_f=="8" || mth_f=="08" ||mth_f=="AUG")
			mth_f="Aug";
		else if(mth_f=="9" || mth_f=="09" ||mth_f=="SEP")
			mth_f="Sep";
		else if(mth_f=="10"  ||mth_f=="OCT")
			mth_f="Oct";
		else if(mth_f=="11" ||mth_f=="NOV")
			mth_f="Nov";
		else if(mth_f=="12" ||mth_f=="DEC")
			mth_f="Dec";

		num_days=day_f-day_s;
		if(year_s!=year_f ||mth_s != mth_f){
			for(i=1;month[i].name != mth_s;i++);
			for(j=1;month[j].name != mth_f;j++);
			for(yr=year_s;yr<=year_f;yr++){
				if((x=yr % 4)==0)
					month[2].lgth=29;
				else
					month[2].lgth=28;
				i=(yr==year_s)? i:1;
				k=(yr==year_f)? j:13;
				for(x=i;x<k;num_days+=month[x++].lgth);
			}
		}
		hrs=(_time_f/100-_time_s/100);
		if(hrs<0){
			num_days--;
			hrs+=24;
		}
		mins=(_time_f-((_time_f/100)*100))-(_time_s-((_time_s/100)*100));
		if(mins<0){
			hrs--;
			mins+=60;
		}
		result=hrs+mins/60.0;
		if(num_days>0) result+=num_days*24;
		return result;
	}
}
