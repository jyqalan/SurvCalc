// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* stratum_cpp_id = "$Id: stratum.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "survey.h"

Stratum::Stratum(std::string _stratum,double _area_km2,double _areal_availability, double _population_area, Species* _p_species,DataManager* p_DM)
{
	//area_km2: NULL not permitted;
	//population_area: NULL not permited if  areal_availability is NULL
	//areal_availability: NULL not permited if population_area is NULL
	DEBUG0("Stratum::Stratum");
	path=" stratum "+_stratum+" species "+_p_species->species_code+" trip "+_p_species->p_trip->trip_code;
	p_species=_p_species;
	stratum= _stratum;
	area_km2=_area_km2;
	if(_areal_availability!=D_NULL && _population_area!=D_NULL)
		fatal("You must define either areal_availability or population_area but not both for stratum "+_stratum+" of trip "+_p_species->p_trip->trip_code);
	if(_areal_availability==D_NULL && _population_area==D_NULL){
		fatal("You must define either areal_availability or population_area for stratum "+_stratum+" of trip "+_p_species->p_trip->trip_code);
	}
	areal_availability = _areal_availability;
	population_area = _population_area;

	std::vector<int> _station_no;
	std::vector<double> _distance,_speed,_dist_doors,_dist_wings,_lat_s,_long_s,_lat_f,_long_f;  
	std::vector<std::string> _nors_s,_eorw_s,_nors_f,_eorw_f;
	std::vector<int> _time_s,_time_f;
	std::vector<std::string> _date_s,_date_f;
	std::vector<double> _weight,_weight_kg_km2;
	std::vector<int> _subcatch_no;
	std::vector<double> _subcatch_weight;
	if(p_DM->catch_kg_km2_supplied){
		_station_no = p_DM->t_station_catch.select_station_no(this->p_species->p_trip->trip_code,stratum);
		_weight_kg_km2 = p_DM->t_station_catch.select_weight_kg_km2(this->p_species->p_trip->trip_code,stratum,this->p_species->species_code);
	} else {
		_station_no = p_DM->t_station.select_station_no(this->p_species->p_trip->trip_code,stratum);
		 _distance =p_DM->t_station.select_distance(this->p_species->p_trip->trip_code,stratum);
		_speed=p_DM->t_station.select_speed(this->p_species->p_trip->trip_code,stratum);
		_dist_doors=p_DM->t_station.select_dist_doors(this->p_species->p_trip->trip_code,stratum);
		_dist_wings=p_DM->t_station.select_dist_wings(this->p_species->p_trip->trip_code,stratum);
		_lat_s=p_DM->t_station.select_lat_s(this->p_species->p_trip->trip_code,stratum);
		_long_s=p_DM->t_station.select_long_s(this->p_species->p_trip->trip_code,stratum);
		_lat_f=p_DM->t_station.select_lat_f(this->p_species->p_trip->trip_code,stratum);
		_long_f=p_DM->t_station.select_long_f(this->p_species->p_trip->trip_code,stratum);
		_nors_s=p_DM->t_station.select_nors_s(this->p_species->p_trip->trip_code,stratum);
		_eorw_s=p_DM->t_station.select_eorw_s(this->p_species->p_trip->trip_code,stratum);
		_nors_f=p_DM->t_station.select_nors_f(this->p_species->p_trip->trip_code,stratum);
		_eorw_f=p_DM->t_station.select_eorw_f(this->p_species->p_trip->trip_code,stratum);
		_time_s=p_DM->t_station.select_time_s(this->p_species->p_trip->trip_code,stratum);
		_date_s=p_DM->t_station.select_date_s(this->p_species->p_trip->trip_code,stratum);
		_time_f=p_DM->t_station.select_time_f(this->p_species->p_trip->trip_code,stratum);
		_date_f=p_DM->t_station.select_date_f(this->p_species->p_trip->trip_code,stratum);
	}
	for(int i=0; i<_station_no.size();i++){
		if(!p_DM->catch_kg_km2_supplied) _weight=p_DM->t_catch.select_weight(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code);
		stations.push_back( new Station(_station_no[i],
							_distance.size()>0?_distance[i]:D_NULL,
							_speed.size()>0?_speed[i]:D_NULL,
							_dist_doors.size()>0?_dist_doors[i]:D_NULL,
							_dist_wings.size()>0?_dist_wings[i]:D_NULL,
							_lat_s.size()>0?_lat_s[i]:D_NULL,
							_long_s.size()>0?_long_s[i]:D_NULL,
							_lat_f.size()>0?_lat_f[i]:D_NULL,
							_long_f.size()>0?_long_f[i]:D_NULL,
							_nors_s.size()>0?_nors_s[i]:S_NULL,
							_eorw_s.size()>0?_eorw_s[i]:S_NULL,
							_nors_f.size()>0?_nors_f[i]:S_NULL,
							_eorw_s.size()>0?_eorw_s[i]:S_NULL,
							_time_s.size()>0?_time_s[i]:D_NULL,
							_date_s.size()>0?_date_s[i]:S_NULL,
							_time_f.size()>0?_time_f[i]:D_NULL,
							_date_f.size()>0?_date_f[i]:S_NULL,
							_weight,
							_weight_kg_km2.size()>0?_weight_kg_km2[i]:D_NULL,
							p_DM->t_subcatch.select_subcatch_no(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code),
							p_DM->t_subcatch.select_weight(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code),
							p_DM->t_lgth.select_lgth(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code),
							p_DM->t_lgth.select_subcatch_no(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code),
							p_DM->t_lgth.select_percent_samp(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code),
							p_DM->t_lgth.select_no_a(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code),
							p_DM->t_lgth.select_no_f(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code),
							p_DM->t_lgth.select_no_m(this->p_species->p_trip->trip_code,stratum,_station_no[i],this->p_species->species_code),
							p_DM->trip_species_station_vulnerability.size()>0? p_DM->trip_species_station_vulnerability[p_species->p_trip->trip_code][p_species->species_code][_station_no[i]]:D_NULL,
							p_DM->trip_species_station_vertical_availability.size()>0? p_DM->trip_species_station_vertical_availability[p_species->p_trip->trip_code][p_species->species_code][_station_no[i]]:D_NULL,
							p_DM->trip_species_station_area_fished.size()>0? p_DM->trip_species_station_area_fished[p_species->p_trip->trip_code][p_species->species_code][_station_no[i]]:D_NULL,
							this,p_DM));
	}
	UN = 0;
	SN = 0;
	MSN = 0;
	SN_prime = 0;
	TN_prime = 0;
	MTN = 0;
	se_MTN = 0;
	cv_TN_prime = 0;
	excluded = false;

	C=D_NULL;
	a=D_NULL;		
	n=D_NULL;		
	m=D_NULL;		
	u_a=D_NULL;		
	a_prime=D_NULL;	
	AD=D_NULL;		
	se_AD=D_NULL;	
	LD=D_NULL;		
	AD_lf=D_NULL;	
	p=D_NULL;		
	B=D_NULL;		
	se_B=D_NULL;	
	cv_B=D_NULL;	
	CF=D_NULL;		
	K=D_NULL;		


}

