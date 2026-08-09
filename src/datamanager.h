// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* datamanager_h_id = "$Id: datamanager.h 2237 2008-09-08 00:33:48Z fud $\n";

#if !defined(DATAMANAGER)
#define DATAMANAGER

#include "parameter_set.h"

class Output_precision;

class DataManager{
public:
  DataManager(std::string _task,Parameter_set& p,std::string stratum_input_file,std::string station_input_file, std::string catch_input_file, std::string lgth_input_file,std::string subcatch_input_file,std::string station_catch_input_file,
    std::string stratum_output_file,std::string station_output_file, std::string catch_output_file, std::string lgth_output_file, std::string subcatch_output_file,std::string station_catch_output_file,std::string stratum_catch_output_file,bool _z_flag);
  std::string task,LF_scaling,type;
  std::vector<std::string> trips,species;
  bool  catch_kg_km2_supplied;
  bool  output_stratum,output_station,output_catch,output_lgth,output_subcatch,output_station_catch,output_stratum_catch;
  std::string input_mode; //"from_DB" and "from_file"
  bool z_flag;

  class T_table{
  public:
    DataManager* p_DM;
    std::string input_file,output_file,sql;
    std::vector<std::string>  key;
    T_table(DataManager* p_DM,std::string input_file,std::string output_file);
    ~T_table();

    virtual void input(Parameter_set& p){};
    virtual void output(Parameter_set& p,Output_precision& output_precision);
    std::vector<std::string> read_file(ifstream& in);
  private:
    virtual void input_from_DB(Parameter_set& p){};
    virtual void input_from_file(ifstream& in){};
    virtual void output_to_file(ostream& out,Output_precision& output_precision){};
    virtual void input_from_p(Parameter_set& p){};
  };

  class T_stratum:public T_table{
  public:
    T_stratum(DataManager* p_DM,std::string input_file,std::string output_file);
    //headers expected from input file
    int trip_code_col,stratum_col,area_km2_col;

    std::vector<string> trip_code;
    std::vector<string> stratum;
    std::vector<double> area_km2;

    void empty();
    void insert(std::string _trip_code,std::string _stratum, double _area_km2);
    void modify_area_km2(std::string _trip_code,std::string _stratum,double _area_km2);
	void modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum);
    std::vector<std::string> select_stratum(std::string _trip_code);  //Get all strata from a trip in stratum table
    std::vector<double> select_area_km2(std::string _trip_code);
    std::vector<double> select_area_km2(std::string _trip_code,std::string _stratum);

