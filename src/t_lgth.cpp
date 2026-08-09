// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* t_lgth_cpp_id = "$Id: t_lgth.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "datamanager.h"



DataManager::T_lgth::T_lgth(DataManager* p_DM,std::string input_file,std::string output_file):T_table(p_DM,input_file,output_file){
	DEBUG0("DataManager::T_lgth::T_lgth");
}


void DataManager::T_lgth::input(Parameter_set& p){
	DEBUG0("DataManager::T_lgth::input");
	if(input_file.empty()){
		if(p_DM->input_mode=="from_file")
			fatal("The program is attemping to extract lgth data from database,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
		else 
			p_DM->input_mode="from_DB";
		#ifndef __MINGW32__
			input_from_DB(p);
		#else
			//fatal("Can not input data for t_lgth table from trawl database on windows.");
			input_from_DB(p);
		#endif
	} else {
		if(p_DM->input_mode=="from_DB")
			fatal("The program is attemping to extract lgth data from text file,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
		else 
			p_DM->input_mode="from_file";
		if(p.get_command_count("input_from_database")>0){
			warning("Input file "+ input_file+" has been specified in command line, @input_from_database will be ingored.");
		}
	}
	if(!input_file.empty()){
		ifstream in(input_file.c_str());
		if (!in) fatal("Could not open file " + input_file);
		input_from_file(in);
		in.close();
	}  else {
		fatal("Can not input data for t_lgth table.");
	}
}

void DataManager::T_lgth::input_from_DB(Parameter_set& p){
	DEBUG0("DataManager::T_lgth::input_from_DB");
	input_file="t_lgth";
	std::string batch_file="./xxx_lgth";
	std::string log_file ="./yyy_lgth";
	std::string database = p.get_string("input_from_database.database","mpress");
	if((database!="postgresql" && database!="Postgresql" && database!= "POSTGRESQL") &&
		(database!="empress" && database!="Empress" && database!= "EMPRESS"))
		fatal("input_from_database.database must be either 'Postgresql' or 'Empress'");
	std::string hostname,user;
	if(database=="postgresql" || database=="Postgresql" || database== "POSTGRESQL"){
		database="Postgresql";
		hostname=p.get_string("input_from_database.hostname","");
		user=p.get_string("input_from_database.user","");
	} else {
		database="Empress";
	}

	std::string _select,_where;
	if(database=="Postgresql"){
		_select="select trip_code,station_no,species,subcatch_no,lgth,";
		_select+="coalesce(to_char(percent_samp,'99999D99'),'NULL') as percent_samp,";
		_select+="coalesce(to_char(no_a,'9999999999'),'NULL') as no_a,";
		_select+="coalesce(to_char(no_f,'9999999999'),'NULL') as no_f,";
		_select+="coalesce(to_char(no_m,'9999999999'),'NULL') as no_m ";
	} else {
		_select="select trip_code,station_no,species,subcatch_no,lgth,percent_samp,no_a,no_f,no_m ";
	}
	_select+="from "+(database=="Postgresql"? p.get_string("input_from_database.schema","trawl")+"."+p.get_string("input_from_database.t_lgth_table","t_lgth"):p.get_string("input_from_database.t_lgth_table","t_lgth"));
	if(p.get_command_count("where")){
		std::vector<std::string> where_labels=p.get_command_labels("where");
		if(where_labels.size()==1 && where_labels[0]==""){
	       _where="where ";
	       for(int i=0;i<p_DM->trip_species.size();i++){
		       _where+="(trip_code='"+p_DM->trips[i]+"'";
		       _where+=" and species in (";
		       for(int j=0;j<p_DM->trip_species[p_DM->trips[i]].size();j++){
			       _where+=(j!=p_DM->trip_species[p_DM->trips[i]].size()-1)? ("'"+p_DM->trip_species[p_DM->trips[i]][j]+"',"):("'"+p_DM->trip_species[p_DM->trips[i]][j]+"'");
		       }
		      _where+="))";
		      if(i!=p_DM->trips.size()-1) _where+=" or ";
		   }
			if(p.present("where.t_lgth"))
				_where+=" and ("+string_vector_to_string(p.get_string_vector("where.t_lgth"))+")";
		} else {
			for(int i=0;i<where_labels.size();i++){
				if(!in(p_DM->trips,where_labels[i]))
					fatal("The label "+where_labels[i]+" of where is not one of the defined trips.");
			}
			_where="where ";
	        for(int i=0;i<p_DM->trip_species.size();i++){
		       _where+="(trip_code='"+p_DM->trips[i]+"'";
		       _where+=" and species in (";
		       for(int j=0;j<p_DM->trip_species[p_DM->trips[i]].size();j++){
			       _where+=(j!=p_DM->trip_species[p_DM->trips[i]].size()-1)? ("'"+p_DM->trip_species[p_DM->trips[i]][j]+"',"):("'"+p_DM->trip_species[p_DM->trips[i]][j]+"'");
		       }
			   if(in(where_labels,p_DM->trips[i])){
			       if(p.present("where["+p_DM->trips[i]+"].t_lgth"))
			           _where+=") and ("+string_vector_to_string(p.get_string_vector("where["+p_DM->trips[i]+"].t_lgth"));
			   }
			   _where+= "))";
			   if(i!=p_DM->trips.size()-1) _where+=" or ";
			}
		}
	} else {
	       _where="where ";
	       for(int i=0;i<p_DM->trip_species.size();i++){
		       _where+="(trip_code='"+p_DM->trips[i]+"'";
		       _where+=" and species in (";
		       for(int j=0;j<p_DM->trip_species[p_DM->trips[i]].size();j++){
			       _where+=(j!=p_DM->trip_species[p_DM->trips[i]].size()-1)? ("'"+p_DM->trip_species[p_DM->trips[i]][j]+"',"):("'"+p_DM->trip_species[p_DM->trips[i]][j]+"'");
		       }
		      _where+="))";
		      if(i!=p_DM->trips.size()-1) _where+=" or ";
		   }
	}

	sql=_select+' '+_where;

	cout<<"SQL for extracting lgth data:\n";
	cout<<sql<<"\n\n";

	if(!system(0))
		fatal("command interpreter not available!");

	if(database=="Postgresql"){
		ofstream bat_file(batch_file.c_str());
		bat_file<<"\\a\n\\f ' '\n"<<sql<<"\n";
		bat_file.close();
		std::string cmd="psql -d "+p.get_string("input_from_database.database_name","fish")+" "+(hostname!=""? "-h "+hostname:"")+" "+(user!=""? "-U "+user:"")+" -f \""+batch_file+"\" -o "+input_file+" >"+log_file;
		if(system(cmd.c_str())){
			fatal("Failed to execute command '"+cmd+"'");
		}
		#ifndef __MINGW32__ 
		{
			unlink(batch_file.c_str());
			unlink(log_file.c_str());
		} 
		#else 
		{
			_unlink(batch_file.c_str());
			_unlink(log_file.c_str());
		} 
		#endif
	} else {
		ofstream bat_file(batch_file.c_str());
		bat_file<<"empbatch "+p.get_string("input_from_database.database_name","trawl")+"<<EOF\nset MSNULLVALUE 'NULL'\n"<<sql<<" into '"<<input_file<<"'\nEOF";
		bat_file.close();
		//change the batch file into an executable;
		std::string cmd="chmod +x "+batch_file;
		if(system(cmd.c_str())){
			fatal("Failed to execute command '"+cmd+"'");
		} 
		if(system(batch_file.c_str())){
			fatal("Failed to execute command '"+batch_file+"'");
		} 
		#ifndef __MINGW32__
			unlink(batch_file.c_str());
		#else
			_unlink(batch_file.c_str());
		#endif
	}
}



void DataManager::T_lgth::input_from_file(ifstream& in){
	DEBUG0("DataManager::T_lgth::input_from_file");
	std::vector<std::string> lines=read_file(in);
	if(lines.size()==0)
		fatal("There is no data  "+input_file);
	// Read in header
	std::vector<std::string> header=string_to_string_vector(lines[0]);
	trip_code_col=pos(header,"trip_code");
	if(trip_code_col==-1 && p_DM->trips.size()>1)
		fatal("Can not find 'trip_code' in "+input_file);
	stratum_col=pos(header,"stratum");
	//if(stratum_col==-1)
		//fatal("Can not find 'stratum' in "+input_file);
	station_no_col=pos(header,"station_no");
	if(station_no_col==-1)
		fatal("Can not find 'station_no' in "+input_file);
	species_col=pos(header,"species");
	if(species_col==-1 && p_DM->species.size()>1)
		fatal("Can not find 'species' in "+input_file);
	
	subcatch_no_col=pos(header,"subcatch_no");
	
	percent_samp_col=pos(header,"percent_samp");
	if(percent_samp_col==-1)
		fatal("Can not find 'percent_samp' in "+input_file);
	lgth_col=pos(header,"lgth");
	if(lgth_col==-1)
		fatal("Can not find 'lgth' in "+input_file);
	no_a_col=pos(header,"no_a");
	if(no_a_col==-1)
		fatal("Can not find 'no_a' in "+input_file);
	no_f_col=pos(header,"no_f");
	if(no_f_col==-1)
		fatal("Can not find 'no_f' in "+input_file);
	no_m_col=pos(header,"no_m");
	if(no_m_col==-1)
		fatal("Can not find 'no_m' in "+input_file);
	
	int ncols=6+(trip_code_col!=-1)+(species_col!=-1)+(stratum_col!=-1)+(subcatch_no_col!=-1);
	//Read in data
	for(int i=1;i<lines.size();i++){
		std::vector<std::string> values=string_to_string_vector(lines[i]);
		if(values.size()!=header.size()){
			warning("Skip line "+itos(i)+" from file "+input_file+":\n"+lines[i]+"\n");
			//warning("There should be "+itos(ncols) +" fields in file "+input_file+" but only "+itos(values.size()) +" are found in line "+itos(i)+":\n"+lines[i]+"\nThe line is ignored!\n");
			continue;
		}
		insert(trip_code_col!=-1? (values[trip_code_col]!="NULL"? values[trip_code_col]:S_NULL):S_NULL,
			stratum_col!=-1? (values[stratum_col]!="NULL"? values[stratum_col]:S_NULL):S_NULL,
				values[station_no_col]!="NULL"? stoi(values[station_no_col],"'station_no' has a value of "+ values[station_no_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,				
				species_col!=-1? (values[species_col]!="NULL"? values[species_col]:S_NULL):S_NULL,
				subcatch_no_col!=-1? (values[subcatch_no_col]!="NULL"? stoi(values[subcatch_no_col],"'subcatch_no' has a value of "+ values[subcatch_no_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL):D_NULL,
				values[lgth_col]!="NULL"? stoi(values[lgth_col],"'lgth' has a value of "+ values[lgth_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,								
				values[percent_samp_col]!="NULL"? stod(values[percent_samp_col],"'percent_samp' has a value of "+ values[percent_samp_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric)."):D_NULL,								
				values[no_a_col]!="NULL"? stoi(values[no_a_col],"'no_a' has a value of "+ values[no_a_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,								
				values[no_f_col]!="NULL"? stoi(values[no_f_col],"'no_f' has a value of "+ values[no_f_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,								
				values[no_m_col]!="NULL"? stoi(values[no_m_col],"'no_m' has a value of "+ values[no_m_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL);
	}
}
void DataManager::T_lgth::output_to_file(ostream& out,Output_precision& output_precision){
	DEBUG0("DataManager::T_lgth::output_to_file");
	if(trip_code.size()>0)
		out<<"trip_code"<<" ";
	if(stratum.size()>0)
		out<<"stratum"<<" ";
	out<<"station_no"<<" ";
	if(species.size()>0)
		out<<"species"<<" ";
	if(subcatch_no.size()>0)
		out <<"subcatch_no"<<" ";
	out<<"percent_samp"<<" ";
	out<<"lgth"<<" ";
	out<<"no_a"<<" ";
	out<<"no_f"<<" ";
	out<<"no_m"<<" ";
	out<<"\n";
	for(int i=0;i<lgth.size();i++){
		if(trip_code.size()>0)
			out<<trip_code[i]<<" ";
		if(stratum.size()>0)
			out<<stratum[i]<<" ";
		out<<station_no[i]<<" ";
		if(species.size()>0)
			out<<species[i]<<" ";
		if(subcatch_no.size()>0)
			out<<subcatch_no[i]<<" ";
		if (percent_samp[i]!=D_NULL) out<< percent_samp[i]<<" "; else out<<"NULL"<<" ";
		out<<lgth[i]<<" ";
		if (no_a[i]!=D_NULL) out<< no_a[i]<<" "; else out<<"NULL"<<" ";
		if (no_f[i]!=D_NULL) out<< no_f[i]<<" "; else out<<"NULL"<<" ";
		if (no_m[i]!=D_NULL) out<< no_m[i]<<" "; else out<<"NULL"<<" ";

		out<<"\n";
	}

}

void DataManager::T_lgth::empty()
{
	DEBUG2("DataManager::T_lgth::empty");
	key=std::vector<std::string>();
	trip_code=std::vector<std::string>();
	stratum=std::vector<std::string>();
	station_no=std::vector<int>();
	species=std::vector<std::string>();
	subcatch_no=std::vector<int>();
	percent_samp=std::vector<double>();
	lgth=std::vector<int>();
	no_a=std::vector<int>();
	no_f=std::vector<int>();
	no_m=std::vector<int>();
}

bool DataManager::T_lgth::subcatch_defined(std::string _species)
{
	DEBUG2("DataManager::T_lgth::subcatch_defined");
	if(subcatch_no_col!=-1){
		for(int i=0;i<subcatch_no.size();i++){
			if(_species==""){
				if(subcatch_no[i]!=1) return true;
			} else {
				if(subcatch_no[i]!=1 && species[i]==_species) return true;
			}
		}
	}
	return false;
}


void DataManager::T_lgth::insert(std::string _trip_code,std::string _stratum,int _station_no, std::string _species,int _subcatch_no,
					int _lgth,double _percent_samp,int _no_a,int _no_f,int _no_m)
{
	DEBUG2("DataManager::T_lgth::insert");
	this->insert(_trip_code,_stratum,_station_no,_species,_lgth,_percent_samp,_no_a,_no_f,_no_m);
	if(subcatch_no_col!=-1){
		if(_subcatch_no==D_NULL) fatal("'subcatch_no' "+itos(_subcatch_no)+" can not be NULL "+"for station "+itos(_station_no) +" in lgth table");
		if(_subcatch_no==0) fatal("'subcatch_no' "+itos(_subcatch_no)+" can not zero "+"for station "+itos(_station_no) +" in lgth table");	
		subcatch_no.push_back(_subcatch_no);
	}
}


void DataManager::T_lgth::insert(std::string _trip_code,std::string _stratum,int _station_no, std::string _species,
					int _lgth,double _percent_samp,int _no_a,int _no_f,int _no_m)
{
	DEBUG2("DataManager::T_lgth::insert");
	if(trip_code_col!=-1){
		if(_trip_code==S_NULL) fatal("'trip_code' "+_trip_code+" can not be NULL "+"for station "+itos(_station_no) +" in lgth table");
		trip_code.push_back(_trip_code);
	}
	if(stratum_col!=-1){
		stratum.push_back(_stratum);
	}
	if(_station_no==D_NULL) fatal("'station_no' "+itos(_station_no)+" can not be NULL "+"for station "+itos(_station_no) +" in lgth table");
	station_no.push_back(_station_no);
	if(_species==S_NULL) fatal("'species' "+_species+" can not be NULL "+"for station "+itos(_station_no) +" in lgth table");
	species.push_back(_species);
	if(_percent_samp!=D_NULL && _percent_samp<0) fatal("'percent_samp' has negative value of "+dtos(_percent_samp)+" for station "+itos(_station_no) +" in lgth table");
	percent_samp.push_back(_percent_samp);
	if(_lgth==D_NULL) fatal("'lgth' "+dtos(_lgth)+" can not be NULL "+"for station "+itos(_station_no) +" in lgth table");
	if(_lgth!=D_NULL && _lgth<0) fatal("'lgth' has negative value of "+dtos(_lgth)+" for station "+itos(_station_no) +" in lgth table");
	lgth.push_back(_lgth);
	if(_no_a!=D_NULL && _no_a<0) fatal("'no_a' has negative value of "+dtos(_no_a)+" for station "+itos(_station_no) +" in lgth table");
	no_a.push_back(_no_a);
	if(_no_f!=D_NULL && _no_f<0) fatal("'no_f' has negative value of "+dtos(_no_f)+" for station "+itos(_station_no) +" in lgth table");
	no_f.push_back(_no_f);
	if(_no_m!=D_NULL && _no_m<0) fatal("'no_m' has negative value of "+dtos(_no_m)+" for station "+itos(_station_no) +" in lgth table");
	no_m.push_back(_no_m);
	if(_no_a!=D_NULL && _no_f!=D_NULL && _no_m!=D_NULL && _no_a-_no_f-_no_m<0) fatal("no_a "+itos(_no_a)+" is less than the sum of no_f "+itos(_no_f)+" and no_m "+itos(_no_m) +" for station "+itos(_station_no) +" in lgth table");	

}



void DataManager::T_lgth::modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum)
{
	DEBUG2("DataManager::T_lgth::modify_stratum");
	//no key checking
	if(stratum_col!=-1){
		for(int i=0;i<station_no.size();i++){
			if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==from_stratum) :  stratum[i]==from_stratum){
				stratum[i]=to_stratum;
			}
		}
	}
}

void DataManager::T_lgth::modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum)
{
	DEBUG2("DataManager::T_lgth::modify_station_no_stratum");
	//no key checking
	if(stratum_col!=-1){
		for(int i=0;i<station_no.size();i++){
			if(trip_code_col!=-1? (trip_code[i]==_trip_code && station_no[i]==_station_no) :  station_no[i]==_station_no){
				stratum[i]=_stratum;
			}
		}
	}

}

std::vector<int> DataManager::T_lgth::select_lgth(std::string _trip_code,std::string _stratum,int _station_no,std::string _species)
{
	DEBUG1("DataManager::T_lgth::select_lgth");
	std::vector<int> result;
	for(int i=0;i<lgth.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (species_col!=-1? (species[i]==_species):true)&& (stratum_col!=-1? (stratum[i]==_stratum):true) &&  station_no[i]==_station_no ){
				result.push_back(lgth[i]);
		}
	}
	return result;
}

std::vector<int> DataManager::T_lgth::select_lgth(std::string _trip_code,std::string _species)
{
	DEBUG1("DataManager::T_lgth::select_lgth");
	std::vector<int> result;
	for(int i=0;i<lgth.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (species_col!=-1? (species[i]==_species):true)){
				result.push_back(lgth[i]);
		}
	}
	return result;
}

std::vector<int> DataManager::T_lgth::select_subcatch_no(std::string _trip_code,std::string _stratum,int _station_no,std::string _species){
	DEBUG1("DataManager::T_lgth::select_subcatch_no");
	std::vector<int> result;
	for(int i=0;i<subcatch_no.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (species_col!=-1? (species[i]==_species):true)&& (stratum_col!=-1? (stratum[i]==_stratum):true) &&  station_no[i]==_station_no ){
				result.push_back(subcatch_no[i]);
		}
	}
	return result;
}


std::vector<double> DataManager::T_lgth::select_percent_samp(std::string _trip_code,std::string _stratum,int _station_no,std::string _species)
{
	DEBUG1("DataManager::T_lgth::select_percent_samp");
	std::vector<double> result;
	for(int i=0;i<percent_samp.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (species_col!=-1? (species[i]==_species):true)&& (stratum_col!=-1? (stratum[i]==_stratum):true) &&  station_no[i]==_station_no ){
				result.push_back(percent_samp[i]);
		}
	}
	return result;
}
std::vector<int> DataManager::T_lgth::select_no_a(std::string _trip_code,std::string _stratum,int _station_no,std::string _species)
{
	DEBUG1("DataManager::T_lgth::select_no_a");
	std::vector<int> result;
	for(int i=0;i<no_a.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (species_col!=-1? (species[i]==_species):true)&& (stratum_col!=-1? (stratum[i]==_stratum):true) &&  station_no[i]==_station_no ){
				result.push_back(no_a[i]);
		}
	}
	return result;
}
std::vector<int> DataManager::T_lgth::select_no_f(std::string _trip_code,std::string _stratum,int _station_no,std::string _species)
{
	DEBUG1("DataManager::T_lgth::select_no_f");
	std::vector<int> result;
	for(int i=0;i<no_f.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (species_col!=-1? (species[i]==_species):true)&& (stratum_col!=-1? (stratum[i]==_stratum):true) &&  station_no[i]==_station_no ){
				result.push_back(no_f[i]);
		}
	}
	return result;
}
std::vector<int> DataManager::T_lgth::select_no_m(std::string _trip_code,std::string _stratum,int _station_no,std::string _species)
{
	DEBUG1("DataManager::T_lgth::select_no_m");
	std::vector<int> result;
	for(int i=0;i<no_m.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (species_col!=-1? (species[i]==_species):true)&& (stratum_col!=-1? (stratum[i]==_stratum):true) &&  station_no[i]==_station_no ){
				result.push_back(no_m[i]);
		}
	}
	return result;
}


