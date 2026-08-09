// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* t_station_cpp_id = "$Id: t_station.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "datamanager.h"

DataManager::T_station::T_station(DataManager* p_DM,std::string input_file,std::string output_file):T_table(p_DM,input_file,output_file){
	DEBUG0("DataManager::T_station::T_station");
}

void DataManager::T_station::set_cols(int _trip_code_col, int _stratum_col,int _station_no_col,
						int _distance_col, int _speed_col, int _dist_doors_col,int _dist_wings_col,
						int _lat_s_col, int _long_s_col, int _lat_f_col, int _long_f_col, 
						int _nors_s_col, int _eorw_s_col, int _nors_f_col,int _eorw_f_col,
						int _time_s_col, int _date_s_col, int _time_f_col,int _date_f_col){
	DEBUG0("DataManager::T_station_catch::set_cols");
	trip_code_col= _trip_code_col;
	stratum_col=_stratum_col;
	station_no_col=_station_no_col;
	distance_col=_distance_col;
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

void DataManager::T_station::input(Parameter_set& p){
	DEBUG0("DataManager::T_station::input");

	if(input_file.empty()){
		if(p_DM->input_mode=="from_file")
			fatal("The program is attemping to extract station data from database,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
		else 
			p_DM->input_mode="from_DB";

		#ifndef __MINGW32__
			input_from_DB(p);
		#else
			//fatal("Can not input data for t_station table from trawl database on Microsoft Windows.");
			input_from_DB(p);
		#endif
	} else {
		if(p_DM->input_mode=="from_DB")
			fatal("The program is attemping to extract station data from text file,while other data are supplied through text files. Make sure that stratum,station,catch and lgth data are supplied in the same fashion."); 
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
		fatal("Can not input data for t_station table.");
	}

}

void DataManager::T_station::input_from_DB(Parameter_set& p){
	DEBUG0("DataManager::T_station::input_from_DB");
	input_file="t_station";
	std::string batch_file="./xxx_station";
	std::string log_file="./yyy_station";
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
		_select="select trip_code,station_no,coalesce(stratum,'NULL') as stratum,";
		_select+=		"coalesce(to_char(distance,'9999D99'),'NULL') as distance,";
		_select+=		"coalesce(to_char(speed,'999D9'),'NULL') as speed,";
		_select+=		"coalesce(to_char(dist_doors,'9999D9'),'NULL') as dist_doors,";
		_select+=		"coalesce(to_char(dist_wings,'9999D9'),'NULL') as dist_wings,";
		_select+=		"coalesce(to_char(lat_s,'9999999999'),'NULL') as lat_s,";
		_select+=		"coalesce(to_char(long_s,'9999999999'),'NULL') as long_s,";
		_select+=		"coalesce(to_char(lat_f,'9999999999'),'NULL') as lat_f,";
		_select+=		"coalesce(to_char(long_f,'9999999999'),'NULL') as long_f,";
		_select+=		"coalesce(nors_s,'NULL') as nors_s,";
		_select+=		"coalesce(eorw_s,'NULL') as eorw_s,";
		_select+=		"coalesce(nors_f,'NULL') as nors_f,";
		_select+=		"coalesce(eorw_f,'NULL') as eorw_f,";
		_select+=		"coalesce(to_char(time_s,'99999'),'NULL') as time_s,";
		_select+=		"coalesce(to_char(time_f,'99999'),'NULL') as time_f,";
		_select+=		"coalesce(to_char(date_s,'DD-FMMM-YY'),'NULL') as date_s,";
		_select+=		"coalesce(to_char(date_f,'DD-FMMM-YY'),'NULL') as date_f ";;
	} else {
		_select="select trip_code,station_no,stratum,distance,speed,dist_doors,dist_wings,lat_s,long_s,lat_f,long_f,NorS_s as nors_s,EorW_s as eorw_s,NorS_f as nors_f,EorW_f as eorw_f,time_s,time_f,";
		_select+="convert dayof date_s to char concat '-' concat convert monthof date_s to char concat '-' concat convert yearof date_s to char as date_s,";
		_select+="convert dayof date_f to char concat '-' concat convert monthof date_f to char concat '-' concat convert yearof date_f to char as date_f ";
	}
	//Select user supplied columns
	std::vector<std::string> columns,supplied_columns;
	columns.push_back("trip_code"); columns.push_back("station_no");columns.push_back("stratum");
	columns.push_back("distance"); columns.push_back("speed"); columns.push_back("dist_doors"); columns.push_back("dist_wings");
	columns.push_back("lat_s"); columns.push_back("long_s"); columns.push_back("lat_f"); columns.push_back("long_f");
	columns.push_back("nors_s"); columns.push_back("eorw_s"); columns.push_back("nors_f");columns.push_back("eorw_f");
	columns.push_back("time_s");columns.push_back("date_s");columns.push_back("time_f");columns.push_back("date_f");
	if(p.present("input_from_database.t_station_columns")){
		supplied_columns=p.get_string_vector("input_from_database.t_station_columns");
	}
	for(int i=0;i<supplied_columns.size();i++){
		if(!in(columns,supplied_columns[i]))
			_select +=","+supplied_columns[i]+" ";
	}

	_select+="from "+(database=="Postgresql"? p.get_string("input_from_database.schema","trawl")+"."+p.get_string("input_from_database.t_station_table","t_station"):p.get_string("input_from_database.t_station_table","t_station"));
	
	if(p.get_command_count("where")){
		std::vector<std::string> where_labels=p.get_command_labels("where");
		if(where_labels.size()==1 && where_labels[0]==""){
			_where="where trip_code in (";
	        for(int i=0;i<p_DM->trips.size();i++){
		        _where+=(i!=p_DM->trips.size()-1)? ("'"+p_DM->trips[i]+"',"):("'"+p_DM->trips[i]+"'");
	        }
		    _where+=")";
			if(p.present("where.t_station"))
				_where+=" and ("+string_vector_to_string(p.get_string_vector("where.t_station"))+")";
		} else {
			for(int i=0;i<where_labels.size();i++){
				if(!in(p_DM->trips,where_labels[i]))
					fatal("The label "+where_labels[i]+" of where is not one of the defined trips.");
			}
			_where="where ";
			for(int i=0;i<p_DM->trips.size();i++){
				if(in(where_labels,p_DM->trips[i])){
					if(p.present("where["+p_DM->trips[i]+"].t_station"))
						_where+= ("(trip_code='"+p_DM->trips[i]+"' and ("+string_vector_to_string(p.get_string_vector("where["+p_DM->trips[i]+"].t_station"))+"))");
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

	cout<<"SQL for extracting station data:\n";
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


void DataManager::T_station::input_from_file(ifstream& in){
	DEBUG0("DataManager::T_station::input_from_file");
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
	distance_col=pos(header,"distance");
	speed_col=pos(header,"speed");
	dist_doors_col=pos(header,"dist_doors");
	dist_wings_col=pos(header,"dist_wings");
	lat_s_col=pos(header,"lat_s");
	long_s_col=pos(header,"long_s");
	lat_f_col=pos(header,"lat_f");
	long_f_col=pos(header,"long_f");
	nors_s_col=pos(header,"nors_s");
	eorw_s_col=pos(header,"eorw_s");
	nors_f_col=pos(header,"nors_f");
	eorw_f_col=pos(header,"eorw_f");
	time_s_col=pos(header,"time_s");
	date_s_col=pos(header,"date_s");
	time_f_col=pos(header,"time_f");
	date_f_col=pos(header,"date_f");

	
	int ncols=2+(trip_code_col!=-1)+(distance_col!=-1)+(speed_col!=-1)+(dist_doors_col!=-1)+(dist_wings_col!=-1)
				+(lat_s_col!=-1)+(long_s_col!=-1)+(lat_f_col!=-1)+(long_f_col!=-1)
				+(nors_s_col!=-1)+(eorw_s_col!=-1)+(nors_f_col!=-1)+(eorw_f_col!=-1)
				+(time_s_col!=-1)+(date_s_col!=-1)+(time_f_col!=-1)+(date_f_col!=-1);
	//Read in data
	for(int i=1;i<lines.size();i++){
		std::vector<std::string> values=string_to_string_vector(lines[i]);
		if(values.size()!=header.size()){
			warning("Skip line "+itos(i)+" from file "+input_file+":\n"+lines[i]+"\n");
			//warning("There should be "+itos(ncols) +" fields in file "+input_file+" but only "+itos(values.size()) +" are found in line "+itos(i)+":\n"+lines[i]+"\nThe line is ignored!\n");
			continue;
		}
		insert(trip_code_col!=-1? (values[trip_code_col]!="NULL"? values[trip_code_col]:S_NULL):S_NULL,
				values[stratum_col]!="NULL"? values[stratum_col]:S_NULL,
				values[station_no_col]!="NULL"? stoi(values[station_no_col],"'station_no' has a value of "+ values[station_no_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,
				distance_col!=-1? (values[distance_col]!="NULL"? stod(values[distance_col],"'distance' has a value of "+ values[distance_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric)."):D_NULL):D_NULL,
				speed_col!=-1? (values[speed_col]!="NULL"? stod(values[speed_col],"'speed' has a value of "+ values[speed_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL):D_NULL,
				dist_doors_col!=-1? (values[dist_doors_col]!="NULL"? stod(values[dist_doors_col],"'dist_doors' has a value of "+ values[dist_doors_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric)."):D_NULL):D_NULL,
				dist_wings_col!=-1? (values[dist_wings_col]!="NULL"? stod(values[dist_wings_col],"'dist_wings' has a value of "+ values[dist_wings_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric)."):D_NULL):D_NULL,
				lat_s_col!=-1? (values[lat_s_col]!="NULL"? stod(values[lat_s_col],"'lat_s' has a value of "+ values[lat_s_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL):D_NULL,
				long_s_col!=-1? (values[long_s_col]!="NULL"? stod(values[long_s_col],"'long_s' has a value of "+ values[long_s_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL):D_NULL,
				lat_f_col!=-1? (values[lat_f_col]!="NULL"? stod(values[lat_f_col],"'lat_f' has a value of "+ values[lat_f_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL):D_NULL,
				long_f_col!=-1? (values[long_f_col]!="NULL"? stod(values[long_f_col],"'long_f' has a value of "+ values[long_f_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL):D_NULL,
				nors_s_col!=-1? (values[nors_s_col]!="NULL"? values[nors_s_col]:S_NULL):S_NULL,
				eorw_s_col!=-1? (values[eorw_s_col]!="NULL"? values[eorw_s_col]:S_NULL):S_NULL,
				nors_f_col!=-1? (values[nors_f_col]!="NULL"? values[nors_f_col]:S_NULL):S_NULL,
				eorw_f_col!=-1? (values[eorw_f_col]!="NULL"? values[eorw_f_col]:S_NULL):S_NULL,
				time_s_col!=-1? (values[time_s_col]!="NULL"? stoi(values[time_s_col],"'time_s' has a value of "+ values[time_s_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL):D_NULL,
				date_s_col!=-1? (values[date_s_col]!="NULL"? values[date_s_col]:S_NULL):S_NULL,
				time_f_col!=-1? (values[time_f_col]!="NULL"? stoi(values[time_f_col],"'time_f' has a value of "+ values[time_f_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL):D_NULL,
				date_f_col!=-1? (values[date_f_col]!="NULL"? values[date_f_col]:S_NULL):S_NULL);
	}
}

void DataManager::T_station::output_to_file(ostream& out,Output_precision& output_precision){
	DEBUG0("DataManager::T_station::output_to_file");
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
			if (lat_s[i]!=D_NULL) out<< (int) lat_s[i]<<" "; else out<<"NULL"<<" ";
		if(long_s.size()>0) 
			if (long_s[i]!=D_NULL) out<< (int) long_s[i]<<" "; else out<<"NULL"<<" ";
		if(lat_f.size()>0) 
			if (lat_f[i]!=D_NULL) out<< (int) lat_f[i]<<" "; else out<<"NULL"<<" ";
		if(long_f.size()>0) 
			if (long_f[i]!=D_NULL) out<< (int) long_f[i]<<" "; else out<<"NULL"<<" ";
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
		out<<"\n";
	}

}

void DataManager::T_station::empty()
{
	DEBUG2("DataManager::T_station::empty");
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
}

void DataManager::T_station::insert(std::string _trip_code,std::string _stratum, int _station_no, 
					double _distance,double _speed, double _dist_doors,double _dist_wings,
					double _lat_s, double _long_s,double _lat_f,double _long_f,
					std::string _nors_s,std::string _eorw_s,std::string _nors_f,std::string _eorw_f,
					int _time_s,std::string _date_s,int _time_f,std::string _date_f)
{
	DEBUG2("DataManager::T_station::insert");
	std::string this_key=(trip_code_col!=-1? _trip_code+' ':"")+itos(_station_no);
	if(in(key,this_key))
		fatal("Duplicate key in station table:"+this_key+ " already exists.");

	if(trip_code_col!=-1){
		if(_trip_code==S_NULL) fatal("'trip_code' "+_trip_code+" can not be NULL "+this_key);
		trip_code.push_back(_trip_code);
	}
	//if(_stratum==S_NULL) fatal("'stratum' "+_stratum+" can not be NULL "+this_key);
	stratum.push_back(_stratum);
	if(_station_no==D_NULL) fatal("'station_no' "+itos(_station_no)+" can not be NULL "+this_key);
	station_no.push_back(_station_no);

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
		if(_eorw_s!=S_NULL && _eorw_s!="E" && _eorw_s!="W") fatal("'eorw_s' must be 'W' or 'E',but it has a value of  "+_eorw_s+" for "+this_key);
		eorw_s.push_back(_eorw_s);
	}
	if(nors_f_col!=-1){
		nors_f.push_back(_nors_f);
	}

	if(eorw_f_col!=-1){
		if(_eorw_f!=S_NULL && _eorw_f!="E" && _eorw_f!="W") fatal("'eorw_f' must be 'W' or 'E',but it has a value of  "+_eorw_f+" for "+this_key);
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



void DataManager::T_station::modify_stratum(std::string _trip_code,std::string from_stratum,std::string to_stratum)
{
	DEBUG2("DataManager::T_station::modify_stratum");

	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==from_stratum) :  stratum[i]==from_stratum){
			stratum[i]=to_stratum;
		}
	}


}

void DataManager::T_station::modify_station_no_stratum(std::string _trip_code,int _station_no,std::string _stratum)
{
	DEBUG1("DataManager::modify_station_no_stratum");
	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && station_no[i]==_station_no) :  station_no[i]==_station_no){
			stratum[i]=_stratum;
		}
	}
}

std::vector<int> DataManager::T_station::select_station_no(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_station::select_station_no");
	std::vector<int> results;
	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(station_no[i]);
		}
	}
	return results;
}

std::vector<int> DataManager::T_station::select_station_no(std::string _trip_code)
{
	DEBUG1("DataManager::T_station::select_station_no");
	std::vector<int> results;
	for(int i=0;i<station_no.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code):true){
				results.push_back(station_no[i]);
		}
	}
	return results;
}

std::vector<double> DataManager::T_station::select_distance(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_station::select_distance");
	std::vector<double> results;
	for(int i=0;i<distance.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(distance[i]);
		}
	}
	return results;

}
std::vector<double> DataManager::T_station::select_speed(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_station::select_speed");
	std::vector<double> results;
	for(int i=0;i<speed.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(speed[i]);
		}
	}
	return results;
}
std::vector<double> DataManager::T_station::select_dist_wings(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_station::select_dist_wings");
	std::vector<double> results;
	for(int i=0;i<dist_wings.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(dist_wings[i]);
		}
	}
	return results;
}

std::vector<double> DataManager::T_station::select_dist_doors(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_station::select_dist_doors");
	std::vector<double> results;
	for(int i=0;i<dist_doors.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(dist_doors[i]);
		}
	}
	return results;
}
std::vector<double> DataManager::T_station::select_lat_s(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_station::select_lat_s");
	std::vector<double> results;
	for(int i=0;i<lat_s.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(lat_s[i]);
		}
	}
	return results;
}
std::vector<double> DataManager::T_station::select_long_s(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_long_s");
	std::vector<double> results;
	for(int i=0;i<long_s.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(long_s[i]);
		}
	}
	return results;
}
std::vector<double> DataManager::T_station::select_lat_f(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_lat_f");
	std::vector<double> results;
	for(int i=0;i<lat_f.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(lat_f[i]);
		}
	}
	return results;
}
std::vector<double> DataManager::T_station::select_long_f(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_long_f");
	std::vector<double> results;
	for(int i=0;i<long_f.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(long_f[i]);
		}
	}
	return results;
}
std::vector<std::string> DataManager::T_station::select_nors_s(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_nors_s");
	std::vector<std::string> results;
	for(int i=0;i<nors_s.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(nors_s[i]);
		}
	}
	return results;
}
std::vector<std::string> DataManager::T_station::select_eorw_s(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_eorw_s");
	std::vector<std::string> results;
	for(int i=0;i<eorw_s.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(eorw_s[i]);
		}
	}
	return results;
}
std::vector<std::string> DataManager::T_station::select_nors_f(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_nors_f");
	std::vector<std::string> results;
	for(int i=0;i<nors_f.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(nors_f[i]);
		}
	}
	return results;
}
std::vector<std::string> DataManager::T_station::select_eorw_f(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_eorw_f");
	std::vector<std::string> results;
	for(int i=0;i<eorw_f.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(eorw_f[i]);
		}
	}
	return results;
}

std::vector<int> DataManager::T_station::select_time_s(std::string _trip_code,std::string _stratum)
{
	DEBUG1("DataManager::T_station::select_time_s");
	std::vector<int> results;
	for(int i=0;i<time_s.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(time_s[i]);
		}
	}
	return results;
}
std::vector<std::string> DataManager::T_station::select_date_s(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_date_s");
	std::vector<std::string> results;
	for(int i=0;i<date_s.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(date_s[i]);
		}
	}
	return results;
}
std::vector<int> DataManager::T_station::select_time_f(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_time_f");
	std::vector<int> results;
	for(int i=0;i<time_f.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(time_f[i]);
		}
	}
	return results;
}
std::vector<std::string> DataManager::T_station::select_date_f(std::string _trip_code,std::string _stratum)	
{
	DEBUG1("DataManager::T_station::select_date_f");
	std::vector<std::string> results;
	for(int i=0;i<date_f.size();i++){
		if(trip_code_col!=-1? (trip_code[i]==_trip_code && stratum[i]==_stratum):(stratum[i]==_stratum)){
				results.push_back(date_f[i]);
		}
	}
	return results;
}


