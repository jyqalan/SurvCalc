// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* survey_id = "$Id: survey.h 2237 2008-09-08 00:33:48Z fud $\n";
#if !defined(SURVEY)
#define SURVEY

#include "datamanager.h"

class Trip;
class Species;
class Stratum;
class Station;
class Length_based_mean_weight;
class Sub_population;


class Survey{
public:
	Survey(std::string _task,std::string input_slc,
			std::string stratum_input_file,std::string station_input_file, std::string catch_input_file, std::string lgth_input_file,std::string subcatch_input_file,std::string station_catch_input_file,
			std::string stratum_output_file,std::string station_output_file, std::string catch_output_file, std::string lgth_output_file,std::string subcatch_output_file,std::string station_catch_output_file,std::string stratum_catch_output_file,bool _z_flag);
	~Survey();
	DataManager* p_DM;
	Parameter_set p;
	std::string task,LF_scaling,type,output_for_catch_at_age_file;


	std::vector<Trip*> trips;
	std::vector<std::string> preferences_labels,constant_speed_labels,constant_doorspread_labels,sub_populations_labels;


	void do_input();
	void do_calculation();
	void do_output();

	void print_biomass_by_species_trip(Output_precision& output_precision,ostream& out = cout);
	void print();
	bool catch_weight_calculated();
	bool catch_weight_recorded();
	bool sub_populations_calculated();

};

class Trip{
public:
	Trip(std::string _trip_code,Survey* _p_survey,DataManager* p_DM);
	~Trip();
	Survey* p_survey;
	std::string path;
	std::string trip_code;
	std::vector<Species*> species;
	std::vector<std::string> preferences_distance_towed;
	std::vector<std::string> preferences_width_swept;
	std::vector<std::string> preferences_catch_weight;
	double constant_speed,constant_doorspread;

	void calc_LFs();
	void calc_biomass();
	void calc_biomass_and_LFs();

	void calc_LFs_ancillary();
	void calc_biomass_ancillary();
	void calc_biomass_and_LFs_ancillary();

	void output_LFs(Output_precision& output_precision,ostream& out = cout);
	void output_biomass(Output_precision& output_precision,ostream& out = cout);
	void output_biomass_and_LFs(Output_precision& output_precision,ostream& out = cout);
	void output_for_catch_at_age(Output_precision& output_precision,ostream& out = cout);
	void output_phase_2(Output_precision& output_precision,ostream& out= cout);

	void output_t_table();
	void output_t_stratum();
	void output_t_station();
	void output_t_catch();
	void output_t_lgth();
	void output_t_subcatch();
    void output_t_station_catch();
    void output_t_stratum_catch();

	void print_biomass_by_species(Output_precision& output_precision,ostream& out = cout);
	void print_biomass_by_species_stratum(Output_precision& output_precision,ostream& out = cout);
	void print_run_parameters(Output_precision& output_precision,ostream& out = cout);
	void print_selection(Output_precision& output_precision,ostream& out = cout);
	void print_projected_cvs(Output_precision& output_precision,ostream& out = cout);

	void print();
};

class Species{
public:
	Species(std::string _species_code,Trip *_p_trip,DataManager* p_DM);
	~Species();
	Trip* p_trip;
	std::string path;
	//input data:
	std::string species_code;
	std::vector<Stratum*> strata;
	std::vector<Length_based_mean_weight> mean_weights;
	std::vector<Sub_population> sub_populations;
	int Lmin,Lmax;
	//calculated variables:
	double B;	//overall biomass (t);
	double se_B;	// standard deviation of B;
	double cv_B;	// cv of B;
	int n;	//number of strata in the survey;
	int m;	//number of strataa with LF samples or zero catch;
	double CF;	//LF correction factor;
	dmatrix* UN; //unscaled numbers of fish;
	dmatrix* SN; //scaled numbers of fish ;
	dmatrix* SN_prime;	//scaled numbers of fish corrected for consistency with biomass estimates and length_weigth relationship
	dvector* TN_prime;	//total numbers;
	dvector* cv_TN_prime; // cv of TN_prime;

	//other derived stuff;
	bool	anycatch;	//if total catch >0
	bool	anyfish;	//if any fish sampled (measured for length);

	//sub_pupulation stuff
	std::vector<double> b;
	std::vector<double> se_b;
	std::vector<double> cv_b;

