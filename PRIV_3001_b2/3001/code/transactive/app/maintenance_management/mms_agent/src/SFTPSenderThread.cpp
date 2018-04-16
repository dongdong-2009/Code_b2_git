/**
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/mms_agent/src/SFTPSenderThread.h $
* @author:   huirong.luo
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/12/29 14:12:57 $
* Last modified by:  $Author: grace.koh $
* 
* This class get MMS request message from database and send to MMS server regularly.
*/
#pragma warning(disable:4786)

#include <algorithm>
#include <sstream>
 
#include <time.h>
#include "ace/OS.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/alarm/src/AlarmHelper.h" 
#include "core/alarm/src/AlarmHelperManager.h" 
#include "core/sftp/src/SFTPException.h"
#include "core/sftp/src/SFTPManager.h"
#include "core/data_access_interface/src/MmsRequestAccessFactory.h" 
#include "app/maintenance_management/mms_agent/src/MmsConnectionException.h" 
#include "app/maintenance_management/mms_agent/src/SFTPSenderThread.h"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/exception.hpp"

#define  Default_Scan_Seconds  60
#define  Maximum_Entry_Per_File 100
#define  Maximum_Entry_Size 620

#define Entry_Header_Sender_Size 4
#define Entry_Header_Receiver_Size 4
#define Entry_Header_MessageType_Size 3
#define Entry_Header_MessageNumber_Size 3
#define Entry_Header_DateTime_Size 19

#define Entry_EquipmentID_Size 38

#define Alarm_Entry_DateTime_Size Entry_Header_DateTime_Size
#define Alarm_Entry_Severity_Size 1
#define Alarm_Entry_EquipmentID_Size Entry_EquipmentID_Size
#define Alarm_Entry_FaultCode_Size 30
#define Alarm_Entry_Description_Size 200


#define Entry_JobRequestor_Size 20
#define Entry_JobProblemReport_Size 256
#define Entry_JobTargetDate_Size Entry_Header_DateTime_Size
#define Entry_MeterCode_Size 30
#define Entry_EquipmentDescription_Size 100
#define Entry_CountValue_Size 14
#define Entry_MeasureValue_Size 12

#define Entry_Separator  "\r"

#define  Entry_Separator_Size 1
#define  Entry_Header_Size 33
#define  Auto_Alarm_Entry_Size   321+Entry_Separator_Size
#define  Semi_Auot_Alarm_Entry_Size 616+Entry_Separator_Size
#define  Schematics_Entry_Size 366 + Entry_Separator_Size
#define  Meter_Counter_Entry_Size 215 +Entry_Separator_Size
#define  Measure_Entry_Size 213+Entry_Separator_Size


using  TA_Base_Core::DebugUtil;
namespace fs = boost::filesystem;

namespace TA_IRS_App
{ 
	SFTPSenderThread::SFTPSenderThread(TA_Base_Core::MmsAgentEntityDataPtr pData)
		: m_scanPeriodSeconds(Default_Scan_Seconds), 
		   m_stopRequest(true) ,m_isControlMode(false)
	{
		FUNCTION_ENTRY("SFTPSenderThread");

		m_PreviousFileTime.time=0;
		m_SftpHostname =pData->getServerAddress().c_str();
		m_SftpPort=atoi(pData->getSFTPport().c_str());
		m_SftpUserName=pData->getSFTPUserName().c_str();
		m_SftpPassword=pData->getSFTPPassword().c_str();
		m_SftpDestinationPath=pData->getSFTPDestinationFolder().c_str();
		m_SftpSourcePath=pData->getSFTPLocalFolder().c_str();
		long sftpPeroidInSeconds=pData->getSFTPPeroidInSeconds();

		if (sftpPeroidInSeconds>0)
		{
			m_scanPeriodSeconds=sftpPeroidInSeconds;
		}

		if (m_SftpDestinationPath.length() > 0)
		{
			std::string::iterator it = m_SftpDestinationPath.end() - 1;
			if ( !(  (*it == '/') ||  (*it == '\\' )   ) )
			{
				m_SftpDestinationPath=m_SftpDestinationPath + "/";
			}		 
		}
		if (m_SftpSourcePath.length() > 0)
		{
			std::string::iterator it = m_SftpSourcePath.end() - 1;
			if ( !( (*it == '/')  || (*it == '\\') ) )
			{
				m_SftpSourcePath=m_SftpSourcePath + "/";
			}
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MMS Server=%s  port=%lu DesPath=%s surPath=%s PeroidInSeconds=%ld", m_SftpHostname.c_str(),
			    m_SftpPort, m_SftpDestinationPath.c_str(),m_SftpSourcePath.c_str(),m_scanPeriodSeconds);

		FUNCTION_EXIT;	
	}