void Stratum::print()
{	
	cerr<<stratum<<endl;
	
	for(int i=0;i<stations.size();i++)
		stations[i]->print();

}

void Stratum::calc_LFs_ancillary()
{
	DEBUG0("Stratum::calc_LFs_ancillary");
	for(int i=0;i<stations.size();i++)
		stations[i]->calc_LFs_ancillary();
	n=0;
	for(int i=0;i<stations.size();i++)
		if(!stations[i]->excluded) n++;
	if(n<2)	{
		//excluded = true; 
		//diagnostics.push_back("Stratum "+stratum+" NOT INCLUDED IN LF ESTIMATES:less than 2 station");
		//return;
	}//NOT SURE ABOUT THIS, MAY NOT NEED TO EXCLUDE THE STRATUM BECAUSE se_AD will not be calculated.
	UN = new dmatrix(1,4,p_species->Lmin,p_species->Lmax);
	UN->initialize();	
	SN = new dmatrix(1,4,p_species->Lmin,p_species->Lmax);
	SN->initialize();
	anyfish=false;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded) {
			anyfish=anyfish|stations[i]->anyfish;
		}
	}

	if(p_species->p_trip->p_survey->LF_scaling!="unscaled"){	// need to know about catch
		anycatch=false;
		m=0;
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded) {
				anycatch=anycatch|stations[i]->anycatch;
				m+=(!stations[i]->anycatch)|stations[i]->anyfish;
			}
		}
		if(!anyfish && anycatch){
			diagnostics.push_back("Missing LF data for Stratum "+stratum+".");
		} else {
			if(!anyfish){
				diagnostics.push_back("Missing LF data for Stratum "+stratum+".");
			}
		}
		if(!p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied){
	        double _C_sum=0;
	        for(int i=0;i<stations.size();i++){
		        if(!stations[i]->excluded) {
			        _C_sum+=(stations[i]->C!=D_NULL?stations[i]->C:0);
		        }  
	        }
	        C = n==0?0:_C_sum/n;
		}
	}
	if(p_species->p_trip->p_survey->LF_scaling=="numbers_per_km2"){
	    double _AD_sum=0;
	    for(int i=0;i<stations.size();i++){
		    if(!stations[i]->excluded) {
			    _AD_sum+=(stations[i]->AD!=D_NULL?stations[i]->AD:0);
		    }
		}
	    AD= n==0?0:_AD_sum/n;
	    if(!p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied && p_species->p_trip->p_survey->p_DM->type!="pot"){
		    double _LD_sum=0;
		    for(int i=0;i<stations.size();i++){
			    if(!stations[i]->excluded) {
				    _LD_sum+=(stations[i]->LD!=D_NULL?stations[i]->LD:0);
			    }
		    }
		    LD= n==0?0:_LD_sum/n;
		}  
	}
	n_stations_used=n_stations_selected=0;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded)
			n_stations_used++;
		n_stations_selected++;
	}
	//That's it, we don't need a,a_prime,u_a,AD,LD,etc.
}

