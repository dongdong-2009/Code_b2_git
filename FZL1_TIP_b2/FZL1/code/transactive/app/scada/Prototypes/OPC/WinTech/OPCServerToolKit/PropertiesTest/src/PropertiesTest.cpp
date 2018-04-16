// PropertiesTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "cots/WinTech/OPCServerToolKit/opc_ae.h"
#include "cots/WinTech/OPCServerToolKit/opcda.h"
#include "cots/WinTech/OPCServerToolKit/WTOPCsvrAPI.h"
#include <string>
#include <iostream>
#include <comdef.h>
#include <conio.h>

const GUID CLSID_Svr = { 0x6764a030, 0x70c, 0x11d3, { 0x80, 0xd6, 0x0, 0x60, 0x97, 0x58, 0x58, 0xbe } };
const std::string SVR_NAME = "TestOPCDataSource";
const std::string SVR_DESC = "Test OPC DataSource";
const std::string SVR_EXE_PATH = "";
const UINT SVR_RATE = 50; // in msecs


static bool initialiseServer();
static void createTag(const char* tagName);


int main(int argc, char* argv[])
{
	std::cout << "Starting test OPC server." << std::endl;

	// Initialise server
	WORD revision = WTOPCsvrRevision();
	std::cout << "OPC server version " << revision/256 << ".0.0." << revision%256 << std::endl;
	if (!initialiseServer()) return 1;

	// Set some example tags
	createTag("TestSystem.TestSubsytem.TestEquipment.Tag1");


	// Wait for keypress to exit
	std::cout << "OPC test server started, press any key to shutdown." << std::endl;
	char ch = getch();
	std::cout << "OPC test server stopping." << std::endl;

	// Cleanup
	if (!UninitWTOPCsvr())
	{
		std::cout << "Unable to un-initialise server." << std::endl;
	}

	if (!UnregisterServer (( BYTE * ) & CLSID_Svr, SVR_NAME.c_str()))
	{
		std::cout << "Unable to de-register server." << std::endl;
	}
	std::cout << "OPC test server stopped." << std::endl;

	return 0;
}


bool initialiseServer()
{
	if (!UpdateRegistry ( ( BYTE * ) & CLSID_Svr,SVR_NAME.c_str(),SVR_DESC.c_str(),SVR_EXE_PATH.c_str()))
	{
		std::cout << "Cannot update registry" << std::endl;
		return false;
	}

	if (!InitWTOPCsvr(( BYTE * ) &CLSID_Svr,SVR_RATE))
	{
		std::cout << "Unable to initialise server"<< std::endl;
		return false;
	}

	SetVendorInfo ( "Test OPC Server" );

	return true;

}

void createTag(const char* tagName)
{
	const DWORD ITEM_DESCRIPTION_PROPERTY = 101;
	const DWORD ITEM_STATUS_PROPERTY      = 300;
	const DWORD ITEM_HILIMIT_PROPERTY     = 308;
	const DWORD ITEM_LOLIMIT_PROPERTY     = 309;
	const DWORD ITEM_SHORT_LABEL_PROPERTY = 5001;

	const BOOL isWritable = FALSE;
	const WORD quality = 192;  // QUALITY_GOOD_NO_SPECIFIC_REASON;
	VARIANT tagValue = (_variant_t)(long)100;

	VARIANT propertyDescription = (_variant_t)(char*)"My test data point";
	VARIANT propertyShortLabel  = (_variant_t)(char*)"MyPoint";
	VARIANT propertyStatus      = (_variant_t)(char*)"NORMAL";
	VARIANT propertyHiLimit     = (_variant_t)(long) 100;
	VARIANT propertyLoLimit     = (_variant_t)(long) 90;

    HANDLE tag1 = CreateTag (tagName, tagValue, quality, isWritable);

	if (!SetTagProperties (tag1, ITEM_DESCRIPTION_PROPERTY, "Description", propertyDescription))
	{
		std::cout << "Unable to set description property for tag." << std::endl;
	}

	if (!SetTagProperties (tag1, ITEM_SHORT_LABEL_PROPERTY, "ShortLabel", propertyShortLabel))
	{
		std::cout << "Unable to set short label property for tag." << std::endl;
	}

	if (!SetTagProperties (tag1, ITEM_STATUS_PROPERTY, "Status", propertyStatus))
	{
		std::cout << "Unable to set status property for tag." << std::endl;
	}

	if (!SetTagProperties (tag1, ITEM_HILIMIT_PROPERTY, "HiLimit", propertyHiLimit))
	{
		std::cout << "Unable to set hi limit property for tag." << std::endl;
	}

	if (!SetTagProperties (tag1, ITEM_LOLIMIT_PROPERTY, "LoLimit", propertyLoLimit))
	{
		std::cout << "Unable to set lo limit property for tag." << std::endl;
	}
}