	//summary diagnostics
	int n_strata_selected,n_strata_used,n_strata_used_sub_population;
	int n_stations_selected,n_stations_used,n_stations_used_sub_population;
	std::vector<int> stations_not_used;
	std::vector<std::string> strata_not_used;
	double mean_d,min_d,max_d,mean_w,min_w,max_w;

	//phase_2_calc stuff
	dmatrix *G;
	std::vector<dmatrix> g; // for sub-population;
	bool phase_2_requested;

	//projected_cvs calculations
	bool projected_cvs_requested;
	double cv_B_proj; //projected cv of B when extra stations when added;
	std::vector<double> cv_b_proj; ////projected cv of b when extra stations when added;
	std::vector<std::string> strata_proj;
	std::vector<double> n_stations_proj;

	//tasks
	void calc_LFs();
	void calc_biomass();
	void calc_biomass_and_LFs();
	void calculate_sub_populations();
	void phase_2_calc(std::string command);

	void calc_LFs_ancillary();
	void calc_biomass_ancillary();
	void calc_biomass_and_LFs_ancillary();
	void calculate_sub_populations_ancillary();


	void print_diagnostics_LFs(Output_precision& output_precision,ostream& out = cout);
    void print_diagnostics_biomass(Output_precision& output_precision,ostream& out = cout);
	void print_diagnostics_biomass_and_LFs(Output_precision& output_precision,ostream& out = cout);
	void print_stratum_summary(Output_precision& output_precision,ostream& out = cout);
	void print_biomass(Output_precision& output_precision,ostream& out = cout);
	void print_sub_biomass_by_stratum(Output_precision& output_precision,ostream& out = cout);
	void print_LF_overall(Output_precision& output_precision,ostream& out = cout);
	void print_LFs_by_stratum(Output_precision& output_precision,ostream& out = cout);
	void print_LFs_by_station(Output_precision& output_precision,ostream& out = cout);
	void print_Number_measured(Output_precision& output_precision,ostream& out = cout);
	void print_LF_totals(Output_precision& output_precision,ostream& out = cout);
	void print_for_catch_at_age(Output_precision& output_precision,std::string output_for_catch_at_age_file);
	void print_phase_2(Output_precision& output_precision,ostream& out = cout);

	void print();
};

class Stratum{
public:
	Stratum(std::string _stratum,double _area_km2,double _areal_availability, double _population_area, Species* _p_species,DataManager* p_DM);
	~Stratum();
	Species* p_species;
	std::string path;
	//input data:
	std::string stratum;
	double area_km2,areal_availability,population_area;
	std::vector<Station*> stations;
	//calculated variables:
	double a;		//stratum area;
	int n;		//number of selected stations;
	int m;		//number of selected stations where there is LF data or C=0;
	double u_a;		//areal_availability;
	double a_prime;	//population area (km2);
	double C;		//mean catch weight(kg); mainly for output
	double AD;		//mean areal density(kg/km2);
	double se_AD;	//standard deviation of AD;
	double LD;		//mean linear density(kg/km);
	double AD_lf;	//mean areal density(kg/km2) over stations with LF data or C=0;
	double p;		// proportions of catch taken in stations with LF data;
	double B;		// biomass(t);
	double se_B;	// standard deviation of B;
	double cv_B;	// cv of B;
	double CF;		// LF correction factor;
	double K;		// LF scaling factor;
	dmatrix* UN;	//unscaled numbers of fish;
	dmatrix* SN;	//scaled numbers of fish ;
	dmatrix* SN_prime;	//scaled numbers of fish corrected for consistency with biomass estimates and length_weigth relationship
	dmatrix* MSN;	//mean scaled numbers;
	dvector* TN_prime;	//total numbers;
	dvector* cv_TN_prime; // cv of TN_prime;
	dvector* MTN;	//total numbers;
	dvector* se_MTN; //cv of MTN;
	//other derived stuff;
	bool	anycatch;	//if total catch >0
	bool	anyfish;	//if any fish sampled (measured for length);
	bool	ex_for_sub_population; // excluded from  sub_population calculation since p<0.3
	bool	excluded;	//excluded from calculation;

	//summary diagnostics
	int n_stations_selected,n_stations_used,n_stations_catch;

	void calc_LFs();
	void calc_biomass();
	void calc_biomass_and_LFs();
	void calculate_sub_populations();

	void calc_LFs_ancillary();
	void calc_biomass_ancillary();
	void calc_biomass_and_LFs_ancillary();
	void calculate_sub_populations_ancillary();