void Stratum::calc_biomass_ancillary()
{
	DEBUG0("Stratum::calc_biomass_ancillary");
	std::string task = p_species->p_trip->p_survey->task;

	for(int i=0;i<stations.size();i++)
		stations[i]->calc_biomass_ancillary();
	n=0;
	for(int i=0;i<stations.size();i++)
		if(!stations[i]->excluded) n++;
	if(n<2)	{
		excluded = true;
		diagnostics.push_back("Stratum "+stratum+" NOT INCLUDED IN "+task+":less than 2 station");
	}//just for now it should be n<2)

	anyfish=false;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded) {
			anyfish=anyfish|stations[i]->anyfish;
		}
	}

	anycatch=false;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded) {
			anycatch=anycatch|stations[i]->anycatch;
		}
	}

	a=area_km2;
	if(areal_availability!=D_NULL)
		u_a=areal_availability;
	if(population_area!=D_NULL)
		a_prime=population_area;
	else
		a_prime=area_km2/u_a;

	if(!p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied){
	    double _C_sum=0;
	    for(int i=0;i<stations.size();i++){
		    if(!stations[i]->excluded) {
			    _C_sum+=(stations[i]->C!=D_NULL?stations[i]->C:0);
		    }
	    }
	    C = n==0?0:_C_sum/n;
	}
	double _AD_sum=0;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded) {
			_AD_sum+=(stations[i]->AD!=D_NULL?stations[i]->AD:0);
		}
	}
	AD= n==0?0:_AD_sum/n;

	if(!p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied && p_species->p_trip->p_survey->p_DM->type!="pot"){
		double _LD_sum=0;
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded) {
				_LD_sum+=(stations[i]->LD!=D_NULL?stations[i]->LD:0);
			}
		}
		LD= n==0?0:_LD_sum/n;
	}
	if(n>1){
		se_AD=0;
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded) {
				se_AD+=pow((stations[i]->AD!=D_NULL? stations[i]->AD:0)-AD,2)/(n*(n-1));
			}
		}
		se_AD=sqrt(se_AD);
	}

	n_stations_used=n_stations_selected=n_stations_catch=0;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded){
			n_stations_used++;
			if(stations[i]->anycatch)
				n_stations_catch++;
		}
		n_stations_selected++;
	}


}

