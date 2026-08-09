// const char* time_stamp = "$Date: 2008-09-08 12:33:48 +1200 (Mon, 08 Sep 2008) $\n";
// const char* t_LW_coeffs_cpp_id = "$Id: t_LW_coeffs.cpp 2237 2008-09-08 00:33:48Z fud $\n";

//############################## INCLUDES ##############################
#include "development.h"
#include "datamanager.h"


DataManager::T_lw_coeff::T_lw_coeff(DataManager *p_DM,std::string input_file):T_table(p_DM,input_file,""){
	DEBUG0("DataManager::T_lw_coeff::T_lw_coeff");
}

void DataManager::T_lw_coeff::input(Parameter_set& p){
	DEBUG0("DataManager::T_stratum::input");
	if(p.get_command_count("lw_coeff")>0){
		input_from_p(p);
	} else {
		
		#ifndef __MINGW32__
			input_from_DB(p);
		#else
			fatal("Can not input data for lw_coeff table from rdb database on Microsoft Windows.");
		#endif
	
		if(!input_file.empty()){
			ifstream in(input_file.c_str());
			if (!in) fatal("Could not open file " + input_file);
			input_from_file(in);
			in.close();
		}  else {
			fatal("Can not input data for t_lw_coeff table.");
		}	
	}
}

void DataManager::T_lw_coeff::output(Parameter_set& p,Output_precision& output_precision){
	DEBUG0("DataManager::T_lw_coeff::output");
	output_to_file(cout,output_precision);
}

