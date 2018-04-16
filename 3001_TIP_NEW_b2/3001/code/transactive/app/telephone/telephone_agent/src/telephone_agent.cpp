/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_agent/src/telephone_agent.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// telephone_agent.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "acs.h"
#include "telephone_agent.h"
#include "TelephoneAgent.h"
#include "Telephone.h"
#include <string>
#include <iostream>
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

using namespace std;

int main(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		cout << "This is the TSAPI Agent. Implementing the TSAPI client code." << endl;
		cout << "This version will connect to the ALCATEL OmniPCX 4400 using the csta32.dll." << endl;

		TA_App::TelephoneAgent* agent = new TA_App::TelephoneAgent(argc, argv);

		char input[1024];
		std::string inputString;
		std::string usrInputUpperCase;
//		std::string prompt = "\nTSAPI Agent > ";
		while(inputString.compare("exit"))
		{
			if (!inputString.compare("make"))
			{
				//make a call
				std::cout << "Provide origin nr: ";
				std::cin.getline(input,1024);
				std::string origin = input;
				std::cout << "Provide destination nr: ";
				std::cin.getline(input,1024);
				std::string destination = input;
				//make the call
				TA_App::IDirectoryCorbaDef::DirectoryEntryData destinationData;
				destinationData.number = CORBA::string_dup(destination.c_str());
				TA_App::ITelephoneCorbaDef_ptr phone = agent->getTelephoneByNumber("",origin.c_str());
				phone->makeCall("",destinationData);
			}
			else if(!inputString.compare("find"))
			{
				std::cout << "Give name to search : ";
				std::cin.getline(input,1024);
				std::string searchName = input;
				TA_App::ITelephoneAgentCorbaDef::ITelephoneCorbaDefSequence* sequence = agent->getTelephonesByName("",searchName.c_str());
				std::cout << sequence->length() << " phones found\n";
			}
//			std::cout << prompt;
			std::cin.getline(input, 1024); //pull the line from std::cin
			inputString = input;
		}
		delete agent;
	}

	return nRetCode;
}