    virtual void input(Parameter_set& p);
  private:
    virtual void input_from_DB(Parameter_set& p);
      virtual void input_from_file(ifstream& in);
    virtual void output_to_file(ostream& out,Output_precision& output_precision);
    virtual void input_from_p(Parameter_set& p);
  };

  class T_station:public T_table{
  public:
    T_station(DataManager* p_DM,std::string input_file,std::string output_file);
    //headers expected from input file
    int trip_code_col,stratum_col,station_no_col,distance_col, speed_col, dist_doors_col, dist_wings_col;
    int lat_s_col, long_s_col, lat_f_col, long_f_col, nors_s_col, eorw_s_col, nors_f_col, eorw_f_col;
    int time_s_col,date_s_col,time_f_col,date_f_col;

    std::vector<string> trip_code;
    std::vector<string> stratum;
    std::vector<int> station_no;
    std::vector<double> distance;
    std::vector<double> speed;
    std::vector<double> dist_doors;
    std::vector<double> dist_wings;
    std::vector<double> lat_s;
    std::vector<string> nors_s;
    std::vector<double> long_s;
    std::vector<string> eorw_s;
    std::vector<double> lat_f;
    std::vector<string> nors_f;
    std::vector<double> long_f;
    std::vector<string> eorw_f;
    std::vector<int> time_s;
    std::vector<std::string> date_s;
    std::vector<int> time_f;
    std::vector<std::string> date_f;

    void set_cols(int _trip_code_col, int _stratum_col,int _station_no_col,
            int _distance_col, int _speed_col, int _dist_doors_col,int _dist_wings_col,
            int _lat_s_col, int _long_s_col, int _lat_f_col, int _long_f_col,
            int _nors_s_col, int _eorw_s_col, int _nors_f_col,int _eorw_f_col,
            int _time_s_col, int _date_s_col, int _time_f_col,int _date_f_col);

    void empty();
    void insert(std::string _trip_code,std::string _stratum, int _station_no,
          double _distance,double _speed, double _dist_doors,double _dist_wings,
          double _lat_s, double _long_s,double _lat_f,double _long_f,
          std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
          int _time_s,std::string _date_s,int _time_f,std::string _date_f);
    void modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum);
    void modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum);
    std::vector<int> select_station_no(std::string _trip_code,std::string _stratum);  //Get all stations from a statrum/trip  in  station table
    std::vector<int> select_station_no(std::string _trip_code);         //Get all stations from trip  in  station table
    std::vector<double> select_distance(std::string _trip_code,std::string _stratum);
    std::vector<double> select_speed(std::string _trip_code,std::string _stratum);
    std::vector<double> select_dist_doors(std::string _trip_code,std::string _stratum);
    std::vector<double> select_dist_wings(std::string _trip_code,std::string _stratum);
    std::vector<double> select_lat_s(std::string _trip_code,std::string _stratum);
    std::vector<double> select_long_s(std::string _trip_code,std::string _stratum);
    std::vector<double> select_lat_f(std::string _trip_code,std::string _stratum);
    std::vector<double> select_long_f(std::string _trip_code,std::string _stratum);
    std::vector<std::string> select_nors_s(std::string _trip_code,std::string _stratum);
    std::vector<std::string> select_eorw_s(std::string _trip_code,std::string _stratum);
    std::vector<std::string> select_nors_f(std::string _trip_code,std::string _stratum);
    std::vector<std::string> select_eorw_f(std::string _trip_code,std::string _stratum);
    std::vector<int> select_time_s(std::string _trip_code,std::string _stratum);
    std::vector<std::string> select_date_s(std::string _trip_code,std::string _stratum);
    std::vector<int> select_time_f(std::string _trip_code,std::string _stratum);
    std::vector<std::string> select_date_f(std::string _trip_code,std::string _stratum);

    virtual void input(Parameter_set& p);
  private:
    virtual void input_from_DB(Parameter_set& p);
      virtual void input_from_file(ifstream& in);
    virtual void output_to_file(ostream& out,Output_precision& output_precision);

  };

  class T_catch:public T_table{
  public:
    T_catch(DataManager* p_DM,std::string input_file,std::string output_file);
    //headers expected from input file
    int trip_code_col,stratum_col,station_no_col,species_col, weight_col;

    std::vector<string> trip_code;
    std::vector<string> stratum;
    std::vector<string> species;
    std::vector<int> station_no;
    std::vector<double> weight;

    void empty();
    void insert(std::string _trip_code,std::string _stratum, int _station_no, std::string _species,double _weight);

    void modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum);
    void modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum);
    std::vector<double> select_weight(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);

    virtual void input(Parameter_set& p);
  private:
    virtual void input_from_DB(Parameter_set& p);
      virtual void input_from_file(ifstream& in);
    virtual void output_to_file(ostream& out,Output_precision& output_precision);
  };

  class T_lgth:public T_table{
  public:
    T_lgth(DataManager* p_DM,std::string input_file,std::string output_file);
    //headers expected from input file
    int trip_code_col,stratum_col,station_no_col,species_col,subcatch_no_col,lgth_col,percent_samp_col,no_a_col,no_f_col,no_m_col;

    std::vector<string> trip_code;
    std::vector<string> stratum;
    std::vector<int> station_no;
    std::vector<string> species;
	std::vector<int> subcatch_no;
    std::vector<int> lgth;

    std::vector<double> percent_samp;
    std::vector<int> no_a;
    std::vector<int> no_f;
    std::vector<int> no_m;
    void empty();
    void insert(std::string _trip_code,std::string _stratum,  int _station_no, std::string _species,
          int _lgth,double _percent_samp,int _no_a,int _no_f,int _no_m);
    void insert(std::string _trip_code,std::string _stratum,  int _station_no, std::string _species,int _subcatch_no,
          int _lgth,double _percent_samp,int _no_a,int _no_f,int _no_m);
    void modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum);
    void modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum);
    std::vector<int> select_lgth(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);
    std::vector<int> select_lgth(std::string _trip_code,std::string _species);
    std::vector<int> select_subcatch_no(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);
    std::vector<double> select_percent_samp(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);
    std::vector<int> select_no_a(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);
    std::vector<int> select_no_f(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);
    std::vector<int> select_no_m(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);
	bool subcatch_defined(std::string _species="");
    virtual void input(Parameter_set& p);
  private:
    virtual void input_from_DB(Parameter_set& p);
    virtual void input_from_file(ifstream& in);
    virtual void output_to_file(ostream& out,Output_precision& output_precision);
  };

  class T_subcatch:public T_table{
  public:
    T_subcatch(DataManager* p_DM,std::string input_file,std::string output_file);
    //headers expected from input file
    int trip_code_col,stratum_col,station_no_col,species_col, weight_col,subcatch_no_col;

    std::vector<string> trip_code;
    std::vector<string> stratum;
    std::vector<string> species;
    std::vector<int> station_no;
	std::vector<int> subcatch_no;
    std::vector<double> weight;

    void empty();
    void insert(std::string _trip_code,std::string _stratum, int _station_no, std::string _species,int _subcatch_no,double _weight);

    void modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum);
    void modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum);
    std::vector<int> select_subcatch_no(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);
	std::vector<double> select_weight(std::string _trip_code,std::string _stratum,int _station_no,std::string _species);

    virtual void input(Parameter_set& p);
  private:
    virtual void input_from_DB(Parameter_set& p);
    virtual void input_from_file(ifstream& in);
    virtual void output_to_file(ostream& out,Output_precision& output_precision);
  };

  class T_station_catch:public T_table{
  public:
    T_station_catch(DataManager* p_DM,std::string input_file,std::string output_file);
    //headers expected from input file
    int trip_code_col,stratum_col,station_no_col;
    //headers in the output file (when input file is station)
    int distance_col, speed_col, dist_doors_col, dist_wings_col;
    int lat_s_col, long_s_col, lat_f_col, long_f_col, nors_s_col, eorw_s_col, nors_f_col, eorw_f_col;
    int time_s_col,date_s_col,time_f_col,date_f_col;

    std::vector<int> weight_kg_km2_cols;  //by sepcies

    std::vector<string> trip_code;
    std::vector<string> stratum;
    std::vector<int> station_no;
    std::vector<std::vector<double> > weight_kg_km2;    // by species

    //variables to be inserted back to T_station_catch to be output to flat files
    std::vector<double> distance;
    std::vector<double> speed;
    std::vector<double> dist_doors;
    std::vector<double> dist_wings;
    std::vector<double> lat_s;
    std::vector<string> nors_s;
    std::vector<double> long_s;
    std::vector<string> eorw_s;
    std::vector<double> lat_f;
    std::vector<string> nors_f;
    std::vector<double> long_f;
    std::vector<string> eorw_f;
    std::vector<int> time_s;
    std::vector<std::string> date_s;
    std::vector<int> time_f;
    std::vector<std::string> date_f;

    //calculated variables to be inserted back to T_station_catch to be output to flat files
    std::vector<double> d;
    std::vector<double> w;
    std::vector<double> t;
    std::vector<double> area_fished;
    std::vector<std::vector<double> > weight;       // by species
    std::vector<std::vector<double> > C;          // by species
    std::vector<std::vector<double> > LD;         // by species
    std::vector<std::vector<double> > AD;         // by species
    std::vector<std::vector<std::vector<double> > >c;          // by species and sub_population
	std::vector<std::vector<std::vector<double> > >ad;         // by species and sub_population

    void empty();
    // Headers indicate whether a particular column exist in the table. They are usually
    // applied to the supplied variables. When insert records to the table, variables with
    // header turned off are ignored.
    void set_cols(int _trip_code_col, int _stratum_col,int _station_no_col,
            int _distance_col, int _speed_col, int _dist_doors_col,int _dist_wings_col,
            int _lat_s_col, int _long_s_col, int _lat_f_col, int _long_f_col,
            int _nors_s_col, int _eorw_s_col, int _nors_f_col,int _eorw_f_col,
            int _time_s_col, int _date_s_col, int _time_f_col,int _date_f_col);

    //from both input file and interval objects (for output)
    //called for output when station_catch_input is supplied
    void insert(std::string _trip_code,std::string _stratum, int _station_no,
          std::vector<double> _weight_kg_km2);
    //from interval objects for output purposes when station_catch_input is not supplied, and
    // task is "calc_LFs" and LF_scaling is "unscaled"
    void insert(std::string _trip_code,std::string _stratum, int _station_no,
          double _distance,double _speed, double _dist_doors,double _dist_wings,
          double _lat_s, double _long_s,double _lat_f,double _long_f,
          std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
          int _time_s,std::string _date_s,int _time_f,std::string _date_f);
    //from interval objects for output purposes when station_catch_input is not supplied, and
    // task is "calc_LFs" , and LF_scaling is none of "unscaled" , "numbers_per_km2", and "numbers_per_hour"
    void insert2(std::string _trip_code,std::string _stratum, int _station_no,
          double _distance,double _speed, double _dist_doors,double _dist_wings,
          double _lat_s, double _long_s,double _lat_f,double _long_f,
          std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
          int _time_s,std::string _date_s,int _time_f,std::string _date_f,
          std::vector<double> _C);
    //from interval objects for output purposes when station_catch_input is not supplied, and
    // task is "calc_LFs" , and LF_scaling is "numbers_per_hour"
    void insert3(std::string _trip_code,std::string _stratum, int _station_no,
          double _distance,double _speed, double _dist_doors,double _dist_wings,
          double _lat_s, double _long_s,double _lat_f,double _long_f,
          std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
          int _time_s,std::string _date_s,int _time_f,std::string _date_f,
          double _t,std::vector<double> _C);
    //from interval objects for output purposes when station_catch_input is not supplied, and
    // task is "calc_LFs" and LF_scaling is "numbers_per_km2", or "calc_biomass" or "calc_biomass_and_LFs", and
    // survey type is "pot"
    void insert4(std::string _trip_code,std::string _stratum, int _station_no,
          double _distance,double _speed, double _dist_doors,double _dist_wings,
          double _lat_s, double _long_s,double _lat_f,double _long_f,
          std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
          int _time_s,std::string _date_s,int _time_f,std::string _date_f,
          double _area_fished,std::vector<double> _C,std::vector<double> _AD);
    //from interval objects for output purposes when station_catch_input is not supplied, and
    // task is "calc_LFs" and LF_scaling is "numbers_per_km2", or "calc_biomass" or "calc_biomass_and_LFs", and
    // survey type is "trawl"
    void insert5(std::string _trip_code,std::string _stratum, int _station_no,
          double _distance,double _speed, double _dist_doors,double _dist_wings,
          double _lat_s, double _long_s,double _lat_f,double _long_f,
          std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
          int _time_s,std::string _date_s,int _time_f,std::string _date_f,
          double _d, double _w,std::vector<double> _C, std::vector<double> _LD,std::vector<double> _AD);
    void insert5(std::string _trip_code,std::string _stratum, int _station_no,
          double _distance,double _speed, double _dist_doors,double _dist_wings,
          double _lat_s, double _long_s,double _lat_f,double _long_f,
          std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
          int _time_s,std::string _date_s,int _time_f,std::string _date_f,
          double _d, double _w,std::vector<double> _C, std::vector<double> _LD,std::vector<double> _AD,
		  std::vector<std::vector<double> >_c,  std::vector<std::vector<double> > _ad);
    void modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum);
    void modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum);
    std::vector<int> select_station_no(std::string _trip_code,std::string _stratum);          //Get all stations from a statrum/trip  in  station table
    std::vector<int> select_station_no(std::string _trip_code);                     //Get all stations from trip  in  station table
    std::vector<double> select_weight_kg_km2(std::string _trip_code,std::string _stratum,std::string _species);

      virtual void input(Parameter_set& p);
  private:
      virtual void input_from_file(ifstream& in);
      virtual void output_to_file(ostream& out,Output_precision& output_precision);
  };

  // Only used in output
  class T_stratum_catch:public T_table{
  public:
    T_stratum_catch(DataManager* p_DM,std::string input_file,std::string output_file);

    std::vector<string> trip_code;
    std::vector<string> stratum;
    std::vector<double> area_km2;

    int trip_code_col,stratum_col,area_km2_col;

    //calculated variables
    std::vector<std::vector<double> > C;  //mean catch rates
    std::vector<std::vector<double> > LD; //mean LD
    std::vector<std::vector<double> > AD; //mean AD;

    //from interval objects called for output when station_catch_input is supplied
	void empty();
    void set_cols(int _trip_code_col,int _stratum_col,int _area_km2_col);
    void insert3(std::string _trip_code,std::string _stratum, double _area_km2,
          std::vector<double>  _AD);
    //from internal objects when station_catch_input is not supplied, and
    //task is "calc_LFs" and LF_scaling is "unscaled"
    void insert2(std::string _trip_code,std::string _stratum, double _area_km2);
    //from interval objects called for output purposes when station_catch_input is not supplied, and
    // task is  "calc_LFs", and LF_scaling is neither "unscaled" nor "numbers_per_km2"
    void insert2(std::string _trip_code,std::string _stratum, double _area_km2,
          std::vector<double>  _C);
    //from interval objects for output purposes when station_catch_input is not supplied, and
    // task is "calc_LFs" and LF_scaling is "numbers_per_km2", or "calc_biomass" or "calc_biomass_and_LFs", and
    // survey type is "pot"
    void insert2(std::string _trip_code,std::string _stratum, double _area_km2,
      std::vector<double>  _C,std::vector<double>  _AD);
    //from interval objects for output purposes when station_catch_input is not supplied, and
    // task is "calc_LFs" and LF_scaling is "numbers_per_km2", or "calc_biomass" or "calc_biomass_and_LFs", and
    // survey type is "trawl"
    void insert2(std::string _trip_code,std::string _stratum, double _area_km2,
      std::vector<double>  _C,std::vector<double>  _LD,std::vector<double>  _AD);

  private:
    virtual void output_to_file(ostream& out,Output_precision& output_precision);
  };

  class T_lw_coeff:public T_table{
  public:
    T_lw_coeff(DataManager* p_DM,std::string input_file);
    int trip_code_col,spp_code_col,sex_col,lw_coeff_a_col,lw_coeff_b_col,lw_coeff_c_col;

    std::vector<string> trip_code;
    std::vector<string> spp_code;
    std::vector<int>  sex;  //D_NULL combined,1 male,2 female,3 unsexed;
    std::vector<double> lw_coeff_a,lw_coeff_b,lw_coeff_c;

    void insert(std::string _trip_code, std::string _spp_code,int _sex,
          double _lw_coeff_a,double _lw_coeff_b,double _lw_coeff_c);
    std::vector<double> select_lw_coeff_a(std::string _trip_code,std::string _spp_code,int _sex);
    std::vector<double> select_lw_coeff_b(std::string _trip_code,std::string _spp_code,int _sex);
    std::vector<double> select_lw_coeff_c(std::string _trip_code,std::string _spp_code,int _sex);
      virtual void input(Parameter_set& p);
    virtual void output(Parameter_set& p,Output_precision& output_precision);
    void print();
  private:
    virtual void input_from_p(Parameter_set& p);
    virtual void input_from_DB(Parameter_set& p);
    virtual void input_from_file(ifstream& in);
    virtual void output_to_file(ostream& out,Output_precision& output_precision);
  };

  T_stratum t_stratum;
  T_station t_station;
  T_catch t_catch;
  T_lgth t_lgth;
  T_subcatch t_subcatch;
  T_station_catch t_station_catch;
  T_stratum_catch t_stratum_catch;
  T_lw_coeff t_lw_coeff;

  std::map<std::string,std::vector<std::string> >  trip_species;
  std::vector<std::string> trip_species_labels; //e.g. tan0901_HOK;
  std::map<std::string,std::vector<std::string> >  species_sub_populatons;
  std::map<std::string,std::map<std::string,std::map<std::string,double> > > trip_species_stratum_areal_availability;
  std::map<std::string,std::map<std::string,std::map<std::string,double> > > trip_species_stratum_population_area;
  std::map<std::string,std::map<std::string,std::map<int,double> > > trip_species_station_vulnerability;
  std::map<std::string,std::map<std::string,std::map<int,double> > > trip_species_station_vertical_availability;
  std::map<std::string,std::map<std::string,std::map<int,double> > > trip_species_station_area_fished;

  void input(Parameter_set& p,bool catch_weight_calculated,bool catch_weight_recorded,bool sub_populations_calculated);
  void output(Parameter_set& p,Output_precision& output_precision);
  void modify(Parameter_set& p);
  void extend(Parameter_set& p);
  void empty();

private:
  void change_strata(Parameter_set& p);
  void change_stratum_area(Parameter_set& p);
  void reassign_strata(Parameter_set& p);

  void set_areal_availability(Parameter_set& p);
  void set_population_area(Parameter_set& p);
  void set_vulnerability(Parameter_set& p);
  void set_vertical_availability(Parameter_set& p);
  void set_area_fished(Parameter_set& p);
};

class Output_precision{
public:
  Output_precision(Parameter_set& p);
  std::map<std::string,std::string> type;
  std::map<std::string,int> precision;
};

#endif

//############################## END OF DATAMANAGER.h ##############################