void Stratum::calc_biomass_and_LFs_ancillary()
{
	DEBUG0("Stratum::calc_biomass_and_LFs_ancillary");
	std::string task = p_species->p_trip->p_survey->task;
	for(int i=0;i<stations.size();i++)
		stations[i]->calc_biomass_and_LFs_ancillary();
	n=0;
	for(int i=0;i<stations.size();i++)
		if(!stations[i]->excluded) n++;
	
	MTN = 0;
	se_MTN = 0;
	cv_TN_prime = 0;
	UN = new dmatrix(1,4,p_species->Lmin,p_species->Lmax);
	SN = new dmatrix(1,4,p_species->Lmin,p_species->Lmax);
	SN_prime = new dmatrix(1,4,p_species->Lmin,p_species->Lmax);
	MSN = new dmatrix(1,4,p_species->Lmin,p_species->Lmax);
	TN_prime = new dvector(1,4);
	MTN = new dvector(1,4);
	se_MTN= new dvector(1,4);
	cv_TN_prime = new dvector(1,4);;
	UN->initialize();
	SN->initialize();
	SN_prime->initialize();
	MSN->initialize();
	TN_prime->initialize();
	MTN->initialize();
	se_MTN->initialize();
	cv_TN_prime->initialize();

	anyfish=false;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded) {
			anyfish=anyfish|stations[i]->anyfish;
		}
	}

	anycatch=false;
	m=0;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded) {
			anycatch=anycatch|stations[i]->anycatch;
			m+=(!stations[i]->anycatch)|stations[i]->anyfish;
		}
	}

	if(m<2){
		excluded = true; 
		diagnostics.push_back("Stratum "+stratum+" NOT INCLUDED IN "+task+":less than 2 station that have measured fish or zero catch weight");
		return;
	}
	if(n<2)	{
		excluded = true; 
		diagnostics.push_back("Stratum "+stratum+" NOT INCLUDED IN "+task+":less than 2 station");
		return;
	}

	if(!anyfish && anycatch){
		if(task=="calc_biomass_and_LFs"){
		    diagnostics.push_back("Missing LF data for Stratum "+stratum+". Total LF numbers have been scaled to total biomass");
		}
	}
	a=area_km2;
	if(areal_availability!=D_NULL)
		u_a=areal_availability;
	if(population_area!=D_NULL)
		a_prime=population_area;
	else
		a_prime=area_km2/u_a;
    
	if(!p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied){
	    double _C_sum=0;
	    for(int i=0;i<stations.size();i++){
		    if(!stations[i]->excluded) {
			    _C_sum+=(stations[i]->C!=D_NULL?stations[i]->C:0);
		    }
	    }
	    C= n==0?0:_C_sum/n;
	}

	double _AD_sum=0;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded) {
			_AD_sum+=(stations[i]->AD!=D_NULL?stations[i]->AD:0);
		}
	}
	AD= n==0?0:_AD_sum/n; 

	if(!p_species->p_trip->p_survey->p_DM->catch_kg_km2_supplied && p_species->p_trip->p_survey->p_DM->type!="pot"){
		double _LD_sum=0;
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded) {
				_LD_sum+=(stations[i]->LD!=D_NULL?stations[i]->LD:0);
			}
		}
		LD= n==0?0:_LD_sum/n; 
	}

	if(n>1){
		se_AD=0;
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded) {
				se_AD+=pow((stations[i]->AD!=D_NULL? stations[i]->AD:0)-AD,2)/(n*(n-1));
			}
		}
		se_AD=sqrt(se_AD);
	}

	if(anyfish){	//otherwise m==0
		double _AD_lf_sum=0;
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded) {
				if(stations[i]->anyfish) _AD_lf_sum+=(stations[i]->AD!=D_NULL?stations[i]->AD:0);
			}
		}
		AD_lf = _AD_lf_sum/m;	
		K = a_prime*AD/AD_lf;
	}
	n_stations_catch=n_stations_used=n_stations_selected=0;
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded)
			n_stations_used++;
			if(stations[i]->anycatch)
				n_stations_catch++;
		n_stations_selected++;
	}

}

