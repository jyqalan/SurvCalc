// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* t_station_catch_cpp_id = "$Id: t_station_catch.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "datamanager.h"

DataManager::T_station_catch::T_station_catch(DataManager* p_DM,std::string input_file,std::string output_file):T_table(p_DM,input_file,output_file){
	DEBUG0("DataManager::T_station_catch::T_station_catch");

}

void DataManager::T_station_catch::input(Parameter_set& p){
	DEBUG0("DataManager::T_station_catch::input");
	if(!input_file.empty()){
		ifstream in(input_file.c_str());
		if (!in) fatal("Could not open file " + input_file);
		input_from_file(in);
		in.close();
		if(p_DM->input_mode=="from_DB")
			fatal("The program is attemping to extract lgth data from text file,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
		else 
			p_DM->input_mode="from_file";
	}  else {
		fatal("Can not input data for t_station_catch table.");
	}
}
void DataManager::T_station_catch::set_cols(int _trip_code_col, int _stratum_col,int _station_no_col,
						int _distance_col, int _speed_col, int _dist_doors_col,int _dist_wings_col,
						int _lat_s_col, int _long_s_col, int _lat_f_col, int _long_f_col, 
						int _nors_s_col, int _eorw_s_col, int _nors_f_col,int _eorw_f_col,
						int _time_s_col, int _date_s_col, int _time_f_col,int _date_f_col){
	DEBUG0("DataManager::T_station_catch::set_cols");
	trip_code_col= _trip_code_col;
	stratum_col=_stratum_col;
	station_no_col=_station_no_col;
	speed_col=_speed_col;
	dist_doors_col=_dist_doors_col;
	dist_wings_col=_dist_wings_col;
	lat_s_col=_lat_s_col;
	long_s_col=_long_s_col;						
	lat_f_col=_lat_f_col;
	long_f_col=_long_f_col;
	nors_s_col=_nors_s_col;
	eorw_s_col=_eorw_s_col;
	nors_f_col=_nors_f_col;
	eorw_f_col=_eorw_f_col;
	time_s_col=_time_s_col;
	date_s_col=_date_s_col;
	time_f_col=_time_f_col;
	date_f_col=_date_f_col;
}


void DataManager::T_station_catch::input_from_file(ifstream& in){
	DEBUG0("DataManager::T_station_catch::input_from_file");
	std::vector<std::string> lines=read_file(in);
	if(lines.size()==0)
		fatal("There is no data "+input_file);
	// Read in header
	std::vector<std::string> header=string_to_string_vector(lines[0]);
	trip_code_col=pos(header,"trip_code");
	if(trip_code_col==-1 && p_DM->trips.size()>1)
		fatal("Can not find 'trip_code' in "+input_file);
	stratum_col=pos(header,"stratum");
	if(stratum_col==-1)
		fatal("Can not find 'stratum' in "+input_file);
	station_no_col=pos(header,"station_no");
	if(station_no_col==-1)
		fatal("Can not find 'station_no' in "+input_file);
	for(int i=0;i<p_DM->species.size();i++){
		int position = pos(header,p_DM->species[i]+"_kg_km2");
		if(position==-1)
			fatal("Can not find '"+p_DM->species[i]+"_kg_km2' in "+input_file);
		else
			weight_kg_km2_cols.push_back(position);
	}
	speed_col=dist_doors_col=dist_wings_col=lat_s_col=long_s_col=lat_f_col=long_f_col=nors_s_col=eorw_s_col=nors_f_col=eorw_f_col=time_s_col=date_s_col=time_f_col=date_f_col=-1;
	
	int ncols=2+(trip_code_col!=-1)+p_DM->species.size();
	//Read in data
	for(int i=1;i<lines.size();i++){
		std::vector<std::string> values=string_to_string_vector(lines[i]);
		if(values.size()!=header.size()){
			warning("Skip line "+itos(i)+" from file "+input_file+":\n"+lines[i]+"\n");
			//warning("There should be "+itos(ncols) +" fields in file "+input_file+" but only "+itos(values.size()) +" are found in line "+itos(i)+":\n"+lines[i]+"\nThe line is ignored!\n");
			continue;
		}
		std::vector<double> _weight_kg_km2;
		for(int j=0;j<p_DM->species.size();j++){
			_weight_kg_km2.push_back(values[weight_kg_km2_cols[j]]!="NULL"? stod(values[weight_kg_km2_cols[j]],p_DM->species[j]+"_kg_km2"+" has a value of "+ values[weight_kg_km2_cols[j]]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL);
		}
		insert(trip_code_col!=-1? (values[trip_code_col]!="NULL"? values[trip_code_col]:S_NULL):S_NULL,
				values[stratum_col]!="NULL"? values[stratum_col]:S_NULL,
				values[station_no_col]!="NULL"? stoi(values[station_no_col],"'station_no' has a value of "+ values[station_no_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,
				_weight_kg_km2);
	}
}

void DataManager::T_station_catch::output_to_file(ostream& out,Output_precision& output_precision){
	DEBUG0("DataManager::T_station_catch::output_to_file");
	if(trip_code.size()>0)
		out<<"trip_code"<<" ";
	out<<"stratum"<<" ";
	out<<"station_no"<<" ";
	if(distance.size()>0)
		out<<"distance"<<" ";
	if(speed.size()>0)
		out<<"speed"<<" ";
	if(dist_doors.size()>0)
		out<<"dist_doors"<<" ";
	if(dist_wings.size()>0)
		out<<"dist_wings"<<" ";
	if(lat_s.size()>0)
		out<<"lat_s"<<" ";
	if(long_s.size()>0)
		out<<"long_s"<<" ";
	if(lat_f.size()>0)
		out<<"lat_f"<<" ";
	if(long_f.size()>0)
		out<<"long_f"<<" ";
	if(nors_s.size()>0)
		out<<"nors_s"<<" ";
	if(eorw_s.size()>0)
		out<<"eorw_s"<<" ";
	if(nors_f.size()>0)
		out<<"nors_f"<<" ";
	if(eorw_f.size()>0)
		out<<"eorw_f"<<" ";
	if(time_s.size()>0)
		out<<"time_s"<<" ";
	if(date_s.size()>0)
		out<<"date_s"<<" ";
	if(time_f.size()>0)
		out<<"time_f"<<" ";
	if(date_f.size()>0)
		out<<"date_f"<<" ";
	if(d.size()>0)
		out<<"d"<<" ";
	if(w.size()>0)
		out<<"w"<<" ";
	if(t.size()>0)
		out<<"t"<<" ";
 
	if(C.size()>0){
		for(int j=0;j<p_DM->species.size();j++){
			out<<p_DM->species[j]+"_kg"<<" ";
		}
	}
	if(LD.size()>0){
		for(int j=0;j<p_DM->species.size();j++){
			out<<p_DM->species[j]+"_kg_km"<<" ";
		}
	}
	if(AD.size()>0 && weight_kg_km2.size()>0){
		fatal("Both 'AD'(calculated) and 'weight_kg_km2'(supplied) exist in station table,this is not supposed to happen, please report this as a bug!");
	}
	if(AD.size()>0){
		for(int j=0;j<p_DM->species.size();j++){
			out<<p_DM->species[j]+"_kg_km2"<<" ";
		}
	}
	if(c.size()>0){
		for(int j=0;j<p_DM->species.size();j++){
			if(in(p_DM->species_sub_populatons,p_DM->species[j])){
				for(int k=0;k<p_DM->species_sub_populatons[p_DM->species[j]].size();k++){
					out<<p_DM->species[j]+"_"+p_DM->species_sub_populatons[p_DM->species[j]][k]+"_kg"<<" ";
				}
			}
		}
	}
	if(ad.size()>0){
		for(int j=0;j<p_DM->species.size();j++){
			if(in(p_DM->species_sub_populatons,p_DM->species[j])){
				for(int k=0;k<p_DM->species_sub_populatons[p_DM->species[j]].size();k++){
					out<<p_DM->species[j]+"_"+p_DM->species_sub_populatons[p_DM->species[j]][k]+"_kg_km2"<<" ";
				}
			}
		}
	}
	if(weight_kg_km2.size()>0){						// can not occur if AD.size()>0,vice versa
		for(int j=0;j<p_DM->species.size();j++){
			out<<p_DM->species[j]+"_kg_km2"<<" ";
		}
	}
	out<<"\n";
	for(int i=0;i<stratum.size();i++){
		if(trip_code.size()>0)
			out<<trip_code[i]<<" ";
		out<<stratum[i]<<" ";
		out<<station_no[i]<<" ";
		if(distance.size()>0) 
			if (distance[i]!=D_NULL) out<< distance[i]<<" "; else out<<"NULL"<<" ";
		if(speed.size()>0) 
			if (speed[i]!=D_NULL) out<< speed[i]<<" "; else out<<"NULL"<<" ";
		if(dist_doors.size()>0) 
			if (dist_doors[i]!=D_NULL) out<< dist_doors[i]<<" "; else out<<"NULL"<<" ";
		if(dist_wings.size()>0) 
			if (dist_wings[i]!=D_NULL) out<< dist_wings[i]<<" "; else out<<"NULL"<<" ";
		if(lat_s.size()>0) 
			if (lat_s[i]!=D_NULL) out<< (int)lat_s[i]<<" "; else out<<"NULL"<<" ";
		if(long_s.size()>0) 
			if (long_s[i]!=D_NULL) out<< (int)long_s[i]<<" "; else out<<"NULL"<<" ";
		if(lat_f.size()>0) 
			if (lat_f[i]!=D_NULL) out<< (int)lat_f[i]<<" "; else out<<"NULL"<<" ";
		if(long_f.size()>0) 
			if (long_f[i]!=D_NULL) out<< (int)long_f[i]<<" "; else out<<"NULL"<<" ";
		if(nors_s.size()>0)
			out<<(nors_s[i]!=S_NULL? nors_s[i]:"NULL")<<" ";
		if(eorw_s.size()>0)
			out<<(eorw_s[i]!=S_NULL? eorw_s[i]:"NULL")<<" ";
		if(nors_f.size()>0)
			out<<(nors_f[i]!=S_NULL? nors_f[i]:"NULL")<<" ";
		if(eorw_f.size()>0)		
			out<<(eorw_f[i]!=S_NULL? eorw_f[i]:"NULL")<<" ";
		if(time_s.size()>0)
			if (time_s[i]!=D_NULL) out<< time_s[i]<<" "; else out<<"NULL"<<" ";
		if(date_s.size()>0)
			if (date_s[i]!=S_NULL) out<< date_s[i]<<" "; else out<<"NULL"<<" ";
		if(time_f.size()>0)
			if (time_f[i]!=D_NULL) out<< time_f[i]<<" "; else out<<"NULL"<<" ";
		if(date_f.size()>0)
			if (date_f[i]!=S_NULL) out<< date_f[i]<<" "; else out<<"NULL"<<" ";
		if(d.size()>0)
			if (d[i]!=D_NULL) out<<FMT_double(d[i],"dec_place",2)<<" "; else out<<"NULL"<<" ";
		if(w.size()>0)
			if (w[i]!=D_NULL) out<<FMT_double(w[i],"dec_place",1)<<" "; else out<<"NULL"<<" ";
		if(t.size()>0)
			if (t[i]!=D_NULL) out<<FMT_double(t[i],"dec_place",2)<<" "; else out<<"NULL"<<" ";
		if(C.size()>0){
			for(int j=0;j<p_DM->species.size();j++){
				if(C[i][j]!=D_NULL) out<<FMT_double(C[i][j],output_precision.type["density"],output_precision.precision["density"])<<" "; else out<<"NULL"<<" ";
			}
		}
		if(LD.size()>0){
			for(int j=0;j<p_DM->species.size();j++){
				if(LD[i][j]!=D_NULL) out<<FMT_double(LD[i][j],output_precision.type["density"],output_precision.precision["density"])<<" "; else out<<"NULL"<<" ";
			}
		}
		if(AD.size()>0){
			for(int j=0;j<p_DM->species.size();j++){
				if(AD[i][j]!=D_NULL) out<<FMT_double(AD[i][j],output_precision.type["density"],output_precision.precision["density"])<<" "; else out<<"NULL"<<" ";
			}
		}
		if(c.size()>0){
			for(int j=0;j<c[i].size();j++){
				for(int k=0;k<c[i][j].size();k++){
					if(c[i][j][k]!=D_NULL) out<<FMT_double(c[i][j][k],output_precision.type["density"],output_precision.precision["density"])<<" "; else out<<"NULL"<<" ";
					}
				}
		}
		if(ad.size()>0){
			for(int j=0;j<ad[i].size();j++){
				for(int k=0;k<ad[i][j].size();k++){
					if(ad[i][j][k]!=D_NULL) out<<FMT_double(ad[i][j][k],output_precision.type["density"],output_precision.precision["density"])<<" "; else out<<"NULL"<<" ";
					}
				}
		}
		if(weight_kg_km2.size()>0){
			for(int j=0;j<p_DM->species.size();j++){
				if(weight_kg_km2[i][j]!=D_NULL) out<<FMT_double(weight_kg_km2[i][j],output_precision.type["density"],output_precision.precision["density"])<<" "; else out<<"NULL"<<" ";
			}
		}
	
		out<<"\n";
	}

}

void DataManager::T_station_catch::empty()
{
	DEBUG2("DataManager::T_station_catch::empty");
	key=std::vector<std::string>();
	trip_code=std::vector<std::string>();
	stratum=std::vector<std::string>();
	station_no=std::vector<int>();
	distance=std::vector<double>();
	speed=std::vector<double>();
	dist_doors=std::vector<double>();
	dist_wings=std::vector<double>();
	lat_s=std::vector<double>();
	long_s=std::vector<double>();
	lat_f=std::vector<double>();
	long_f=std::vector<double>();
	nors_s=std::vector<std::string>();
	eorw_s=std::vector<std::string>();
	nors_f=std::vector<std::string>();
	eorw_f=std::vector<std::string>();
	time_s=std::vector<int>();
	date_s=std::vector<std::string>();
	time_f=std::vector<int>();
	date_f=std::vector<std::string>();
	d=std::vector<double>();
	w=std::vector<double>();
	t=std::vector<double>();
	C=std::vector<std::vector<double> >();
	LD=std::vector<std::vector<double> >();
	AD=std::vector<std::vector<double> >();
	c=std::vector<std::vector<std::vector<double> > >();
	ad=std::vector<std::vector<std::vector<double> > >();
	weight_kg_km2=std::vector<std::vector<double> >();

}

void DataManager::T_station_catch::insert(std::string _trip_code,std::string _stratum, int _station_no, 
						std::vector<double> _weight_kg_km2)
{
	DEBUG2("DataManager::T_station_catch::insert");
	std::string this_key=(trip_code_col!=-1? _trip_code+' ':"")+itos(_station_no);
	if(in(key,this_key))
		fatal("Duplicate key in station_catch table:"+this_key+ " already exists.");

	if(trip_code_col!=-1){
		if(_trip_code==S_NULL) fatal("'trip_code' "+_trip_code+" can not be NULL "+this_key);
		trip_code.push_back(_trip_code);
	}
	if(_stratum==S_NULL) fatal("'stratum' "+_stratum+" can not be NULL "+this_key);
	stratum.push_back(_stratum);
	if(_station_no==D_NULL) fatal("'station_no' "+itos(_station_no)+" can not be NULL "+this_key);
	station_no.push_back(_station_no);
	
	for(int i=0;i<_weight_kg_km2.size();i++){
		if(_weight_kg_km2[i]!=D_NULL && _weight_kg_km2[i]<0) 
			fatal(p_DM->species[i]+"_kg_km2"+ " has negative value of "+dtos(_weight_kg_km2[i])+" for "+this_key);
	}
	weight_kg_km2.push_back(_weight_kg_km2);
	key.push_back(this_key);
}

void DataManager::T_station_catch::insert(std::string _trip_code,std::string _stratum, int _station_no, 
					double _distance,double _speed, double _dist_doors,double _dist_wings,
					double _lat_s, double _long_s,double _lat_f,double _long_f,
					std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
					int _time_s,std::string _date_s,int _time_f,std::string _date_f)
{
	DEBUG2("DataManager::T_station_catch::insert");
	std::string this_key=(trip_code_col!=-1? _trip_code+' ':"")+itos(_station_no);
	if(in(key,this_key))
		fatal("Duplicate key in station_catch table:"+this_key+ " already exists.");

	if(trip_code_col!=-1){
		if(_trip_code==S_NULL) fatal("'trip_code' "+_trip_code+" can not be NULL "+this_key);
		trip_code.push_back(_trip_code);	
	}
	if(_stratum==S_NULL) fatal("'stratum' "+_stratum+" can not be NULL "+this_key);
	stratum.push_back(_stratum);
	if(_station_no==D_NULL) fatal("'station_no' "+itos(_station_no)+" can not be NULL "+this_key);
	station_no.push_back(_station_no);

	//No column checking as these are from interval calculations not from an input file.
	if(distance_col!=-1){
		if(_distance!=D_NULL && _distance<0) fatal("'distance' has negative value of "+dtos(_distance)+" for "+this_key);
		distance.push_back(_distance);
	}
	if(speed_col!=-1){
		if(_speed!=D_NULL && _speed<0) fatal("'speed' has negative value of "+dtos(_speed)+" for "+this_key);
		speed.push_back(_speed);
	}
	if(dist_doors_col!=-1){
		if(_dist_doors!=D_NULL && _dist_doors<0) fatal("'dist_doors' has negative value of "+dtos(_dist_doors)+" for "+this_key);
		dist_doors.push_back(_dist_doors);
	}
	if(dist_wings_col!=-1){
		if(_dist_wings!=D_NULL && _dist_wings<0) fatal("'dist_wings' has negative value of "+dtos(_dist_wings)+" for "+this_key);
		dist_wings.push_back(_dist_wings);
	}
	if(lat_s_col!=-1){
		lat_s.push_back(_lat_s);
	}
	if(long_s_col!=-1){
		long_s.push_back(_long_s);
	}
	if(lat_f_col!=-1){
		lat_f.push_back(_lat_f);
	}

	if(long_f_col!=-1){
		long_f.push_back(_long_f);
	}
	if(nors_s_col!=-1){
		nors_s.push_back(_nors_s);
	}
	if(eorw_s_col!=-1){
		eorw_s.push_back(_eorw_s);
	}
	if(nors_f_col!=-1){
		nors_f.push_back(_nors_f);
	}

	if(eorw_f_col!=-1){
		eorw_f.push_back(_eorw_f);
	}

	if(time_s_col!=-1){
		if(_time_s!=D_NULL && _time_s<0) fatal("'time_s' has negative value of "+dtos(_time_s)+" for "+this_key);
		time_s.push_back(_time_s);
	}
	if(date_s_col!=-1){
		date_s.push_back(_date_s);
	}
	if(time_f_col!=-1){
		if(_time_f!=D_NULL && _time_f<0) fatal("'time_f' has negative value of "+dtos(_time_f)+" for "+this_key);
		time_f.push_back(_time_f);
	}
	if(date_f_col!=-1){
		date_f.push_back(_date_f);
	}

	key.push_back(this_key);
}


void DataManager::T_station_catch::insert2(std::string _trip_code,std::string _stratum, int _station_no, 
					double _distance,double _speed, double _dist_doors,double _dist_wings,
					double _lat_s, double _long_s,double _lat_f,double _long_f,
					std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
					int _time_s,std::string _date_s,int _time_f,std::string _date_f,
					std::vector<double> _C)
{
	DEBUG2("DataManager::T_station_catch::insert2");
	insert(_trip_code,_stratum,_station_no,_distance,_speed,_dist_doors,_dist_wings,
			_lat_s,_long_s,_lat_f,_long_f,_nors_s,_eorw_s,_nors_f,_eorw_f,_time_s,_date_s,_time_f,_date_f);
	C.push_back(_C);
}

void DataManager::T_station_catch::insert3(std::string _trip_code,std::string _stratum, int _station_no, 
					double _distance,double _speed, double _dist_doors,double _dist_wings,
					double _lat_s, double _long_s,double _lat_f,double _long_f,
					std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
					int _time_s,std::string _date_s,int _time_f,std::string _date_f,
					double _t, std::vector<double> _C)
{
	DEBUG2("DataManager::T_station_catch::insert3");
	insert(_trip_code,_stratum,_station_no,_distance,_speed,_dist_doors,_dist_wings,
			_lat_s,_long_s,_lat_f,_long_f,_nors_s,_eorw_s,_nors_f,_eorw_f,_time_s,_date_s,_time_f,_date_f);
	t.push_back(_t);
	C.push_back(_C);
}

void DataManager::T_station_catch::insert4(std::string _trip_code,std::string _stratum, int _station_no, 
					double _distance,double _speed, double _dist_doors,double _dist_wings,
					double _lat_s, double _long_s,double _lat_f,double _long_f,
					std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
					int _time_s,std::string _date_s,int _time_f,std::string _date_f,
					double _area_fished,std::vector<double> _C,std::vector<double> _AD)
{
	DEBUG2("DataManager::T_station_catch::insert4");
	insert(_trip_code,_stratum,_station_no,_distance,_speed,_dist_doors,_dist_wings,
			_lat_s,_long_s,_lat_f,_long_f,_nors_s,_eorw_s,_nors_f,_eorw_f,_time_s,_date_s,_time_f,_date_f);
	area_fished.push_back(_area_fished);
	C.push_back(_C);
	AD.push_back(_AD);
}

void DataManager::T_station_catch::insert5(std::string _trip_code,std::string _stratum, int _station_no, 
					double _distance,double _speed, double _dist_doors,double _dist_wings,
					double _lat_s, double _long_s,double _lat_f,double _long_f,
					std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
					int _time_s,std::string _date_s,int _time_f,std::string _date_f,
					double _d, double _w,std::vector<double> _C, std::vector<double> _LD,std::vector<double> _AD)
{
	DEBUG2("DataManager::T_station_catch::insert5");
	insert(_trip_code,_stratum,_station_no,_distance,_speed,_dist_doors,_dist_wings,
			_lat_s,_long_s,_lat_f,_long_f,_nors_s,_eorw_s,_nors_f,_eorw_f,_time_s,_date_s,_time_f,_date_f);
	d.push_back(_d);
	w.push_back(_w);
	C.push_back(_C);
	LD.push_back(_LD);
	AD.push_back(_AD);
}

void DataManager::T_station_catch::insert5(std::string _trip_code,std::string _stratum, int _station_no, 
					double _distance,double _speed, double _dist_doors,double _dist_wings,
					double _lat_s, double _long_s,double _lat_f,double _long_f,
					std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
					int _time_s,std::string _date_s,int _time_f,std::string _date_f,
					double _d, double _w,std::vector<double> _C, std::vector<double> _LD,std::vector<double> _AD,
					std::vector<std::vector<double> > _c,std::vector<std::vector<double> > _ad)
{
	DEBUG2("DataManager::T_station_catch::insert5");
	insert(_trip_code,_stratum,_station_no,_distance,_speed,_dist_doors,_dist_wings,
			_lat_s,_long_s,_lat_f,_long_f,_nors_s,_eorw_s,_nors_f,_eorw_f,_time_s,_date_s,_time_f,_date_f);
	d.push_back(_d);
	w.push_back(_w);
	C.push_back(_C);
	LD.push_back(_LD);
	AD.push_back(_AD);
	c.push_back(_c);
	ad.push_back(_ad);

}
void DataManager::T_station_catch::modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum)
{
	DEBUG2("DataManager::T_station_catch::modify_stratum");

	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==from_stratum) :  stratum[i]==from_stratum){
			std::string to_key=(trip_code_col!=-1? _trip_code+' ':"")+to_stratum+' '+itos(station_no[i]);
			if(in(key,to_key))
				fatal("(trip_code)+stratum+station_no "+to_key+" already exists in station_catch table when attempting to change the strata.");
			stratum[i]=to_stratum;
			key[i]=to_key;
		}
	}


}

