/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/RadioSimulatorCommandProcessor.cpp $
  * @author:  LIU YU
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  * 
  */

#include <iostream>
#include <fstream>
#include <string>
#include "app/radio/radio_simulator/src/RadioSimulatorCommandProcessor.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/PreRecordedPaTimeOutThread.h"
//#include "krtc_telegram/TrainUtilities.h"

namespace TA_IRS_App
{
	//TODO replace with your own user commands
    std::string RadioSimulatorCommandProcessor::commands[NUM_COMMANDS] =
    { 
		"passnumber_check",
		"add_issi",
		"list_issi_passnumber",
		"list_delivery_status_options",
		"delivery_status",
		"set_cmdata",
		"send_cmdata",
        "status",
		"loadsim",
		"showsim",
		"clearsim",
		"runsim",
		"w",
		"y",
		"showMessage",
		"tm",
		"version",
		"set_ttsi_ver",
    };

	//TODO replace with your own help messages
    std::string RadioSimulatorCommandProcessor::commandUsage[NUM_COMMANDS] =
    {
        "passnumber_check <'enable'/'disable'>",
		"add_issi <ISSI in hex (24-bit)> passnumber <pass number in hex (32-bit)>",
		"list_issi_passnumber",
		"list_delivery_status_options",
		"delivery_status <status>",	
		"set_cmdata <user data>",
		"send_cmdata",
        "status",
		"loadsim <path to configSimulator.ini>",
		"showsim ",
		"clearsim ",
		"runsim <sim id>",
		"w SERVICE_SERIAL(4) CARGROUP(8) CURRENT_LINE DATA_BYTES(4) RESERVED_BYTE",
		"y SERVICE_SERIAL(4) CARGROUP(8) CURRENT_LINE DATA_BYTES(4) RESERVED_BYTE",
		"showMessage <0=show, 1=hidden>",
		"tm <Dva pa timeout(ms), 0=no timeout>"
		"version",
		"set_ttsi_ver <version number no coma, dot, etc>"
    };


    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    RadioSimulatorCommandProcessor::RadioSimulatorCommandProcessor(RadioSimulator* parentSimulator)
    :        m_parentSimulator(parentSimulator),
            m_errMsg("")
    {

    }

    RadioSimulatorCommandProcessor::~RadioSimulatorCommandProcessor()
    {

    }

        //////////////////////////////////////////////////////////////////////////////
        //                                                                            //
        //                    ISimulationCommandProcessor methods                        //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////

    //
    //hasCommandName
    //
    bool RadioSimulatorCommandProcessor::hasCommandName( const std::string &name )
    {
        if (name == "help")
        {
            return true;
        }

        return hasCommand(name);
    }


    //
    //usageString
    //
    std::string RadioSimulatorCommandProcessor::usageString(const std::string &name)
    {
        return cmdUsage(name);
    }


    //
    //errorMsg
    //
    std::string RadioSimulatorCommandProcessor::errorMsg()
    {
        return m_errMsg;
    }