void Stratum::calculate_sub_populations_ancillary(){
	DEBUG0("Station::~calculate_sub_populations_ancillary");


	ex_for_sub_population=false;
	if(anyfish){
		double _C_lf_sum=0,_C_sum=0;
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded){
				if(stations[i]->anycatch) _C_sum+=stations[i]->C;
				if(stations[i]->anyfish) _C_lf_sum+=stations[i]->C;
			}
		}
		p=_C_lf_sum/_C_sum;
	} else {
		p=0;
	}
	if(p<0.3 & anycatch) {
		ex_for_sub_population=true; 
		diagnostics.push_back("Stratum "+stratum+" NOT INCLUDED IN SUB_POPULATION ESTIMATES: <30% of the catch had LFs");
		return;
	}

	//First calculate_sub_populations_ancillary for all stations with sampled fish	
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded && stations[i]->anyfish){
			stations[i]->calculate_sub_populations_ancillary();
		}
	}
	//Second calculate_sub_populations_ancillary for all stations with no sampled fish	
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded && !stations[i]->anyfish)
			stations[i]->calculate_sub_populations_ancillary();
	}

	for(int i=0;i<p_species->sub_populations.size();i++){
			double _ad_sum=0;
			for(int j=0;j<stations.size();j++){
				if(!stations[j]->excluded){
					_ad_sum+=stations[j]->ad[i];
				}
			}
			ad.push_back(_ad_sum/n);	
			double temp=0;
			for(int j=0;j<stations.size();j++){
				if(!stations[j]->excluded) {
					temp+=pow(stations[j]->ad[i]-_ad_sum/n,2)/(n*(n-1));
				}
			}
			se_ad.push_back(sqrt(temp));
	}
}

