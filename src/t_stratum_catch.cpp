// const char* time_stamp = "$Date: 2008/08/14 04:53:06 $\n";
// const char* t_stratum_catch_cpp_id = "$Id: t_stratum_catch.cpp,v 1.1 2008/08/14 04:53:06 fud Exp $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "datamanager.h"



DataManager::T_stratum_catch::T_stratum_catch(DataManager* p_DM,std::string input_file,std::string output_file):T_table(p_DM,input_file,output_file){
	DEBUG0("DataManager::T_stratum_catch::T_stratum_catch");
	

}

void DataManager::T_stratum_catch::set_cols(int _trip_code_col,int _stratum_col,int _area_km2_col){
	DEBUG0("DataManager::T_stratum_catch::set_cols");
	trip_code_col=_trip_code_col;
	stratum_col=_stratum_col;
	area_km2_col=_area_km2_col;
}


void DataManager::T_stratum_catch::output_to_file(ostream& out,Output_precision& output_precision){
	DEBUG0("DataManager::T_stratum_catch::output_to_file");
	//write header                            
	out<<"trip_code"<<" ";
	out<<"stratum"<<" ";
	out<<"area_km2"<<" ";
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
	if(AD.size()>0){
		for(int j=0;j<p_DM->species.size();j++){
			out<<p_DM->species[j]+"_kg_km2"<<" ";
		}
	}
	out<<"\n";

	for(int i=0;i<stratum.size();i++){
		out<<trip_code[i]<<" ";
		out<<stratum[i]<<" ";
		if (area_km2[i]!=D_NULL) out<< area_km2[i]<<" "; else out<<"NULL"<<" ";
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
		out<<"\n";
	}
}



void DataManager::T_stratum_catch::empty()
{
	DEBUG2("DataManager::T_stratum_catch::empty");
	key=std::vector<std::string>();
    trip_code=std::vector<string>();
    stratum=std::vector<string>();
    area_km2=std::vector<double>();
    C=std::vector<std::vector<double> >() ;  //mean catch rates
    LD=std::vector<std::vector<double> >(); //mean LD
    AD=std::vector<std::vector<double> >(); //mean AD;
}



void DataManager::T_stratum_catch::insert3(std::string _trip_code,std::string _stratum, double _area_km2,
					std::vector<double>  _AD)
{
	DEBUG2("DataManager::T_stratum_catch::insert3");
	std::string this_key= _trip_code+' ' +_stratum;
	if(in(key,this_key))
		fatal("Duplicate key in stratum_catch table:"+this_key+ " already exists.");
	if(trip_code_col!=-1){
		if(_trip_code==S_NULL) fatal("'trip_code' "+_trip_code+" can not be NULL "+this_key);
		trip_code.push_back(_trip_code);
	}
	if(_stratum==S_NULL) fatal("'stratum' "+_stratum+" can not be NULL "+this_key);
	stratum.push_back(_stratum);
	if(_area_km2==D_NULL) fatal("'area_km2' "+dtos(_area_km2)+" can not be NULL "+this_key);
	if(_area_km2<=0) fatal("'area_km2' "+dtos(_area_km2)+" must be positive for "+this_key);
	area_km2.push_back(_area_km2);

	AD.push_back(_AD);

	key.push_back(this_key);

}
void DataManager::T_stratum_catch::insert2(std::string _trip_code,std::string _stratum, double _area_km2)
{
	DEBUG2("DataManager::T_stratum::insert2");
	std::string this_key= _trip_code+' ' +_stratum;
	if(in(key,this_key))
		fatal("Duplicate key in stratum_catch table:"+this_key+ " already exists.");
	if(trip_code_col!=-1){
		if(_trip_code==S_NULL) fatal("'trip_code' "+_trip_code+" can not be NULL "+this_key);
		trip_code.push_back(_trip_code);
	}
	trip_code.push_back(_trip_code);
	if(_stratum==S_NULL) fatal("'stratum' "+_stratum+" can not be NULL "+this_key);
	stratum.push_back(_stratum);
	if(_area_km2==D_NULL) fatal("'area_km2' "+dtos(_area_km2)+" can not be NULL "+this_key);
	if(_area_km2<=0) fatal("'area_km2' "+dtos(_area_km2)+" must be positive for "+this_key);
	area_km2.push_back(_area_km2);
	key.push_back(this_key);

}
void DataManager::T_stratum_catch::insert2(std::string _trip_code,std::string _stratum, double _area_km2,
					std::vector<double>  _C)
{
	DEBUG2("DataManager::T_stratum::insert2");
	insert2(_trip_code,_stratum,_area_km2);
	C.push_back(_C);

}
void DataManager::T_stratum_catch::insert2(std::string _trip_code,std::string _stratum, double _area_km2,
			std::vector<double>  _C,std::vector<double>  _AD)
{
	DEBUG2("DataManager::T_stratum::insert2");
	insert2(_trip_code,_stratum,_area_km2);
	C.push_back(_C);
	AD.push_back(_AD);

}
void DataManager::T_stratum_catch::insert2(std::string _trip_code,std::string _stratum, double _area_km2,
			std::vector<double>  _C,std::vector<double>  _LD,std::vector<double>  _AD)
{
	DEBUG2("DataManager::T_stratum::insert2");
	insert2(_trip_code,_stratum,_area_km2);
	C.push_back(_C);
	LD.push_back(_LD);
	AD.push_back(_AD);
}
