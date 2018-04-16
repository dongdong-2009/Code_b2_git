#include <stdio.h>
#include "excelHelper.h"
#include "executeSql.h"
#include "DataBase.h"
#include "templateFactory.h"

typedef std::map<std::string, std::string> parasMap;

int getParas(std::string filePath, parasMap& paras);
int copy_template(std::string tempName, std::string newFileName);
string ExePath();
string getCurrentDir();

int main()
{
	printf("starting...\n");	
	parasMap paras;
	if (0 < getParas("loadDBConfig.txt", paras))
	{
		return 1;
	}
	std::string newExcelName = getCurrentDir() + "\\" + paras["OutputFileName"];
	std::cout << newExcelName << std::endl;	
	if (0 < copy_template(paras["ExcelTemplateName"], newExcelName))	
	{
		printf("copy template file to file:%s failed!!!\n", newExcelName.c_str());
		return 1;
	}
	int flag;
	DataBase db(paras["Username"].c_str(), paras["Password"].c_str(), paras["Servicename"].c_str());
	flag = db._DB_Connect();
	if( DB_SUCCESS != flag)
	{
		cout<<"[ "<<__FILE__<<" ] [ "<<__LINE__<<" ] db._DB_Connect() = "<<flag<<endl;
		return -1;
	}
	ExecuteSql* ePtr = new ExecuteSql(&db);
	TemplateFactory* fPtr = new TemplateFactory(ePtr);
	ExcelHelper eh;	
    fPtr->execute(paras, newExcelName, eh);
	printf("output file is: %s\n", newExcelName.c_str());
	printf("ok\n");
	return 0;
}

int getParas(std::string filePath, parasMap& paras)
{
	string data;
	ifstream infile;
	infile.open (filePath.c_str());
	if (!infile.is_open())
	{
		printf("file: %s not exist. created a new one, please update it.\n", filePath.c_str());
		ofstream myfile;
		myfile.open (filePath.c_str());
		myfile << "ExcelTemplateName=\n";
		myfile << "Username=\n";
		myfile << "Password=\n";
		myfile << "Servicename=\n";
		myfile << "OutputFileName=\n";
		myfile << "HideLoadSheets=1\n";
		myfile << "entitys_sql=select en.pkey, en.name, en.address, en.description, en.subsystemname, en.locationname, en.typename, en.parentname, en.agentname, en.deleted, enp.value AssetName from entity_v en, entityparametervalue_v enp where en.typename = 'DataPoint' and enp.entitykey = en.parentkey and enp.parametername = 'AssetName' order by en.locationname, AssetName, en.description\n";
		myfile << "datapoints_sql=select en.pkey, en.name, en.address, en.description, enp.value AssetName, enp2.value DataTypeShortName, enp3.value DataType from entity_v en, entityparametervalue_v enp, entityparametervalue_v enp2, entityparametervalue_v enp3 where en.typename = 'DataPoint' and enp.entitykey = en.parentkey and enp.parametername = 'AssetName' and enp2.entitykey = en.pkey and enp2.parametername = 'DataTypeShortName' and enp3.entitykey = en.pkey and enp3.parametername = 'DataType' order by en.parentkey, en.description\n";
		myfile << "derivedState_sql=select derived_dp_pkey, state_value, state_description, state_return_condition from sc_derived_state where derived_dp_pkey in (select pkey from entity_v where typename = 'DataPoint') order by derived_dp_pkey, state_value\n";
		myfile << "trueFalseLabel_sql=select entitykey, parametername, value from entityparametervalue_v where entitykey in (select pkey from entity_v where typename = 'DataPoint') and parametername in ('TrueLabel', 'FalseLabel') order by entitykey, parametername\n";
		myfile << "input_association_sql=select input_a.main_entity_pkey, input_a.input_calc_order, en.address from sc_input_association input_a, entity_v en where input_a.input_entity_pkey = en.pkey order by input_a.main_entity_pkey, input_a.input_calc_order\n";
		myfile << "locations_sql=select pkey, name, description from location order by name, description\n";
		myfile << "datanodes_sql=select en.pkey, en.name, en.description, en.locationname, en.physical_subsystem_name, enp.value AssetName from entity_v en, entityparametervalue_v enp where en.pkey in (select parentkey from entity_v where typename='DataPoint') and enp.entitykey = en.pkey and enp.parametername='AssetName' order by en.locationname, en.physical_subsystem_name, AssetName\n";
		myfile << "locationSubSys_sql=select distinct locationname, physical_subsystem_name from entity_v where pkey in (select parentkey from entity_v where typename = 'DataPoint') order by locationname, physical_subsystem_name\n";
		myfile.close();
		return 1;
	}

    while(infile.good())
    {
	    getline(infile, data);		
		size_t pos = data.find("=");
		if (std::string::npos != pos)
		{
			paras[data.substr(0, pos)] = data.substr(pos + 1);
		}
    }
	infile.close();
	return 0;
}

int copy_template(std::string tempName, std::string newFileName)
{
	ifstream f1(tempName.c_str(), fstream::binary);
	std::cout << "tempname " << tempName << std::endl;
	if (!f1.is_open())
	{
		printf("file:%s con't open\n", tempName.c_str());
		return 2;
	}
	std::cout << "open file successful" << std::endl;

	ifstream t;
	t.open (newFileName.c_str());
	if (t.is_open())
	{
		printf("file:%s exist!!!\n", newFileName.c_str());
		t.close();
		return 1;	// exist this file.
	}
	ofstream f2(newFileName.c_str(), fstream::trunc | fstream::binary);
	f2 << f1.rdbuf();
	return 0;
}

string ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}

string getCurrentDir()
{
	char buffer[MAX_PATH] = {0};
    GetCurrentDirectory( MAX_PATH, buffer);
	std::cout << "current dir is " << buffer << std::endl;
	return string(buffer);
}