void Stratum::calc_LFs()
{
	DEBUG0("Stratum::calc_LFs");
	std::string LF_scaling = p_species->p_trip->p_survey->LF_scaling;
	if(anyfish){
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded){
				stations[i]->calc_LFs();
			}
		}
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded){
				if(stations[i]->anyfish){
						(*UN)+=*(stations[i]->UN);
					if(LF_scaling=="unscaled"){
						(*SN)+=*(stations[i]->SN);
					} else {
						for(int k=1;k<=3;k++){
							for(int l=(*SN).colmin();l<=(*SN).colmax();l++){
								(*SN)[k][l]+=(*stations[i]->SN)[k][l]/m;
								(*SN)[4][l]+=(*stations[i]->SN)[k][l]/m;
							}
						}
					}
				}
			}
		}
	}
}
void Stratum::calc_biomass()
{
	DEBUG0("Stratum::calc_biomass");
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded){
			stations[i]->calc_biomass();
		}
	}
	B=AD*a_prime/1000;
	se_B=se_AD*a_prime/1000;
	cv_B=B==0?0:se_B/B;

}
void Stratum::calc_biomass_and_LFs()
{
	DEBUG0("Stratum::calc_biomass_and_LFs");
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded){
			stations[i]->calc_biomass_and_LFs();
		}
	}
	B=AD*a_prime/1000;
	se_B=se_AD*a_prime/1000;
	cv_B=B==0?0:se_B/B;

	if(anyfish){
		for(int i=0;i<stations.size();i++){
			if(!stations[i]->excluded){
				if(stations[i]->anyfish){
					(*UN)+=*(stations[i]->UN);
					for(int k=1;k<=3;k++){
						for(int l=(*MSN).colmin();l<=(*MSN).colmax();l++){
							(*MSN)[k][l]+=(*stations[i]->SN)[k][l]/m;
							(*MSN)[4][l]+=(*stations[i]->SN)[k][l]/m;
						}
						(*MTN)[k]=sum((*MSN)[k]);
						(*MTN)[4]+=sum((*MSN)[k]);
					}
				}
			}
		}
		
		double _sum_SN_W=0;
		for(int k=1;k<=3;k++){
			for(int l=(*SN).colmin();l<=(*SN).colmax();l++){
				(*SN)[k][l]=(*MSN)[k][l]*K;
				(*SN)[4][l]+=(*MSN)[k][l]*K;;
				_sum_SN_W+=(*SN)[k][l]*p_species->mean_weights[k-1].get_mean_weight(l);
				
			}
		}
		CF=1000000*B/_sum_SN_W;
		//cerr<<stratum<<" "<<"B="<<B<<" "<<"F="<<sum((*SN)[1])<<" "<<"M="<<sum((*SN)[2])<<" "<<"U="<<sum((*SN)[3])<<" "<<"CF="<<CF<<"\n";
		for(int k=1;k<=3;k++){
			for(int l=(*SN_prime).colmin();l<=(*SN_prime).colmax();l++){
				(*SN_prime)[k][l]=(*SN)[k][l]*CF;
				(*SN_prime)[4][l]+=(*SN)[k][l]*CF;
			}
			(*TN_prime)[k]=sum((*SN_prime)[k]);
			(*TN_prime)[4]+=sum((*SN_prime)[k]);
		}
		for(int k=1;k<=4;k++){
			for(int i=0;i<stations.size();i++){
				if(!stations[i]->excluded){
					if(stations[i]->anyfish || stations[i]->C==0){
						(*se_MTN)[k]+=pow((*stations[i]->TN)[k]-(*MTN)[k],2)/(m*(m-1));
					}
				}
			}
			(*se_MTN)[k]=sqrt((*se_MTN)[k]);
			(*cv_TN_prime)[k]=(*MTN)[k]==0?0:100*(*se_MTN)[k]/(*MTN)[k];
		}
	}
}


void Stratum::calculate_sub_populations()
{
	DEBUG0("Stratum::calculate_sub_populations");
	for(int i=0;i<stations.size();i++){
		if(!stations[i]->excluded){
			stations[i]->calculate_sub_populations();
		}
	}
	for(int i=0;i<p_species->sub_populations.size();i++){
		b.push_back(ad[i]*a_prime/1000);
		se_b.push_back(se_ad[i]*a_prime/1000);
		cv_b.push_back(b[b.size()-1]==0?0:se_b[se_b.size()-1]/b[b.size()-1]);
	}
}



Stratum::~Stratum()
{
	DEBUG0("Station::~Stratum");
	if(!UN) delete UN;
	if(!SN) delete SN;
	if(!MSN) delete MSN;
	if(!MTN) delete MTN;
	if(!SN_prime) delete SN_prime;
	if(!TN_prime) delete TN_prime;
	if(!se_MTN) delete se_MTN;
	if(!cv_TN_prime) delete cv_TN_prime;
	for(int i=0;i<stations.size();i++)
		delete stations[i];


}
