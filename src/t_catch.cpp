// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* t_catch_cpp_id = "$Id: t_catch.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "datamanager.h"


void DataManager::T_catch::empty()
{
	DEBUG2("DataManager::T_catch::empty");
	key=std::vector<std::string>();
	trip_code=std::vector<std::string>();
	stratum=std::vector<std::string>();
	station_no=std::vector<int>();
	species=std::vector<std::string>();
	weight=std::vector<double>();
}

DataManager::T_catch::T_catch(DataManager* p_DM,std::string input_file,std::string output_file):T_table(p_DM,input_file,output_file){
	DEBUG0("DataManager::T_catch::T_catch");

}

void DataManager::T_catch::input(Parameter_set& p){
	DEBUG0("DataManager::T_catch::input");
	if(input_file.empty()){
		if(p_DM->input_mode=="from_file")
			fatal("The program is attemping to extract catch data from database,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
		else 
			p_DM->input_mode="from_DB";
		#ifndef __MINGW32__
			input_from_DB(p);
		#else
			//fatal("Can not input data for t_catch table from trawl database on Microsoft Windows.");
			input_from_DB(p);
		#endif
	} else {
		if(p_DM->input_mode=="from_DB")
			fatal("The program is attemping to extract catch data from text file,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
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
		fatal("Can not input data for T_catch table.");
	}
}
void DataManager::T_catch::input_from_DB(Parameter_set& p){
	DEBUG0("DataManager::T_catch::input_from_DB");
	input_file="t_catch";
	std::string batch_file="./xxx_catch";
	std::string log_file ="./yyy_catch";
	std::string database = p.get_string("input_from_database.database","postgresql");
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
		_select="select trip_code,station_no,species,coalesce(to_char(weight,'9999999D9'),'NULL') as weight ";
	} else {
		_select="select trip_code,station_no,species,weight ";
	}
	_select+="from "+(database=="Postgresql"? p.get_string("input_from_database.schema","trawl")+"."+p.get_string("input_from_database.t_catch_table","t_catch"):p.get_string("input_from_database.t_catch_table","t_catch"));

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
			if(p.present("where.t_catch"))
				_where+=" and ("+string_vector_to_string(p.get_string_vector("where.t_catch"))+")";
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
			       if(p.present("where["+p_DM->trips[i]+"].t_catch"))
			           _where+=") and ("+string_vector_to_string(p.get_string_vector("where["+p_DM->trips[i]+"].t_catch"));
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

	cout<<"SQL for extracting catch data:\n";
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

void DataManager::T_catch::input_from_file(ifstream& in){
	DEBUG0("DataManager::T_catch::input_from_file");
	std::vector<std::string> lines=read_file(in);
	if(lines.size()==0)
		fatal("There is no data "+input_file);
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
	weight_col=pos(header,"weight");
	if(weight_col==-1)
		fatal("Can not find 'weight' in "+input_file);
	

	
	int ncols=2+(trip_code_col!=-1)+(stratum_col!=-1)+(species_col!=-1);
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
				values[weight_col]!="NULL"? stod(values[weight_col],"'weight' has a value of "+ values[weight_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL);
	}
}

void DataManager::T_catch::output_to_file(ostream& out,Output_precision& output_precision){
	DEBUG0("DataManager::T_catch::output_to_file");
	if(trip_code.size()>0)
		out<<"trip_code"<<" ";
	if(stratum.size()>0)
		out<<"stratum"<<" ";
	out<<"station_no"<<" ";
	if(species.size()>0)
		out<<"species"<<" ";
	out<<"weight"<<" ";
	out<<"\n";
	for(int i=0;i<station_no.size();i++){
		if(trip_code.size()>0)
			out<<trip_code[i]<<" ";
		if(stratum.size()>0)
			out<<stratum[i]<<" ";
		out<<station_no[i]<<" ";
		if(species.size()>0)
			out<<species[i]<<" ";
		if (weight[i]!=D_NULL) out<< weight[i]<<" "; else out<<0<<" ";
		out<<"\n";
	}
}

void DataManager::T_catch::insert(std::string _trip_code,std::string _stratum,  int _station_no, std::string _species,double _weight)
{
	DEBUG2("DataManager::T_catch::insert");
	std::string this_key=(trip_code_col!=-1? _trip_code+' ':"")+itos(_station_no)+(species_col!=-1? ' '+_species:"");
	/* t_catch table has no primary_key 
	if(in(key,this_key)){
		fatal("Duplicate key in catch table:"+this_key+ " already exists.");
	}
	*/
	if(trip_code_col!=-1){
		if(_trip_code==S_NULL) fatal("'trip_code' "+_trip_code+" can not be NULL "+this_key);
		trip_code.push_back(_trip_code);
	}
	if(stratum_col !=-1){
		stratum.push_back(_stratum);
	}
	if(_station_no==D_NULL) fatal("'station_no' "+itos(_station_no)+" can not be NULL "+this_key);
	station_no.push_back(_station_no);
	if(species_col !=-1){
		if(_species==S_NULL) fatal("'species' "+_species+" can not be NULL "+this_key);
		species.push_back(_species);
	}

	if(_weight!=D_NULL && _weight<0) fatal("'weight' has negative value of "+dtos(_weight)+" for "+this_key);
	weight.push_back(_weight);

	/* t_catch table has no primary_key 
	key.push_back(this_key);
	*/

}


void DataManager::T_catch::modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum)
{
	DEBUG2("DataManager::T_catch::modify_stratum");
	if(stratum_col!=-1){
		for(int i=0;i<station_no.size();i++){
			if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==from_stratum) :  stratum[i]==from_stratum){
				stratum[i]=to_stratum;
			}
		}
	}
}


void DataManager::T_catch::modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum)
{
	DEBUG2("DataManager::T_catch::modify_station_no_stratum");
	if(stratum_col!=-1){
		for(int i=0;i<station_no.size();i++){
			if(trip_code_col!=-1? (trip_code[i]==_trip_code && station_no[i]==_station_no) :  station_no[i]==_station_no){
				stratum[i]=_stratum;
			}
		}
	}

}


std::vector<double> DataManager::T_catch::select_weight(std::string _trip_code,std::string _stratum,int _station_no,std::string _species)
{
	DEBUG1("DataManager::T_catch::select_weigh");
	std::vector<double> result;
	for(int i=0;i<weight.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (species_col!=-1? (species[i]==_species):true)&& (stratum_col!=-1? (stratum[i]==_stratum):true) &&  station_no[i]==_station_no ){
			result.push_back(weight[i]);
		}
	}
	return result;

}