    //
    //execute
	// TODO create execute actions for your commands
    //
    bool RadioSimulatorCommandProcessor::execute(const std::string &name, const std::string &params)
    {
        m_errMsg = "";

        if(name == "help" )
        {
            for (unsigned int i = 0; i < NUM_COMMANDS; i++)
            {
                std::cout << commandUsage[i] << std::endl;
            }
            return true;
        }

		else if (name == "passnumber_check")
		{
			//std::cout << "set pass number check" << std::endl;
			if(params == "enable")
				return m_parentSimulator->execPassnumber_check(true);
			else if(params == "disable")
				return m_parentSimulator->execPassnumber_check(false);
			else
				return false;
		}

		else if (name == "add_issi")
		{
			return m_parentSimulator->execAddISSI(params);
		}

		else if (name == "list_issi_passnumber")
		{
			return m_parentSimulator->execListISSIPassNum();
		}

		else if (name == "list_delivery_status_options")
		{
			return m_parentSimulator->execListDeliveryStatusOptions();
		}

		else if (name == "delivery_status")
		{
			return m_parentSimulator->execDeliveryStatus(params);
		}

		else if (name == "set_cmdata")
		{
			return m_parentSimulator->execSetCmdata(params);
		}

		else if (name == "send_cmdata")
		{
			return m_parentSimulator->execSendCmdata();
		}

        else if(name == "status")
        {
            return m_parentSimulator->execStatus();
        }
		else if(name == "loadsim")
		{
			return m_parentSimulator->execLoadTable(params);
		}
		else if(name == "clearsim")
		{
			return m_parentSimulator->execClearTable();
		}
		else if(name == "showsim")
		{
			if (name == params) {
				return m_parentSimulator->execShowTable("");
			} else {
				return m_parentSimulator->execShowTable(params);
			}
		}
		else if (name=="runsim")
		{
			return m_parentSimulator->execRun(params);
		}else if(name=="w")
		{
		
			//std::ifstream w_telegram("telegram_w.ini");
			//std::string c1;
			//std::string c2;
			//unsigned char cmdata[22];
			//unsigned char data[22];
			//if(w_telegram.is_open())
			//{
			//	std::getline(w_telegram, c1);
			//	std::getline(w_telegram, c2);
			//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Read from config file, CMDATA: {%s}  DATA: {%s}",c1.c_str(),c2.c_str());
			//}
			//w_telegram.close();

			//int f=0;
			//for (int a = 0; a < 44; a++) { 
			//	cmdata[f] = (CMHelper::Hex2Int(c1[a],1)+CMHelper::Hex2Int(c1[++a],0)); 
			//	f++;
			//}
			//f=0;
			//for (int b = 0; b < 44; b++) { 
			//	data[f] = (CMHelper::Hex2Int(c2[b],1)+CMHelper::Hex2Int(c2[++b],0)); 
			//	f++;
			//}

			////unsigned char cmdata[]={0x00,0x2A,0x00,0x01,0x63,0xdf,0x0f,0x42,0x41,0xc0,0x60,0x2e,0x00,0x00,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
			//
			////unsigned char data[]={
			////0x59, 0x31, 0x33, 0x30, 0x34, 0x31, 0x31, 0x32, 0x39, 0x30, 0x32, 0x30, 0x31, 0x30, 0x31, 
			////0x00, 0x00, 0x11, 0x22, // mileage
			////0x00, 0x01, 0x02, 0x00, 0x03, 0x04, 0x50, // actual data
			////0x00, 0x00, 0x9b, 0x3a };
			//std::vector<unsigned char> ch,dat;
			//
			//// calculate checksum
			//for(int i=0;i<22;i++)
			//	ch.push_back(data[i]);
			//unsigned char checksum = TA_IRS_Bus::TrainUtilities::calculateChecksum(ch, 20);
			//data[20] = checksum;

			//
			//for(int j=0;j<22;j++)
			//	dat.push_back(cmdata[j]);
			//for(int k=0;k<22;k++)
			//	dat.push_back(data[k]);
			//m_parentSimulator->execSendData(dat);

			return 1;



			/*
	const unsigned char		OccTelegramReader::EVENT_TYPE				= 'W';
	const unsigned short    OccTelegramReader::SERVICE_SERIAL_LOCATION  = 1;
	const unsigned short    OccTelegramReader::SERVICE_SERIAL_LENGTH    = 4;
	const unsigned short	OccTelegramReader::CARGROUP_LOCATION		= 5;
	const unsigned short	OccTelegramReader::CURRENT_LINE_LOCATION	= 14;
	const unsigned short	OccTelegramReader::CURRENT_LINE_LENGTH		= 1;
	const unsigned short	OccTelegramReader::DATA_BYTE_1_LOCATION		= 15;
	const unsigned short	OccTelegramReader::DATA_BYTE_2_LOCATION		= 16;
	const unsigned short	OccTelegramReader::DATA_BYTE_3_LOCATION		= 17;
	const unsigned short	OccTelegramReader::DATA_BYTE_4_LOCATION		= 18;
	const unsigned short	OccTelegramReader::RESERVE_BYTE_LOCATION	= 19;
	const unsigned short	OccTelegramReader::CHECKSUM_LOCATION		= 20;
	const unsigned short	OccTelegramReader::STUFF_BYTE_LOCATION		= 21;
	//Cargroup length is configured in the database so make it non-constant
	unsigned short	OccTelegramReader::CARGROUP_LENGTH			= 8;
	unsigned short	OccTelegramReader::CARGROUP_LENGTH			= 4;
  *//*
			for(int i=0;i<22;i++)
				ch.push_back(v[i]);
			unsigned char chk=TA_IRS_Bus::TrainUtilities::calculateChecksum(ch,20);
					ch[20]=chk;
					for(i=0;i<22;i++)
						dat.push_back(cmdata[i]);
					for(vector<unsigned char>::iterator it=ch.begin();it!=ch.end();it++)
					{
						dat.push_back(*it);
					}

m_parentSimulator->execSendData(dat);
			return 1;*/
		}else if(name=="y")
		{
		/*	int num=atoi(params.c_str());
			std::cout<<"Setting Status of 9 telegram to "<<num<<std::endl;
			RadioCommsObserver::setStatus(num);
			return 1;*/
			


			//std::ifstream y_telegram("telegram_y.ini");
			//std::string c1;
			//std::string c2;
			//unsigned char cmdata[22];
			//unsigned char data[30];
			//if(y_telegram.is_open())
			//{
			//	std::getline(y_telegram, c1);
			//	std::getline(y_telegram, c2);
			//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Read from config file, CMDATA: {%s}  DATA: {%s}",c1.c_str(),c2.c_str());
			//}
			//y_telegram.close();
			//int f = 0;
			//for (int a = 0; a < 44; a++) { 
			//	cmdata[f] = (CMHelper::Hex2Int(c1[a],1)+CMHelper::Hex2Int(c1[++a],0)); 
			//	f++;
			//}
			//f=0;
			//for (int b = 0; b < 60; b++) { 
			//	data[f] = (CMHelper::Hex2Int(c2[b],1)+CMHelper::Hex2Int(c2[++b],0)); 
			//	f++;
			//}
			//
			//std::vector<unsigned char> ch,dat;
			//// calculate checksum
			//for(int i=0;i<30;i++)
			//	ch.push_back(data[i]);
			//unsigned char checksum = TA_IRS_Bus::TrainUtilities::calculateChecksum(ch, 28);
			//data[28] = checksum;

			//
			//for(int j=0;j<22;j++)
			//	dat.push_back(cmdata[j]);
			//for(int k=0;k<30;k++)
			//	dat.push_back(data[k]);
			//m_parentSimulator->execSendData(dat);
			
			return 1;

		/*
			const unsigned char  MisTelegramReader::EVENT_TYPE					= 'Y';
	const unsigned short MisTelegramReader::SERVICE_SERIAL_LOCATION     = 1;
	const unsigned short MisTelegramReader::SERVICE_SERIAL_LENGTH       = 4;
	const unsigned short MisTelegramReader::CARGROUP_LOCATION			= 5;
	const unsigned short MisTelegramReader::CARGROUP_LENGTH				= 8;
	const unsigned short MisTelegramReader::CURRENT_LINE_LOCATION		= 13;
	const unsigned short MisTelegramReader::CURRENT_LINE_LENGTH			= 2;
	const unsigned short MisTelegramReader::OPERATING_MILEAGE_LOCATION	= 15;
	const unsigned short MisTelegramReader::DATA_BYTE_1_LOCATION		= 19;
	const unsigned short MisTelegramReader::DATA_BYTE_2_LOCATION		= 20;
	const unsigned short MisTelegramReader::DATA_BYTE_3_LOCATION		= 21;
	const unsigned short MisTelegramReader::DATA_BYTE_4_LOCATION		= 22;
	const unsigned short MisTelegramReader::DATA_BYTE_5_LOCATION		= 23;
	const unsigned short MisTelegramReader::DATA_BYTE_6_LOCATION		= 24;
	const unsigned short MisTelegramReader::DATA_BYTE_7_LOCATION		= 25;
	const unsigned short MisTelegramReader::RESERVE_BYTE_1_LOCATION		= 26;
	const unsigned short MisTelegramReader::RESERVE_BYTE_2_LOCATION		= 27;
	const unsigned short MisTelegramReader::CHECKSUM_LOCATION			= 28;
	const unsigned short MisTelegramReader::STUFF_BYTE_LOCATION			= 29;
	*/
		}
		else if (name=="showMessage") {
			return m_parentSimulator->execShowMessage(params);
		}
        else if (name == "tm") {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "tm = %d ms\n", atoi(params.c_str()));
            PreRecordedPaTimeoutThread::setTimeOutValue( atoi(params.c_str()) );
        }
		else if (name == "version") {
			std::cout << "Radio Simulator version 01.00" << std::endl;
			std::cout << "Built on 12 August 2011" << std::endl;
			return true;
		} else if (name=="set_ttsi_ver") {
			return m_parentSimulator->execSetTtsiVer(params);
		}

        return false;
    }


        //////////////////////////////////////////////////////////////////////////////
        //                                                                            //
        //                PASimulatorCommandProcessor specific methods                //
        //                                                                            //
        //////////////////////////////////////////////////////////////////////////////


    //
    //hasCommand
    //
    bool RadioSimulatorCommandProcessor::hasCommand( const std::string &name )
    {
        for (unsigned int i = 0; i < NUM_COMMANDS; i++)
        {
            if (commands[i] == name)
            {
                return true;
            }
        }
        return false;
    }


    //
    //cmdUsage
    //
    std::string RadioSimulatorCommandProcessor::cmdUsage(const std::string& name)
    {
        for (unsigned int i = 0; i < NUM_COMMANDS; i++)
        {
            if (commands[i] == name)
            {
                return commandUsage[i];
            }
        }
        return "";
    }

}//close namespace