	SFTPSenderThread::~SFTPSenderThread()
	{
		FUNCTION_ENTRY("~SFTPSenderThread");	 
		FUNCTION_EXIT;
	}
   
	void SFTPSenderThread::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_stopRequest = true;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "terminate( ) called");
		FUNCTION_EXIT;
	}
  
	void SFTPSenderThread::setScanPeriodSecond(unsigned int sec)
	{
		FUNCTION_ENTRY("setScanPeriodMinutes");
		m_scanPeriodSeconds = sec;
		FUNCTION_EXIT;
	}
 
	void SFTPSenderThread::run()
	{
		FUNCTION_ENTRY("SFTPSenderThread::Run");
		m_stopRequest = false;	 

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"SFTPSenderThread::run() called"); 
	 
		while(m_stopRequest == false)
		{
            try
            {			     
			    if(m_isControlMode)
			    {
				    sendMMSMessages();	 
			    }
			    else
			    {
				    sleep(2000);
				    continue;
			    }
            }
            catch (TA_Base_Core::TransactiveException& e)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, e.what());
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown exception");
            }

			//have a nap for the time remaining
			//sleep first, make sure the previous control agent can finish its task.
			time_t lastScan;
			time(&lastScan);
			time_t now;
			time(&now);
			while( ( (now - lastScan) ) < static_cast<long>(m_scanPeriodSeconds) )
			{
				if(m_stopRequest == true) break;						
				sleep(2000);
				time(&now);
			}		
		}
	}

	void SFTPSenderThread::setToControlMode()
	{
		FUNCTION_ENTRY("setToControlMode");
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "setToControlMode");
		m_isControlMode=true; 
		FUNCTION_EXIT;
	}

	void SFTPSenderThread::setToMonitorMode()
	{
		FUNCTION_ENTRY("setToMonitorMode");
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "setToMonitorMode");
		m_isControlMode=false;
		FUNCTION_EXIT;
	}	

	void SFTPSenderThread::setBuffer(char* buffer, std::string& data, int maxlength)
	{
		int length=data.size();
		if (length>= maxlength)
		{
			memcpy(buffer,data.c_str() ,maxlength); 	
		}
		else
		{
			memcpy(buffer,data.c_str() ,length); 
			memset(buffer+ length,' ', maxlength- length);
		}
	}

	void SFTPSenderThread::setBuffer(char* buffer, timeb& data, int maxlength)
	{
		struct tm  tmStruct;	 		 
		if (data.time==0)
		{
			memset(buffer,' ',maxlength);
			return;
		}
		if (ACE_OS::localtime_r(&data.time, &tmStruct) != NULL)
		{
			sprintf(buffer, "%02d/%02d/%04d/%02d:%02d:%02d",tmStruct.tm_mday, tmStruct.tm_mon + 1,
				tmStruct.tm_year + 1900, tmStruct.tm_hour, tmStruct.tm_min, tmStruct.tm_sec);
		}
	}

	void SFTPSenderThread::setBuffer(char* buffer, unsigned long  data, int maxlength)
	{
		std::string formatStr="";
		std::stringstream out;
		out << "%0" << maxlength << "d";
		formatStr = out.str();
		sprintf(buffer,formatStr.c_str(),data);
	}

	void SFTPSenderThread::setBuffer(char* buffer, double data, int maxlength)
	{
		std::string formatStr="";
		std::stringstream out;
		memset(buffer,'0',maxlength);
		out << "%" << maxlength << ".3f";
		formatStr = out.str();
		char tmpBuffer[50] = {0};
		sprintf(tmpBuffer,formatStr.c_str(),data);	
		std::string newstr;
		newstr=tmpBuffer;
		//trim space
		newstr.erase( std::remove( newstr.begin(), newstr.end(), ' '), newstr.end() ); 
		std::string::size_type dotPos = newstr.find_last_of ( ".", newstr.length() );
		if ( dotPos ==(maxlength-1) )  //when the last char is a dot.
		{
			newstr= "0" + newstr.substr ( 0, maxlength-1);			 
		}		 
		setBuffer(buffer, newstr, newstr.length() );
	}

	void SFTPSenderThread::addSeparator(char* buffer)
	{
		memcpy(buffer,Entry_Separator,Entry_Separator_Size);
	}

	bool SFTPSenderThread::getAutoAlarmMessage(char* msgContext,int startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestAlarmData* data)
	{
		bool r= createMessageHeader(msgContext,startPosition,msgIndex,TA_Base_Bus::MMSMessageType::ALARM);
		if (r==false) return false;
		char* pos = msgContext+ (startPosition+ Entry_Header_Size);
		//alarm date time	 
		timeb  alarmDate=data->getAlarmDate();
		setBuffer(pos,alarmDate, Alarm_Entry_DateTime_Size); pos=pos+Alarm_Entry_DateTime_Size;		
		//alarm severity	 
		unsigned long  alarmSeverity=data->getAlarmSeverity();
		setBuffer(pos,alarmSeverity, Alarm_Entry_Severity_Size);  pos=pos+Alarm_Entry_Severity_Size;
		//Equipment ID
		std::string equipID=data->getAlarmEquipmentID();
		setBuffer(pos,equipID, Alarm_Entry_EquipmentID_Size);pos=pos+Alarm_Entry_EquipmentID_Size;		
		//alarm fault code
		std::string  alarmFaultCode=data->getAlarmFaultCode();
		setBuffer(pos,alarmFaultCode, Alarm_Entry_FaultCode_Size);pos=pos+Alarm_Entry_FaultCode_Size;
		//alarm description
		std::string desc=data->getAlarmDescription();
		setBuffer(pos,desc, Alarm_Entry_Description_Size);pos=pos+Alarm_Entry_Description_Size; 

		addSeparator(pos);
		return true;
	}

	bool SFTPSenderThread::getSemiAutoAlarmMessage(char* msgContext,int startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestAlarmData* data)
	{
		bool r= createMessageHeader(msgContext,startPosition,msgIndex,TA_Base_Bus::MMSMessageType::JOB_REQUEST_ALARM);
		if (r==false) return false;
		char* pos = msgContext+ (startPosition+ Entry_Header_Size);
		//Requestor
		std::string requestor=data->getRequestor();
		setBuffer(pos,requestor, Entry_JobRequestor_Size);
        pos += Entry_JobRequestor_Size;	
		//Equipement ID
		std::string equipID=data->getAlarmEquipmentID();
		setBuffer(pos,equipID, Alarm_Entry_EquipmentID_Size);
        pos += Alarm_Entry_EquipmentID_Size;		
		//alarm date time	 
		timeb  alarmDate=data->getAlarmDate();
		setBuffer(pos,alarmDate, Alarm_Entry_DateTime_Size);
        pos += Alarm_Entry_DateTime_Size;		
		//alarm severity	 
		unsigned long  alarmSeverity=data->getAlarmSeverity();
		setBuffer(pos,alarmSeverity, Alarm_Entry_Severity_Size);
        pos += Alarm_Entry_Severity_Size;		
		//alarm fault code
		std::string  alarmFaultCode=data->getAlarmFaultCode();
		setBuffer(pos,alarmFaultCode, Alarm_Entry_FaultCode_Size);
        pos += Alarm_Entry_FaultCode_Size;
		//alarm description
		std::string desc=data->getAlarmDescription();
		setBuffer(pos,desc, Alarm_Entry_Description_Size);pos=pos+Alarm_Entry_Description_Size; 
		//problem report
		std::string   problemReport=data->getProblemReport();
		setBuffer(pos,problemReport, Entry_JobProblemReport_Size);pos=pos+Entry_JobProblemReport_Size; 
		//target date
		timeb  targetDate=data->getTargetDate();
		setBuffer(pos,targetDate, Entry_JobTargetDate_Size);pos=pos+Entry_JobTargetDate_Size; 

		addSeparator(pos);
		return true;

	}
 
	bool SFTPSenderThread::getRequestSchematicMessage(char* msgContext,int startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestSchematicData* data)
	{
		bool r= createMessageHeader(msgContext,startPosition,msgIndex,TA_Base_Bus::MMSMessageType::JOB_REQUEST_SCHEMATIC);
		if (r==false) return false;
		char* pos = msgContext+ (startPosition+ Entry_Header_Size);
		//Requestor
		std::string requestor=data->getRequestor();
		setBuffer(pos,requestor, Entry_JobRequestor_Size);pos=pos+Entry_JobRequestor_Size;	
		//Equipement ID
		std::string equipID=data->getEquipmentID();
		setBuffer(pos,equipID, Entry_EquipmentID_Size);pos=pos+Entry_EquipmentID_Size;				
		//problem report
		std::string   problemReport=data->getProblemReport();
		setBuffer(pos,problemReport, Entry_JobProblemReport_Size);pos=pos+Entry_JobProblemReport_Size; 
		//target date
		timeb  targetDate=data->getTargetDate();
		setBuffer(pos,targetDate, Entry_JobTargetDate_Size);pos=pos+Entry_JobTargetDate_Size; 

		addSeparator(pos);
		return true;
	}

	bool SFTPSenderThread::getCountMessage(char* msgContext,int startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestScheduledData* data)
	{
		bool r= createMessageHeader(msgContext,startPosition,msgIndex,TA_Base_Bus::MMSMessageType::COUNTER,&(data->getCreateDate()));
		if (r==false) return false;
		char* pos = msgContext+ (startPosition+ Entry_Header_Size);	 
		//Equipement ID
		std::string equipID=data->getEquipmentID();
		setBuffer(pos,equipID, Entry_EquipmentID_Size);pos=pos+Entry_EquipmentID_Size;				
		//meter code
		std::string meterCode=data->getMeterCode();
		setBuffer(pos,meterCode, Entry_MeterCode_Size);pos=pos+Entry_MeterCode_Size; 
		//Equipment Description
		std::string equipDesc=data->getEquipmentDescription();
		setBuffer(pos,equipDesc, Entry_EquipmentDescription_Size);pos=pos+Entry_EquipmentDescription_Size; 
		//value
		double  dpValue=data->getDpValue();
		setBuffer(pos,dpValue, Entry_CountValue_Size);pos=pos+Entry_CountValue_Size;

		addSeparator(pos);
		return true;
	}
	
	bool SFTPSenderThread::getMeasureMessage(char* msgContext,int startPosition, unsigned long  msgIndex ,TA_Base_Core::MmsRequestScheduledData* data)
	{
		bool r= createMessageHeader(msgContext,startPosition,msgIndex,TA_Base_Bus::MMSMessageType::MEASUREMENT,&(data->getCreateDate()));
		if (r==false) return false;
		char* pos = msgContext+ (startPosition+ Entry_Header_Size);	 
		//Equipement ID
		std::string equipID=data->getEquipmentID();
		setBuffer(pos,equipID, Entry_EquipmentID_Size);pos=pos+Entry_EquipmentID_Size;				
		//meter code
		std::string meterCode=data->getMeterCode();
		setBuffer(pos,meterCode, Entry_MeterCode_Size);pos=pos+Entry_MeterCode_Size; 
		//Equipment Description
		std::string equipDesc=data->getEquipmentDescription();
		setBuffer(pos,equipDesc, Entry_EquipmentDescription_Size);pos=pos+Entry_EquipmentDescription_Size; 
		//value
		double  dpValue=data->getDpValue();
		setBuffer(pos,dpValue, Entry_MeasureValue_Size);pos=pos+Entry_MeasureValue_Size;

		addSeparator(pos);
		return true;
	}

	bool SFTPSenderThread::createMessageHeader(char* msgContext,int startPosition, unsigned long  msgIndex, TA_Base_Bus::MMSMessageType::MessageType msgType, timeb* valueDate)
	{
		char* pos=msgContext+startPosition;
		std::string value="";
		//sender
		value="ISCS";
		setBuffer(pos, value,Entry_Header_Sender_Size );pos=pos+Entry_Header_Sender_Size;		 
		//Receiver
		value="MMS ";
		setBuffer(pos, value,Entry_Header_Receiver_Size ); pos=pos+Entry_Header_Receiver_Size;
		//Message Type
		if (msgType==TA_Base_Bus::MMSMessageType::ALARM)
		{
			value="ALA";
		}
		else if (msgType==TA_Base_Bus::MMSMessageType::JOB_REQUEST_ALARM)
		{
			value="REQ";		 
		}
		else if (msgType==TA_Base_Bus::MMSMessageType::JOB_REQUEST_SCHEMATIC)
		{
			value="SCH";			
		}
		else if (msgType==TA_Base_Bus::MMSMessageType::MEASUREMENT)
		{
			value="VAL";			
		}
		else if (msgType==TA_Base_Bus::MMSMessageType::COUNTER)
		{
			value="CPT";			
		}
		setBuffer(pos, value,Entry_Header_MessageType_Size);pos=pos+Entry_Header_MessageType_Size;
		//Message Number
		setBuffer(pos,msgIndex,Entry_Header_MessageNumber_Size);pos=pos+Entry_Header_MessageNumber_Size;		
		//Date Time
        if (NULL == valueDate)
        {
            timeb taTime;
            ftime( &taTime );
            setBuffer(pos,taTime,Entry_Header_DateTime_Size);
        }
        else
        {
            setBuffer(pos,*valueDate,Entry_Header_DateTime_Size);
        }
	 
		return true;
	}

	void SFTPSenderThread::deleteDatabaseData(std::vector<TA_Base_Core::MmsRequestAlarmData*>& vecAlarm,
		std::vector<TA_Base_Core::MmsRequestSchematicData*>& vecSchematic,
		std::vector<TA_Base_Core::MmsRequestScheduledData*>& vecScheduled)
	{	
		if (!vecAlarm.empty())
		{
			TA_Base_Core::MmsRequestAccessFactory::getInstance().deleteMmsRequestAlarmData(vecAlarm);
            vecAlarm.clear();
		}
		if (!vecSchematic.empty())
		{
			TA_Base_Core::MmsRequestAccessFactory::getInstance().deleteMmsRequestSchematicData(vecSchematic);
            vecSchematic.clear();
		}
		if (!vecScheduled.empty())
		{
			TA_Base_Core::MmsRequestAccessFactory::getInstance().deleteMmsRequestScheduledData(vecScheduled);
            vecScheduled.clear();
		}
	}

	std::string SFTPSenderThread::getNewMessageFileName()
	{ 
		timeb taTime;
		ftime( &taTime );
		if (taTime.time!= m_PreviousFileTime.time) 
		{
			 m_PreviousFileTime.time=taTime.time;
			 m_FileNumber=0;
		}
		else
		{
			 m_FileNumber++;
		}
		struct tm  tmStruct;	 
		char buffer[200]= {0};
		if (ACE_OS::localtime_r(&taTime.time, &tmStruct) != NULL)
		{
			sprintf(buffer, "ISCS-%04d-%02d-%02d-%02d-%02d-%02d-%ld.csv",tmStruct.tm_year + 1900, tmStruct.tm_mon + 1,tmStruct.tm_mday,
				 tmStruct.tm_hour, tmStruct.tm_min, tmStruct.tm_sec, m_FileNumber);
		}
		std::string fileName=buffer;
		return fileName;

	}

	bool SFTPSenderThread::sendbySFTP(char* msgContext, int length)
	{
	    if (boost::filesystem::is_directory(m_SftpSourcePath)==false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,   "configuration ERROR %s", m_SftpSourcePath.c_str());
			return false;		   
		}
		
		std::string fileName=getNewMessageFileName();
	  
	   std::string srcFullName=m_SftpSourcePath + fileName;
	   std::string desFullName=m_SftpDestinationPath + fileName;	    
 	   fs::path filepath = fs::system_complete(  fs::path( srcFullName, fs::native ));
 	   fs::ofstream file( filepath );
 	   file.write(msgContext,length);
 	   file.close(); 
	   bool ret=false;
	   try
	   {
		   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,   "Start to send SFTP File %s", srcFullName.c_str());		  
  		   TA_Base_Core::SFTPManager::getInstance().sendFile(m_SftpHostname, m_SftpUserName, m_SftpPassword,
  			    srcFullName,desFullName  ,m_SftpPort);
		   ret=true;
		   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,   "send SFTP File finished");
	   }	  
	   catch(TA_Base_Core::SFTPException& ex)
	   {
		   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,   "send SFTP File unknown error, %s  ERROR=%s", srcFullName.c_str() , ex.what());
		 
	   }
	   catch (...)
	   {
		   LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,   "send SFTP File unknown error %s", srcFullName.c_str());
		   
	   }
       fs::remove(srcFullName);	   
	   return ret;
	}

	void SFTPSenderThread::sendMMSMessages()
	{
		//get message from database
		//1. alarm //2. schematic //3. schedule Must clear delete the pointers before return
		std::vector<TA_Base_Core::MmsRequestAlarmData*> vecAlarm=TA_Base_Core::MmsRequestAccessFactory::getInstance().getMmsRequestAlarmData();		
		std::vector<TA_Base_Core::MmsRequestSchematicData*> vecSchematic=TA_Base_Core::MmsRequestAccessFactory::getInstance().getMmsRequestSchematicData();		
		std::vector<TA_Base_Core::MmsRequestScheduledData*> vecSchedule=TA_Base_Core::MmsRequestAccessFactory::getInstance().getMmsRequestScheduledData();		
		unsigned long  totalSize=vecAlarm.size() + vecSchematic.size()+ vecSchedule.size();

		if (totalSize==0) return;

		bool r=false; 
		int batchSize=0;
		int startPos=0;
		std::vector<TA_Base_Core::MmsRequestAlarmData*> vecBatchAlarm;
		std::vector<TA_Base_Core::MmsRequestSchematicData*> vecBatchSchematic;
		std::vector<TA_Base_Core::MmsRequestScheduledData*> vecBatchScheduled;
		char fileBuffer[Maximum_Entry_Per_File* Maximum_Entry_Size];

		//alarm
		std::vector<TA_Base_Core::MmsRequestAlarmData*>::iterator iterAlarm=vecAlarm.begin();
		for (;iterAlarm!=vecAlarm.end();iterAlarm++)
		{
			batchSize++;
			TA_Base_Core::MmsRequestAlarmData* data=*iterAlarm;			
			int msgSize=0;
			if (data->getIsAutoTrigger()==true)
			{
				msgSize=Auto_Alarm_Entry_Size;
				r=getAutoAlarmMessage(fileBuffer, startPos, batchSize, data);
			}
			else
			{
				msgSize=Semi_Auot_Alarm_Entry_Size;
				r=getSemiAutoAlarmMessage(fileBuffer, startPos, batchSize, data);
			}

			if (r==true)
			{
				startPos+=msgSize;
				vecBatchAlarm.push_back(data);
			}

			if (batchSize>=Maximum_Entry_Per_File)
			{
				//SFTP to MMS server				
				if (sendbySFTP( fileBuffer, startPos )==true)
				{
					deleteDatabaseData(vecBatchAlarm,vecBatchSchematic,vecBatchScheduled);
				}
				batchSize=0;
				startPos=0;
			}
        }
		
		//schematic
		std::vector<TA_Base_Core::MmsRequestSchematicData*>::iterator iterSchematic=vecSchematic.begin();
		for (;iterSchematic!=vecSchematic.end();iterSchematic++)
		{
			batchSize++;
			TA_Base_Core::MmsRequestSchematicData* data=*iterSchematic;
			r=getRequestSchematicMessage(fileBuffer,startPos,batchSize,data);
			if (r==true)
			{
				startPos+=Schematics_Entry_Size;
				vecBatchSchematic.push_back(data);
			}

			if (batchSize>=Maximum_Entry_Per_File)
			{
				//SFTP to MMS server				
				if (sendbySFTP( fileBuffer, startPos )==true)
				{
					deleteDatabaseData(vecBatchAlarm,vecBatchSchematic,vecBatchScheduled);
				}
				batchSize=0;
				startPos=0;
			}
		}
        
		//schedule
		std::vector<TA_Base_Core::MmsRequestScheduledData*>::iterator iterSchedule=vecSchedule.begin();
		for (;iterSchedule!=vecSchedule.end();iterSchedule++)
		{
			batchSize++;
			TA_Base_Core::MmsRequestScheduledData* data=*iterSchedule;
			int msgSize=0;
			if (data->getScheduledType()==TA_Base_Core::MmsRequestScheduledData::COUNTER)  
			{
				msgSize=Meter_Counter_Entry_Size; 
				r=getCountMessage(fileBuffer, startPos, batchSize, data);
			}
			else
			{
				msgSize=Measure_Entry_Size;
				r=getMeasureMessage(fileBuffer, startPos, batchSize, data);			 
			}

			if (r==true)
			{
				startPos+=msgSize;
				vecBatchScheduled.push_back(data);
			}

			if (batchSize>=Maximum_Entry_Per_File)
			{
				//SFTP to MMS server				
				if (sendbySFTP( fileBuffer, startPos )==true)
				{
					deleteDatabaseData(vecBatchAlarm,vecBatchSchematic,vecBatchScheduled);
				}
				batchSize=0;
				startPos=0;
			}
		}
		
		totalSize=vecBatchAlarm.size() + vecBatchSchematic.size()+ vecBatchScheduled.size();
		if (totalSize>0)
		{ 			 
			//SFTP to MMS server
			if (sendbySFTP( fileBuffer, startPos )==true)
			{
				deleteDatabaseData(vecBatchAlarm,vecBatchSchematic,vecBatchScheduled);
			}
        }

        // Clear the memory
        for ( iterAlarm=vecAlarm.begin(); iterAlarm!=vecAlarm.end(); ++iterAlarm )
        {
            delete (*iterAlarm);
            *iterAlarm = NULL;
        }
        vecAlarm.clear();

        for ( iterSchematic=vecSchematic.begin(); iterSchematic!=vecSchematic.end(); ++iterSchematic )
        {
            delete (*iterSchematic);
            *iterSchematic = NULL;
        }
        vecSchematic.clear();

        for ( iterSchedule=vecSchedule.begin(); iterSchedule!=vecSchedule.end(); ++iterSchedule )
        {
            delete (*iterSchedule);
            *iterSchedule = NULL;
        }
        vecSchedule.clear();
    }

}