void DataManager::T_lw_coeff::input_from_DB(Parameter_set& p){
	DEBUG0("DataManager::T_lw_coeff::input_from_DB");
	input_file="lw_coeff";
	std::string batch_file="./xxx_lw_coeff";
	std::string log_file="./yyy_lw_coeff";

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
		_select="select spp_code,";
		_select+="coalesce(to_char(sex,'9'),'NULL') as sex,";
		_select+="coalesce(to_char(lw_coeff_a,'9999999D999999'),'NULL') as lw_coeff_a,";
		_select+="coalesce(to_char(lw_coeff_b,'9999999D999999'),'NULL') as lw_coeff_b,";
		_select+="coalesce(to_char(lw_coeff_c,'9999999D999999'),'NULL') as lw_coeff_c";
	} else {
		_select="select spp_code,sex,lw_coeff_a,lw_coeff_b,lw_coeff_c ";
	}
	_select+="from lw_coeff ";
	_where=" where ts_default='Y' ";
	_where+=" and spp_code in(";
	for(int i=0;i<p_DM->species.size();i++){
		_where+=(i!=p_DM->species.size()-1)? ("'"+p_DM->species[i]+"',"):("'"+p_DM->species[i]+"'");
	}
	_where+=")";

	sql=_select+' '+_where;

	cout<<"SQL for extracting lw_coeff data:\n";
	cout<<sql<<"\n\n";

	if(!system(0))
		fatal("command interpreter not available!");

	if(database=="Postgresql"){
		ofstream bat_file(batch_file.c_str());
		bat_file<<"\\a\n\\f ' '\n"<<sql<<"\n";
		bat_file.close();
		std::string cmd="psql -d rdb "+(hostname!=""? "-h "+hostname:"")+" "+(user!=""? "-U "+user:"")+" -f \""+batch_file+"\" -o "+input_file+" >"+log_file;
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
		bat_file<<"empbatch rdb <<EOF\nset MSNULLVALUE 'NULL'\n"<<sql<<" into '"<<input_file<<"'\nEOF";
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

void DataManager::T_lw_coeff::input_from_file(ifstream& in){
	DEBUG0("DataManager::T_catch::input_from_file");
	std::vector<std::string> lines=read_file(in);
	if(lines.size()==0)
		fatal("There is no data "+input_file);
	// Read in header
	std::vector<std::string> header=string_to_string_vector(lines[0]);
	// Notice there should be no trip_code from rdb
	trip_code_col=pos(header,"trip_code");	
	spp_code_col=pos(header,"spp_code");
	if(spp_code_col==-1 && p_DM->species.size()>1)
		fatal("Can not find 'spp_code' in "+input_file);
	sex_col=pos(header,"sex");
	if(sex_col==-1)
		fatal("Can not find 'sex' in "+input_file);
	lw_coeff_a_col=pos(header,"lw_coeff_a");
	if(lw_coeff_a_col==-1)
		fatal("Can not find 'lw_coeff_a' in "+input_file);
	lw_coeff_b_col=pos(header,"lw_coeff_b");
	if(lw_coeff_b_col==-1)
		fatal("Can not find 'lw_coeff_b' in "+input_file);
	lw_coeff_c_col=pos(header,"lw_coeff_c");
	if(lw_coeff_c_col==-1)
		fatal("Can not find 'lw_coeff_c' in "+input_file);
	

	
	int ncols=4+(trip_code_col!=-1)+(spp_code_col!=-1);
	//Read in data
	for(int i=1;i<lines.size();i++){
		std::vector<std::string> values=string_to_string_vector(lines[i]);
		if(values.size()!=header.size()){
			warning("Skip line "+itos(i)+" from file "+input_file+":\n"+lines[i]+"\n");
			//warning("There should be "+itos(ncols) +" fields in file "+input_file+" but only "+itos(values.size()) +" are found in line "+itos(i)+":\n"+lines[i]+"\nThe line is ignored!\n");
			continue;
		}
		insert(trip_code_col!=-1? (values[trip_code_col]!="NULL"? values[trip_code_col]:S_NULL):S_NULL,
				spp_code_col!=-1? (values[spp_code_col]!="NULL"? values[spp_code_col]:S_NULL):S_NULL,
				values[sex_col]!="NULL"? stoi(values[sex_col],"'sex' has a value of "+ values[sex_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,
				values[lw_coeff_a_col]!="NULL"? stod(values[lw_coeff_a_col],"'lw_coeff_a' has a value of "+ values[lw_coeff_a_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,
				values[lw_coeff_b_col]!="NULL"? stod(values[lw_coeff_b_col],"'lw_coeff_b' has a value of "+ values[lw_coeff_b_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL,
				values[lw_coeff_c_col]!="NULL"? stod(values[lw_coeff_c_col],"'lw_coeff_c' has a value of "+ values[lw_coeff_c_col]+" in line "+itos(i)+" of file "+input_file+" (none numeric).",""):D_NULL);
	}
}


void DataManager::T_lw_coeff::input_from_p(Parameter_set& p){
	DEBUG0("DataManager::T_lw_coeff::input_from_p");
	std::vector<std::string> labels=p.get_command_labels("lw_coeff");
	for(int i=0;i<labels.size();i++){
		if(labels[i]==""){
			if(!(p_DM->trips.size()==1 && p_DM->species.size()==1))
				fatal("Ambiguous @lw_coeff command.");
		} else if(!(in(p_DM->trip_species_labels,labels[i]) || in(p_DM->species,labels[i]))) {
			fatal("The label "+labels[i]+" of @lw_coeff is ambiguous.");
		}
	}

	for(int i=0;i<p_DM->trips.size();i++){
		for(int j=0;j<p_DM->trip_species[p_DM->trips[i]].size();j++){
			std::string command;
			if(labels.size()==1 && labels[0]==""){
				command = "lw_coeff.";
			} else if(in(labels,p_DM->trip_species[p_DM->trips[i]][j])+in(labels,p_DM->trips[i]+"_"+p_DM->trip_species[p_DM->trips[i]][j])>1){
				fatal("Ambiguous 'lw_coeff' command label for trip "+p_DM->trips[i]+ " and species "+p_DM->trip_species[p_DM->trips[i]][j]);
			} /*else if (in(labels,p_DM->trips[i])){
				if(p_DM->trip_species[p_DM->trips[i]].size()>1)
					fatal("Ambiguous 'lw_coeff' command label for trip "+p_DM->trips[i]+ " and species "+p_DM->trip_species[p_DM->trips[i]][j]);
				command = "lw_coeff["+p_DM->trips[i]+"].";
			} */else if(in(labels,p_DM->trip_species[p_DM->trips[i]][j])){
				//for(int k=0;k<p_DM->trips.size();k++){
				//	if(k!=i && in(p_DM->trip_species[p_DM->trips[k]],p_DM->trip_species[p_DM->trips[i]][j]))
				//		fatal("Ambiguous 'lw_coeff' command label for trip "+p_DM->trips[i]+ " and species "+p_DM->trip_species[p_DM->trips[i]][j]);
				//}
				command = "lw_coeff["+p_DM->trip_species[p_DM->trips[i]][j]+"].";
			} else if(in(labels,p_DM->trips[i]+"_"+p_DM->trip_species[p_DM->trips[i]][j])){
				command="lw_coeff["+p_DM->trips[i]+"_"+p_DM->trip_species[p_DM->trips[i]][j]+"].";
			} else 	{
				continue; //there are entensive codes to check whether the lw_coeff neeeds to be defined for a species
				//fatal("You must define lw_coeff for trip "+p_DM->trips[i]+ " and species "+p_DM->trip_species[p_DM->trips[i]][j]);
			}
			trip_code_col=0;spp_code_col=0;sex_col=0;lw_coeff_a_col=0;lw_coeff_b_col=0;lw_coeff_c_col=0;
			double _lw_coeff_a,_lw_coeff_b, _lw_coeff_c;
			if((p.present(command+"a") || p.present(command+"b")||p.present(command+"c")) && 
				(p.present(command+"a_male") || p.present(command+"b_male") || p.present(command+"c_male") ||
				 p.present(command+"a_female") || p.present(command+"b_female") || p.present(command+"c_female") || 
				 p.present(command+"a_unsexed") || p.present(command+"b_unsexed") || p.present(command+"c_unsexed"))){
				fatal("You must define either a,b,and (optionally ) c, or a_male,b_male,a_female,b_female, and (optionally) c_male,c_female,a_unsexed,b_unsexed, c_unsexed.");
			}
			if(p.present(command+"a") || p.present(command+"b")||p.present(command+"c")){
				_lw_coeff_a=p.get_double(command+"a");
				if(_lw_coeff_a<=0) fatal(command+"a must be positive.");
				_lw_coeff_b=p.get_double(command+"b");
				_lw_coeff_c=p.get_double(command+"c",0);
				insert(p_DM->trips[i],p_DM->trip_species[p_DM->trips[i]][j],D_NULL,_lw_coeff_a,_lw_coeff_b,_lw_coeff_c);
			} else {
				_lw_coeff_a=p.get_double(command+"a_male");
				if(_lw_coeff_a<=0) fatal(command+"a_male must be positive.");
				_lw_coeff_b=p.get_double(command+"b_male");
				_lw_coeff_c=p.get_double(command+"c_male",0);
				insert(p_DM->trips[i],p_DM->trip_species[p_DM->trips[i]][j],1,_lw_coeff_a,_lw_coeff_b,_lw_coeff_c);

				_lw_coeff_a=p.get_double(command+"a_female");
				if(_lw_coeff_a<=0) fatal(command+"a_female must be positive.");
				_lw_coeff_b=p.get_double(command+"b_female");
				_lw_coeff_c=p.get_double(command+"c_female",0);
				insert(p_DM->trips[i],p_DM->trip_species[p_DM->trips[i]][j],2,_lw_coeff_a,_lw_coeff_b,_lw_coeff_c);
				if(p.present(command+"a_unsexed") || p.present(command+"b_unsexed") ||  p.present(command+"c_unsexed")){
					_lw_coeff_a=p.get_double(command+"a_unsexed");
					if(_lw_coeff_a<=0) fatal(command+"a_unsexed must be positive.");
					_lw_coeff_b=p.get_double(command+"b_unsexed");
					_lw_coeff_c=p.get_double(command+"c_unsexed",0);
					insert(p_DM->trips[i],p_DM->trip_species[p_DM->trips[i]][j],3,_lw_coeff_a,_lw_coeff_b,_lw_coeff_c);
				} 
			}
		}
	}
}

void DataManager::T_lw_coeff::output_to_file(ostream& out,Output_precision& output_precision){
	DEBUG0("DataManager::T_lw_coeff::output_to_file");
	out<<"Coefficiets of length weight relationship:\n\n";

	//write header                            
	if(trip_code.size()>0)
		out<<"trip_code"<<" ";
	if(spp_code.size()>0)
		out<<"spp_code"<<" ";
	out<<"sex ";
	out<<"lw_coeff_a"<<" ";
	out<<"lw_coeff_b"<<" ";
	out<<"lw_coeff_c"<<" ";

	out<<"\n";
	for(int i=0;i<sex.size();i++){
		if(trip_code.size()>0)
			out<<trip_code[i]<<" ";
		if(spp_code.size()>0)
			out<<spp_code[i]<<" ";
		if(sex[i]!=D_NULL) out <<sex[i]<<" "; else out<<"NULL"<<" ";
		out<<lw_coeff_a[i]<<" ";
		out<<lw_coeff_b[i]<<" ";
		if(lw_coeff_c[i]!=D_NULL) out <<lw_coeff_c[i]<<" "; else out<<"NULL"<<" ";
		out<<"\n";
	}
}


void DataManager::T_lw_coeff::insert(std::string _trip_code, std::string _spp_code,int _sex,
					double _lw_coeff_a,double _lw_coeff_b,double _lw_coeff_c){
	DEBUG1("DataManager::T_lw_coeff::insert");
	std::string this_key;
	if(trip_code_col==-1 && spp_code_col==-1){
		this_key=_sex;
	} else {
		if(trip_code_col==-1 && spp_code_col!=-1)
			this_key=_spp_code+' '+itos(_sex);
		else if(trip_code_col!=-1 && spp_code_col==-1)
			this_key=_trip_code+' '+itos(_sex);
		else
			this_key=_trip_code+' '+_spp_code+itos(_sex);
	}
	if(in(key,this_key))
	fatal("Duplicate key in lw_coeff table:"+this_key+ " already exists.");

	if(trip_code_col!=-1)
			trip_code.push_back(_trip_code);
	if(spp_code_col!=-1)
			spp_code.push_back(_spp_code);
	sex.push_back(_sex);
	if(_lw_coeff_a==D_NULL) fatal("lw_coeff_a can not be NULL");
	if(_lw_coeff_a!=D_NULL && _lw_coeff_a<=0) fatal("lw_coeff_a must be positive in lw_coeff table");
	lw_coeff_a.push_back(_lw_coeff_a);
	if(_lw_coeff_b==D_NULL) fatal("lw_coeff_b can not be NULL");
	if(_lw_coeff_b!=D_NULL && _lw_coeff_b<0) fatal("lw_coeff_b must be positive or zero in lw_coeff table");
	lw_coeff_b.push_back(_lw_coeff_b);
	lw_coeff_c.push_back(_lw_coeff_c);	
	key.push_back(this_key);
}


std::vector<double> DataManager::T_lw_coeff::select_lw_coeff_a(std::string _trip_code,std::string _spp_code,int _sex)
{
	DEBUG1("DataManager::T_lw_coeff::select_lw_coeff_a");
	std::vector<double> result;
	for(int i=0;i<lw_coeff_a.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (spp_code_col!=-1? (spp_code[i]==_spp_code):true) && (sex[i]==_sex)){
			result.push_back(lw_coeff_a[i]);
		}
	}
	return result;
}

std::vector<double> DataManager::T_lw_coeff::select_lw_coeff_b(std::string _trip_code,std::string _spp_code,int _sex)
{
	DEBUG1("DataManager::T_lw_coeff::select_lw_coeff_b");
	std::vector<double> result;
	for(int i=0;i<lw_coeff_a.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (spp_code_col!=-1? (spp_code[i]==_spp_code):true) && (sex[i]==_sex)){
			result.push_back(lw_coeff_b[i]);
		}
	}
	return result;
}

std::vector<double> DataManager::T_lw_coeff::select_lw_coeff_c(std::string _trip_code,std::string _spp_code,int _sex)
{
	DEBUG1("DataManager::T_lw_coeff::select_lw_coeff_c");
	std::vector<double> result;
	for(int i=0;i<lw_coeff_c.size();i++){
		if((trip_code_col!=-1? (trip_code[i]==_trip_code):true) && (spp_code_col!=-1? (spp_code[i]==_spp_code):true) && (sex[i]==_sex)){
			result.push_back(lw_coeff_c[i]);
		}
	}
	return result;
}

void DataManager::T_lw_coeff::print()
{
	for(int i=0;i<sex.size();i++){
		cerr<<trip_code[i]<<" ";
		cerr<<spp_code[i]<<" ";
		cerr<<sex[i]<<" ";
		cerr<<lw_coeff_a[i]<<" ";
		cerr<<lw_coeff_b[i]<<"\n";
	}

}