	//sub_pupulation stuff
	std::vector<double> c;
	std::vector<double> ad;
	std::vector<double> se_ad;
	std::vector<double> b;
	std::vector<double> se_b;
	std::vector<double> cv_b;

	std::vector<std::string> diagnostics;


	void print();
};

class Station{
public:
	Station(int _station_no, double _distance,double _speed,double _dist_doors,double _dist_wings,
			double _lat_s,double _long_s,double _lat_f, double _long_f,
			std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
			int _time_s,std::string _date_s,int _time_f, std::string _date_f,
			std::vector<double> _weight,double _weight_kg_km2,std::vector<int> _subcatch_no,std::vector<double> _subcatch_weight,
			std::vector<int> _lgth,std::vector<int> _lgth_subcatch_no,std::vector<double> _percent_samp,
			std::vector<int> _no_a,std::vector<int> _no_f,std::vector<int> _no_m,
			double _vulnerability,double _vertical_availability, double _area_fished,
			Stratum* _p_stratum,DataManager* p_DM);


	~Station();
	Stratum *p_stratum;
	std::string path;
	//input data:
	int station_no;
	double distance,speed,dist_doors,dist_wings;
	double lat_s,long_s,lat_f,long_f;
	std::string nors_s,eorw_s,nors_f,eorw_f;
	int time_s, time_f ;
	std::string date_s,date_f;
	double weight,weight_kg_km2;
	std::vector<int> subcatch_no;
	std::vector<double> subcatch_weight;
	std::vector<int> lgth,no_a, no_f,no_m,no_unsexed,lgth_subcatch_no;
	std::vector<double> percent_samp;
	double vulnerability,vertical_availability,area_fished;
	//calculated variables:
	double t;	//time towed (h);
	double d;	//distance towed (n.mile);
	double C;	//catch weight (kg);
	double w;	//width of tow (m);
	double v;	//vulnerability ;
	double u_v; // vertical availability;
	double f;	// effective area fished (m2)
	double AD;	// areal density;
	double LD;	// linear density;
	std::vector<double> AD_subcatch; //area density for subcatch
	std::vector<int> C_subcatch_no; //subcatch no;
	std::vector<double> C_subcatch; //subcatch weight (kg);
	std::vector<int> L_subcatch_no; //subcatch no for each fish
	std::vector<double> p;	//proportion sampled
	std::vector<std::vector<int> >	N;	//number of fish by length and by sex (female,male,unsexed)
	std::vector<int>	L;	//length of fish (cm);
	std::vector<std::vector<double> >	W;	//weight of fish by length and by sex(female,male,unsexed);
	dmatrix* UN;	//unscaled numbers of fish;
	dmatrix* SN;	//scaled numbers of fish;
	dvector* TN;	//total numbers;

	//other derived stuff;
	bool	anycatch;	//if total catch >0
	bool	anyfish;	//if any fish sampled (measured for length);
	bool	excluded;	//excluded if unable to obtain C,d,w; or (anyfish &&!anycatch)

	//sub_pupulation stuff
	std::vector<double> c;
	std::vector<double> ad;

	std::vector<std::string> diagnostics;

	void calc_LFs();
	void calc_biomass();
	void calc_biomass_and_LFs();
	void calculate_sub_populations();

	void calc_LFs_ancillary();
	void calc_biomass_ancillary();
	void calc_biomass_and_LFs_ancillary();
	void calculate_sub_populations_ancillary();
	void print();

};
double get_distance_from_lat_long(std::string nors_s,std::string eorw_s,std::string nors_f,std::string eorw_f,
								  double lat_s,double long_s,double lat_f,double long_f);
double get_time(int time_s,std::string date_s,int time_f, std::string date_f);


class Length_based_mean_weight{
public:
	Length_based_mean_weight(double _a1,double _b1,double _c1,double _a2=D_NULL,double _b2=D_NULL,double _c2=D_NULL);
	double get_mean_weight(double l);
	double a1,b1,c1,a2,b2,c2;
	void print();
};

class Sub_population{
public:
	Sub_population(std::string _label,std::string _sex,int _Lmin,int _Lmax);
	std::vector<int> sexes;
	std::string label;
	int Lmin;
	int Lmax;
	bool in_sub_population(int sex,int L);
	void print();
};

typedef struct{
	std::string name;
	int lgth;
} MTH;


#endif

//############################## END OF SURVEY.h ##############################