void DataManager::T_station_catch::modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum)
{
	DEBUG1("DataManager::modify_station_no_stratum");
	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && station_no[i]==_station_no) :  station_no[i]==_station_no){
			std::string to_key=(trip_code_col!=-1? _trip_code+' ':"")+_stratum+' '+itos(station_no[i]);
			if(in(key,to_key))
				fatal("(trip_code)+stratum+station_no "+to_key+" already exists in station_catch table when attempting to reassign station strata.");
			stratum[i]=_stratum;
			key[i]=to_key;
		}
	}
}

std::vector<int> DataManager::T_station_catch::select_station_no(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_station_catch::select_station_no");
	std::vector<int> results;
	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(station_no[i]);
		}
	}
	return results;
}


std::vector<int> DataManager::T_station_catch::select_station_no(std::string _trip_code)
{
	DEBUG1("DataManager::T_station_catch::select_station_no");
	std::vector<int> results;
	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code):true){
				results.push_back(station_no[i]);
		}
	}
	return results;
}

std::vector<double> DataManager::T_station_catch::select_weight_kg_km2(std::string _trip_code,std::string _stratum,std::string _species)
{
	DEBUG1("DataManager::T_station_catch::select_station_no");
	std::vector<double> results;
	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(weight_kg_km2[i][pos(p_DM->species,_species)]);
		}
	}
	return results;
}


