// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* t_stratum_cpp_id = "$Id: t_stratum.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "datamanager.h"


DataManager::T_stratum::T_stratum(DataManager* p_DM,std::string input_file,std::string output_file):T_table(p_DM,input_file,output_file){
	DEBUG0("DataManager::T_stratum::T_stratum");
	
}


void DataManager::T_stratum::input(Parameter_set& p){
	DEBUG0("DataManager::T_stratum::input");
	if(input_file.empty()){
		if(p_DM->input_mode=="from_file")
			fatal("The program is attemping to extract stratum data from database,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
		else 
			p_DM->input_mode="from_DB";
		#ifndef __MINGW32__
			input_from_DB(p);
		#else
			//fatal("Can not input data for t_stratum table from trawl database on Microsoft Windows.");
			input_from_DB(p);
		#endif
	} else  {
		if(p_DM->input_mode=="from_DB")
			fatal("The program is attemping to extract stratum data from text file,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
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
		fatal("Can not input data for t_stratum table.");
	}
	input_from_p(p);
}

void DataManager::T_stratum::input_from_DB(Parameter_set& p){
	DEBUG0("DataManager::T_stratum::input_from_DB");

	input_file="t_stratum";
	std::string batch_file="./xxx_stratum";
	std::string log_file="./yyy_stratum";


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
	_select="select trip_code,stratum, area_km2 from "+(database=="Postgresql"? p.get_string("input_from_database.schema","trawl")+"."+p.get_string("input_from_database.t_stratum_table","t_stratum"):p.get_string("input_from_database.t_stratum_table","t_stratum"));
	
	if(p.get_command_count("where")){
		std::vector<std::string> where_labels=p.get_command_labels("where");
		if(where_labels.size()==1 && where_labels[0]==""){
			_where="where trip_code in (";
	        for(int i=0;i<p_DM->trips.size();i++){
		        _where+=(i!=p_DM->trips.size()-1)? ("'"+p_DM->trips[i]+"',"):("'"+p_DM->trips[i]+"'");
	        }
		    _where+=")";
			if(p.present("where.t_stratum"))
				_where+=" and ("+string_vector_to_string(p.get_string_vector("where.t_stratum"))+")";
		} else {
			for(int i=0;i<where_labels.size();i++){
				if(!in(p_DM->trips,where_labels[i]))
					fatal("The label "+where_labels[i]+" of where is not one of the defined trips.");
			}
			_where="where ";
			for(int i=0;i<p_DM->trips.size();i++){
				if(in(where_labels,p_DM->trips[i])){
					if(p.present("where["+p_DM->trips[i]+"].t_stratum"))
						_where+= ("(trip_code='"+p_DM->trips[i]+"' and ("+string_vector_to_string(p.get_string_vector("where["+p_DM->trips[i]+"].t_stratum"))+"))");
					else 
						_where+= ("(trip_code='"+p_DM->trips[i]+"')");
				} else {
					_where+=("(trip_code='"+p_DM->trips[i]+"')");
				}	                                       
				if(i!=p_DM->trips.size()-1) _where+=" or ";
			}
		}
	} else {
	    _where="where trip_code in (";
	    for(int i=0;i<p_DM->trips.size();i++){
		    _where+=(i!=p_DM->trips.size()-1)? ("'"+p_DM->trips[i]+"',"):("'"+p_DM->trips[i]+"'");
	    }
	    _where+=")";
	}


	sql=_select+' '+_where;

	cout<<"SQL for extracting stratum data:\n";
	cout<<sql<<"\n\n";

	if(!system(0))
		fatal("command interpreter not available!");

	if(database=="Postgresql"){
		ofstream bat_file(batch_file.c_str());
		bat_file<<"\\a\n\\f ' '\n"<<sql<<"\n";
		bat_file.close();
		cerr<<"user:"<<user<<"\n";
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


void DataManager::T_stratum::input_from_file(ifstream& in){
	DEBUG0("DataManager::T_stratum::input_from_file");
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
	area_km2_col=pos(header,"area_km2");
	if(area_km2_col==-1)
		fatal("Can not find 'area_km2' in "+input_file);

	int ncols=2+(trip_code_col!=-1); /*+(areal_availability_col!=-1)+(population_area_col!=-1)*/
	for(int i=1;i<lines.size();i++){
		std::vector<std::string> values=string_to_string_vector(lines[i]);
		if(values.size()!=header.size()){
			warning("Skip line "+itos(i)+" from file "+input_file+":\n"+lines[i]+"\n");
			//warning("There should be "+itos(ncols) +" fields in file "+input_file+" but only "+itos(values.size()) +" are found in line "+itos(i)+":\n"+lines[i]+"\nThe line is ignored!\n");
			continue;
		}
		insert(trip_code_col!=-1? (values[trip_code_col]!="NULL"? values[trip_code_col]:S_NULL):S_NULL,
				values[stratum_col]!="NULL"? values[stratum_col]:S_NULL,
				values[area_km2_col]!="NULL"? stod(values[area_km2_col],"'area_km2' has a value of "+ values[area_km2_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL);
	}

}

void DataManager::T_stratum::input_from_p(Parameter_set& p){
	DEBUG0("DataManager::T_stratum::input_from_p");
	if(p.get_command_count("new_strata")>0){
		std::vector<std::string> labels=p.get_command_labels("new_strata");
		cout<<"labels:"<<labels<<" "<<labels.size()<<"\n";
		if(labels.size()==1 && labels[0]==""){
			if(p_DM->trips.size()>1)
				fatal("You must define new_strata with a trip_code as the label if there are more than two trips.");
			std::string command = "new_strata.";
			std::vector<std::string> strata= p.get_string_vector(command+"strata");
			std::vector<double> areas= p.get_double_vector(command+"areas");
			if(areas.size()!=strata.size())
				fatal(command+"areas is not equal to that of "+command+".strata.");
			for(int j=0;j<strata.size();j++){
				insert(trip_code_col!=-1?p_DM->trips[0]:S_NULL,strata[j],areas[j]);
			}	
		} else {
			std::vector<std::string> labels=p.get_command_labels("new_strata");
			for(int i=0; i<labels.size();i++){
				std::string command = "new_strata["+labels[i]+"].";
				if(!in(p_DM->trips,labels[i]))
					fatal("The label of "+command+" is not one of the specified trips");
				std::vector<std::string> strata= p.get_string_vector(command+"strata");
				std::vector<double> areas= p.get_double_vector(command+"areas");
				if(areas.size()!=strata.size())
					fatal(command+"areas is not equal to that of "+command+".strata.");
				for(int j=0;j<strata.size();j++){
					insert(trip_code_col!=-1?labels[i]:S_NULL,strata[j],areas[j]);
				}
			}
		}
	}
}

void DataManager::T_stratum::output_to_file(ostream& out,Output_precision& output_precision){
	DEBUG0("DataManager::T_stratum::output_to_file");
	//write header                            
	if(trip_code.size()>0)
		out<<"trip_code"<<" ";
	out<<"stratum"<<" ";
	out<<"area_km2"<<" "<<"\n";
	for(int i=0;i<stratum.size();i++){
		if(trip_code.size()>0)
			out<<trip_code[i]<<" ";
		out<<stratum[i]<<" ";
		if (area_km2[i]!=D_NULL) out<< area_km2[i]<<"\n"; else out<<"NULL"<<"\n";
	}
}



void DataManager::T_stratum::empty()
{
	DEBUG2("DataManager::T_stratum::empty");
	key=std::vector<std::string>();
	trip_code=std::vector<std::string>();
	stratum=std::vector<std::string>();
	area_km2=std::vector<double>();
}

void DataManager::T_stratum::insert(std::string _trip_code,std::string _stratum, double _area_km2)
{
	DEBUG2("DataManager::T_stratum::insert");
	std::string this_key=(trip_code_col!=-1? _trip_code+' ':"") +_stratum;
	if(in(key,this_key))
		fatal("Duplicate key in stratum table:"+this_key+ " already exists.");
	if(trip_code_col!=-1){
		if(_trip_code==S_NULL) fatal("'trip_code' "+_trip_code+" can not be NULL "+this_key);
		trip_code.push_back(_trip_code);
	}
	if(_stratum==S_NULL) fatal("'stratum' "+_stratum+" can not be NULL "+this_key);
	stratum.push_back(_stratum);
	if(_area_km2==D_NULL) fatal("'area_km2' "+dtos(_area_km2)+" can not be NULL "+this_key);
	if(_area_km2<=0) fatal("'area_km2' "+dtos(_area_km2)+" must be positive for "+this_key);
	area_km2.push_back(_area_km2);

	key.push_back(this_key);
}

void DataManager::T_stratum::modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum)
{
	DEBUG2("DataManager::T_stratum::modify_stratum");
	std::string this_key=(trip_code_col!=-1? _trip_code+' ':"") +from_stratum;
	std::string that_key=(trip_code_col!=-1? _trip_code+' ':"") +to_stratum;
	int this_position=pos(key,this_key);
	if(this_position==-1)
		fatal("Can not locate the (trip_code)+stratum "+this_key+" in stratum table when attempting to change the stratum.");
	int that_position=pos(key,that_key);
	if(that_position==-1){
		key[this_position] = that_key;
		stratum[this_position]=to_stratum;
	} else {
		//any better idea?
		std::vector<std::string>::iterator iter_1(key.begin());
		std::vector<std::string>::iterator iter_2(stratum.begin());
		std::vector<double>::iterator iter_3(area_km2.begin());
		std::vector<std::string>::iterator iter_4(trip_code.begin());
		for(int loc=0; loc<key.size();loc++){
			if(key.at(loc)==this_key){
				key.erase(iter_1);
				stratum.erase(iter_2);
				area_km2.erase(iter_3);
				if(trip_code_col != -1) trip_code.erase(iter_4);
				break;
			}
			iter_1++;iter_2++;iter_3++;if(trip_code_col != -1) iter_4++;
		}
	} 
}

void DataManager::T_stratum::modify_area_km2(std::string _trip_code,std::string _stratum,double _area_km2)
{
	DEBUG2("DataManager::T_stratum::modify_area_km2");
	std::string this_key=(trip_code_col!=-1? _trip_code+' ':"") +_stratum;
	int position=pos(key,this_key);
	if(position==-1)
		fatal("Can not locate the (trip_code)+stratum "+this_key+" in stratum table when attempting to change the stratum areas.");
	if(_area_km2<=0)
		fatal("'area_km2'"+dtos(_area_km2)+" must be positive for "+this_key);
	area_km2[position]=_area_km2;

}


std::vector<std::string> DataManager::T_stratum::select_stratum(std::string _trip_code)
{
	DEBUG1("DataManager::T_stratum::select_strata");
	std::vector<std::string> results;
	for(int i=0;i<stratum.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code):(true)){
				results.push_back(stratum[i]);
		}
	}
	return results;
}


std::vector<double> DataManager::T_stratum::select_area_km2(std::string _trip_code)
{
	DEBUG1("DataManager::T_stratum::select_area_km2");
	std::vector<double> results;
	for(int i=0;i<area_km2.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code):(true)){
				results.push_back(area_km2[i]);
		}
	}
	return results;
}

std::vector<double> DataManager::T_stratum::select_area_km2(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_stratum::select_area_km2");
	std::vector<double> results;
	for(int i=0;i<area_km2.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(area_km2[i]);
		}
	}
	return results;
}