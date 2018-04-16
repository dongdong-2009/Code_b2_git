/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/CtaIncidentReportData.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Local storage for Incident Reports. Reads Incident Reports from the database.
  * Writes Incident Reports to the database.
  * 
  * There are a large number of methods in this class. This is because the
  * relationships between the data elements have not been specified in the
  * requirements. Thus assumptions can not be made. The current arrangement of
  * methods also follows the likely useage patterns for this class. That is: update
  * one or more elements then write the result to the database.
  * 
  * The class does intelligent loading of the data. Its inital load (on creation)
  * will be of all the required summary data that is displayed by the Incident
  * Manager in list form. When data that is not part of the summary is accessed by
  * the Incident Manager this class will load all remaining data in one operation.
  */



#include "core/data_access_interface/incident_dai/src/CtaIncidentReportData.h"
#include "core/data_access_interface/incident_dai/src/CtaStationReportData.h"
#include "core/data_access_interface/incident_dai/src/IncidentReportHelper.h"

#include "core/message/types/IncidentManagerBroadcastComms_MessageTypes.h"
#include "core/message/types/IncidentAudit_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
//TD16362 Mintao++
#include "core/exceptions/src/IncidentException.h"

//Bugzilla Bug 152: Mintao
//removed unnecessary including header files

#include <sstream>

using namespace TA_Base_Core;
using namespace std;

namespace TA_IRS_Core
{
	
//TD15251  - not used
// using namespace TA_Base_Bus; 
//Mintao ++
//Bug 652 (TD12313)
	static void SendBroadcastMessage(const MessageType& msg, const MessageType& auditmsg,std::string reportId, std::string reportTitle)
	{
		CommsMessageSender *broadcastCommsSender = 
			MessagePublicationManager::getInstance().getCommsMessageSender(
			IncidentManagerBroadcastComms::Context);
		
		// Send an audit message stating that the PA Server was not responding
		TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
			getAuditMessageSender(TA_Base_Core::IncidentAudit::Context);
		
		TA_Base_Core::DescriptionParameters params;
		
		IEntityData* entityData = EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME));
		unsigned long appkey = entityData->getKey();
		delete entityData;
		entityData = NULL;
		
		// reverting back changes for TD15041 (removing IncidentReportID)
		// since specifications have changed (TD15204)
		TA_Base_Core::NameValuePair param1("IncidentReportID", reportId);
		TA_Base_Core::NameValuePair param2("IncidentReportTitle", reportTitle);
		params.push_back(&param1);
		params.push_back(&param2);
		//++lizettejl
		
		
		sender->sendAuditMessage(auditmsg,
			appkey,
			params,
			"N/A",
			TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ),//"",
			"",
			"",
			"" );
		delete sender;
		sender = NULL;
		
		
		CORBA::Any data;
		data <<= CORBA::Any::from_string(reportId.c_str(), 0);
		broadcastCommsSender->sendBroadcastCommsMessage(msg, data);
		delete broadcastCommsSender;
		//Bugzilla Bug 154: Mintao
		broadcastCommsSender = NULL;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	//TD15089 - overloaded the function to pass the operator name when an incident is created
	static void SendBroadcastMessage(const MessageType& msg, const MessageType& auditmsg,std::string reportId, std::string reportTitle, std::string operatorName)
	{
		CommsMessageSender *broadcastCommsSender = 
			MessagePublicationManager::getInstance().getCommsMessageSender(
			IncidentManagerBroadcastComms::Context);
		
		// Send an audit message stating that the PA Server was not responding
		TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
			getAuditMessageSender(TA_Base_Core::IncidentAudit::Context);
		
		TA_Base_Core::DescriptionParameters params;
		
		IEntityData* entityData = EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME));
		unsigned long appkey = entityData->getKey();
		delete entityData;
		entityData = NULL;
		
		TA_Base_Core::NameValuePair param1("IncidentReportID", reportId);
		TA_Base_Core::NameValuePair param2("IncidentReportTitle", reportTitle);
		TA_Base_Core::NameValuePair param3("IncidentOperatorName", operatorName);
		params.push_back(&param1);
		params.push_back(&param2);
		params.push_back(&param3);
		
		sender->sendAuditMessage(auditmsg,
			appkey,
			params,
			"N/A",
			TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ),//"",
			"",
			"",
			"" );
		delete sender;
		sender = NULL;
		
		
		CORBA::Any data;
		data <<= CORBA::Any::from_string(reportId.c_str(), 0);
		broadcastCommsSender->sendBroadcastCommsMessage(msg, data);
		delete broadcastCommsSender;
		//Bugzilla Bug 154: Mintao
		broadcastCommsSender = NULL;
	}
	
	
	CtaIncidentReportData::CtaIncidentReportData()
        :m_stationReportData(NULL), m_incidentReportHelper(NULL),
		m_isModified(true),
		m_canClose(true)
	{
		FUNCTION_ENTRY("Default Constructor");
		
		m_incidentReportHelper = new IncidentReportHelper(); 
		
		FUNCTION_EXIT;
	}
	
	//Mintao ++
	//Bug 652 (TD12313)
	CtaIncidentReportData::CtaIncidentReportData(std::string id, time_t createDate,
		const std::string& title, 
		const std::string& location1, const std::string& location2,
		bool innerTrack, bool outerTrack, const std::string& train, 
		const std::string& alarmDesc, const std::string& asset, 
		const std::string& subsystem, const std::string& operator1, const std::string& profile, 
		time_t closeDate)
        :m_stationReportData(NULL), m_incidentReportHelper(NULL),
		m_isModified(false),
		m_canClose(true)
	{
		FUNCTION_ENTRY("Constructor");
		
		m_incidentReportHelper = new IncidentReportHelper(id, createDate, title,
			location1, location2, innerTrack, outerTrack, train,
			alarmDesc, asset, subsystem, operator1, profile, closeDate); 
		
		FUNCTION_EXIT;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	CtaIncidentReportData::CtaIncidentReportData(const CtaIncidentReportData& source)
        :m_stationReportData(NULL), m_incidentReportHelper(NULL),
		//m_isModified(source.isModified),
		m_isModified(true),
		m_canClose(true)
	{
		FUNCTION_ENTRY("Constructor(source)");
		
		m_incidentReportHelper = new IncidentReportHelper(*source.m_incidentReportHelper);
		
		FUNCTION_EXIT;
	}
	
	CtaIncidentReportData::CtaIncidentReportData(const std::string & alarmId, const std::string & alarmValue)
        :m_stationReportData(NULL), m_incidentReportHelper(NULL),
		m_isModified(true),
		m_canClose(true)
	{
		FUNCTION_ENTRY("Constructor(alarmId)");
		
		m_incidentReportHelper = new IncidentReportHelper(alarmId, alarmValue);
		
		FUNCTION_EXIT;
	}
	
	//Mintao ++
	//Bug 652 (TD12313)
	
	
	CtaIncidentReportData::CtaIncidentReportData(std::string id, bool isIDKey)
        :m_stationReportData(NULL), m_incidentReportHelper(NULL),
		m_isModified(false),
		m_canClose(true)
	{
		FUNCTION_ENTRY("Constructor(id)");
		
		m_incidentReportHelper = new IncidentReportHelper(id, isIDKey);
		
		FUNCTION_EXIT;
	}
	
	CtaIncidentReportData::~CtaIncidentReportData()
	{
		FUNCTION_ENTRY("Destructor");
		
		delete m_incidentReportHelper;
		m_incidentReportHelper = NULL;
		
		delete m_stationReportData;
		m_stationReportData = NULL;
		
		FUNCTION_EXIT;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	ItaStationReportData& CtaIncidentReportData::getStationReport()
	{
		FUNCTION_ENTRY("getStationReport");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (m_stationReportData == NULL)
		{
			//Mintao ++
			//Bug 652 (TD12313)
			std::string stationId = m_incidentReportHelper->getStationId();
			if (stationId.empty())
			{
				//TD15204 - the incident report ID will be the station report's it
				m_stationReportData = new CtaStationReportData(m_incidentReportHelper->getReportId(), true);			
			}
			else
			{
				m_stationReportData = new CtaStationReportData(stationId);
			}
		}
		//Mintao ++
		//Bug 652 (TD12313)
		
		FUNCTION_EXIT;
		return *m_stationReportData;
	}


	// TD15865 Yanrong++
	void CtaIncidentReportData::openForEdit()
	{
		FUNCTION_ENTRY( "openForEdit" );

		// Update the report state to DB 
		m_incidentReportHelper->increaseEditOperator();
		
		// Send Message to other instance to update state
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15865 openForEdit SendBroadcast" );
		SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportModified,
			TA_Base_Core::IncidentAudit::IncidentEdit,
			m_incidentReportHelper->getReportId(),
			m_incidentReportHelper->getTitle());
		
		FUNCTION_EXIT;
	}


	void CtaIncidentReportData::endEdit()
	{
		FUNCTION_ENTRY( "endEdit" );
		// Update the report state to DB 
		m_incidentReportHelper->decreaseEditOperator();

		// Send Message to other instance to update state
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15865 endEdit SendBroadcast" );
		SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportModified,
			TA_Base_Core::IncidentAudit::IncidentEdit,
			m_incidentReportHelper->getReportId(),
			m_incidentReportHelper->getTitle());
		
		FUNCTION_EXIT;
	}


	bool CtaIncidentReportData::canClose()
	{
		unsigned int edCount;
		if( m_incidentReportHelper->getEditOperatorCount(edCount) )
		{
			return edCount == 0;
		}

		return true;
	}
	// ++Yanrong
	
	
	void CtaIncidentReportData::close()
	{
		FUNCTION_ENTRY("close");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		m_incidentReportHelper->close();
        //TD16632 Mintao++
        //used for archive manager
        std::string stationId = m_incidentReportHelper->getStationId();
		if (!stationId.empty())
		{
            if (m_stationReportData == NULL )
            {
                m_stationReportData = new CtaStationReportData(stationId);
            }
            try
            {
                m_stationReportData->setClosedDate(getCloseDate());
                m_stationReportData->save();
            }
		    catch (TA_Base_Core::IncidentException& e)
		    {
			    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", e.what());
                if (m_stationReportData != NULL)
                {
                    //got exception, reset the station pointer to NULL
                    delete m_stationReportData;
                    m_stationReportData = NULL;
                }
            }        
        }
		SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportModified, TA_Base_Core::IncidentAudit::IncidentClosed,m_incidentReportHelper->getReportId(), m_incidentReportHelper->getTitle());
		
		FUNCTION_EXIT;
	}
	
	ItaIncidentReportData* CtaIncidentReportData::clone() const
	{
		FUNCTION_ENTRY("clone");
		
		CtaIncidentReportData* data = new CtaIncidentReportData(*this);
		
		FUNCTION_EXIT;
		return data;
	}
	
	void CtaIncidentReportData::deleteThisReport()
	{
		FUNCTION_ENTRY("deleteThisReport");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		m_incidentReportHelper->deleteThisReport();
		SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportRemoved, TA_Base_Core::IncidentAudit::IncidentDeleted,m_incidentReportHelper->getReportId(), m_incidentReportHelper->getTitle());
		
		FUNCTION_EXIT;
	}
	
	bool CtaIncidentReportData::isClosed()
	{
		FUNCTION_ENTRY("isClosed");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getCloseDate() != 0;
	}
	
	time_t CtaIncidentReportData::getCloseDate()
	{
		FUNCTION_ENTRY("getCloseDate");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getCloseDate();
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	std::string CtaIncidentReportData::getReportId()
	{
		FUNCTION_ENTRY("getReportId");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getReportId();
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	/**
	* @param stationId
	* 
	*/
	//Mintao ++
	//Bug 778 (TD12858)
	std::string CtaIncidentReportData::getStationId()
	{
		FUNCTION_ENTRY("getStationId");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getStationId();
	}
	//Mintao ++
	//Bug 778 (TD12858)
	
	void CtaIncidentReportData::setTitle(const std::string& title)
	{
		FUNCTION_ENTRY("setTitle");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (title != m_incidentReportHelper->getTitle())
		{
			m_incidentReportHelper->setTitle(title);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	std::string CtaIncidentReportData::getTitle()
	{
		FUNCTION_ENTRY("getTitle");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getTitle();
	}
	
	std::string CtaIncidentReportData::getCreator()
	{
		FUNCTION_ENTRY("getCreator");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getCreator();
	}
	
	void CtaIncidentReportData::reopen()
	{
		FUNCTION_ENTRY("reopen");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		m_incidentReportHelper->reopen();
		SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportModified, TA_Base_Core::IncidentAudit::IncidentReOpened,m_incidentReportHelper->getReportId(), m_incidentReportHelper->getTitle());
		
		FUNCTION_EXIT;
	}
	
	
	std::string CtaIncidentReportData::getCreatorProfile()
	{
		FUNCTION_ENTRY("getCreatorProfile");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getCreatorProfile();
	}
	
	time_t CtaIncidentReportData::getCreatedDateTime()
	{
		FUNCTION_ENTRY("getCreatedDateTime");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getCreatedDateTime();
	}
	
	
	std::string CtaIncidentReportData::getModifier()
	{
		FUNCTION_ENTRY("getModifier");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getModifier();
	}
	
	std::string CtaIncidentReportData::getModifierProfile()
	{
		FUNCTION_ENTRY("getModifierProfile");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getModifierProfile();
	}
	
	time_t CtaIncidentReportData::getModifiedDateTime()
	{
		FUNCTION_ENTRY("getModifiedDateTime");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getModifiedDateTime();
	}
	
	void  CtaIncidentReportData::setFalseIncident(bool state)
	{
		FUNCTION_ENTRY("setFalseIncident");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (state != m_incidentReportHelper->isFalseIncident())
		{
			m_incidentReportHelper->setFalseIncident(state);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	bool  CtaIncidentReportData::isFalseIncident()
	{
		FUNCTION_ENTRY("isFalseIncident");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->isFalseIncident();
	}
	
	
	void  CtaIncidentReportData::setAsset(AssetType* asset)
	{
		FUNCTION_ENTRY("setAsset");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		AssetType currentAsset = m_incidentReportHelper->getAsset();
		if (asset->id != currentAsset.id)
		{
			m_incidentReportHelper->setAsset(asset);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	AssetType  CtaIncidentReportData::getAsset()
	{
		FUNCTION_ENTRY("getAsset");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getAsset();
	}
	
	void  CtaIncidentReportData::setTrain(const std::string& train)
	{
		FUNCTION_ENTRY("setTrain");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (train != m_incidentReportHelper->getTrain())
		{
			m_incidentReportHelper->setTrain(train);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	std::string  CtaIncidentReportData::getTrain()
	{
		FUNCTION_ENTRY("getTrain");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getTrain();
	}
	
	void  CtaIncidentReportData::setInnerTrack(bool state)
	{
		FUNCTION_ENTRY("setInnerTrack");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (state != m_incidentReportHelper->getInnerTrack())
		{
			m_incidentReportHelper->setInnerTrack(state);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	bool  CtaIncidentReportData::getInnerTrack()
	{
		FUNCTION_ENTRY("getInnerTrack");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getInnerTrack();
	}
	
	void  CtaIncidentReportData::setOuterTrack(bool state)
	{
		FUNCTION_ENTRY("setOuterTrack");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (state != m_incidentReportHelper->getOuterTrack())
		{
			m_incidentReportHelper->setOuterTrack(state);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	bool  CtaIncidentReportData::getOuterTrack()
	{
		FUNCTION_ENTRY("getOuterTrack");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getOuterTrack();
	}
	
	void  CtaIncidentReportData::setKilometreLoss(unsigned long kilometres)
	{
		FUNCTION_ENTRY("setKilometreLoss");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (kilometres != m_incidentReportHelper->getKilometreLoss())
		{
			m_incidentReportHelper->setKilometreLoss(kilometres);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	unsigned long  CtaIncidentReportData::getKilometreLoss()
	{
		FUNCTION_ENTRY("getKilometreLoss");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getKilometreLoss();
	}
	
	void  CtaIncidentReportData::setLocation1(std::string location)
	{
		FUNCTION_ENTRY("setLocation1");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (location != m_incidentReportHelper->getLocation1())
		{
			m_incidentReportHelper->setLocation1(location);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	std::string  CtaIncidentReportData::getLocation1()
	{
		FUNCTION_ENTRY("getLocation1");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getLocation1();
	}
	
	void  CtaIncidentReportData::setLocation2(std::string location)
	{
		FUNCTION_ENTRY("setLocation2");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (location != m_incidentReportHelper->getLocation2())
		{
			m_incidentReportHelper->setLocation2(location);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	std::string  CtaIncidentReportData::getLocation2()
	{
		FUNCTION_ENTRY("getLocation2");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getLocation2();
	}
	
	void  CtaIncidentReportData::setStartDateTime(time_t dateTime)
	{
		FUNCTION_ENTRY("setStartDateTime");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (dateTime != m_incidentReportHelper->getStartDateTime())
		{
			m_incidentReportHelper->setStartDateTime(dateTime);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	time_t CtaIncidentReportData::getStartDateTime()
	{
		FUNCTION_ENTRY("getStartDateTime");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getStartDateTime();
	}
	
	void CtaIncidentReportData::setEndDateTime(time_t dateTime)
	{
		FUNCTION_ENTRY("setEndDateTime");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (dateTime != m_incidentReportHelper->getEndDateTime())
		{
			m_incidentReportHelper->setEndDateTime(dateTime);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	time_t CtaIncidentReportData::getEndDateTime()
	{
		FUNCTION_ENTRY("getEndDateTime");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getEndDateTime();
	}
	
	void CtaIncidentReportData::setInitialDelay(unsigned long delay)
	{
		FUNCTION_ENTRY("setInitialDelay");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (delay != m_incidentReportHelper->getInitialDelay())
		{
			m_incidentReportHelper->setInitialDelay(delay);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	unsigned long CtaIncidentReportData::getInitialDelay()
	{
		FUNCTION_ENTRY("getInitialDelay");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getInitialDelay();
	}
	
	void CtaIncidentReportData::setAccumulatedDelay(unsigned long delay)
	{
		FUNCTION_ENTRY("setAccumulatedDelay");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (delay != m_incidentReportHelper->getAccumulatedDelay())
		{
			m_incidentReportHelper->setAccumulatedDelay(delay);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	unsigned long CtaIncidentReportData::getAccumulatedDelay()
	{
		FUNCTION_ENTRY("getAccumulatedDelay");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getAccumulatedDelay();
	}
	
	std::string CtaIncidentReportData::getAssociatedAlarm()
	{
		FUNCTION_ENTRY("getAssociatedAlarm");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getAssociatedAlarm();
	}
	
	void CtaIncidentReportData::setAdditionalAlarmDescription(const std::string& description)
	{
		FUNCTION_ENTRY("setAdditionalAlarmDescription");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (description != m_incidentReportHelper->getAdditionalAlarmDescription())
		{
			m_incidentReportHelper->setAdditionalAlarmDescription(description);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	std::string CtaIncidentReportData::getAdditionalAlarmDescription()
	{
		FUNCTION_ENTRY("getAdditionalAlarmDescription");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getAdditionalAlarmDescription();
	}
	
	void CtaIncidentReportData::setPlansExecuted(std::vector<PlanType> plans)
	{
		FUNCTION_ENTRY("setPlansExecuted");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		std::vector<PlanType> currentPlans = m_incidentReportHelper->getPlansExecuted();
		unsigned int psize = currentPlans.size();
		bool changed = (psize != plans.size());
		if (!changed && psize > 0)
		{
			for (unsigned int i = 0; i < psize; ++i)
			{
				if (currentPlans[i].compare(plans[i]) != 0)
				{
					changed = true;
					break;
				}
			}
		}
		
		if (changed)
		{
			m_incidentReportHelper->setPlansExecuted(plans);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	std::vector<PlanType> CtaIncidentReportData::getPlansExecuted()
	{
		FUNCTION_ENTRY("getPlansExecuted");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getPlansExecuted();
	}
	
	void CtaIncidentReportData::setOperators(const std::string& operators)
	{
		FUNCTION_ENTRY("setOperators");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (operators != m_incidentReportHelper->getOperators())
		{
			m_incidentReportHelper->setOperators(operators);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	
	std::string CtaIncidentReportData::getOperators()
	{
		FUNCTION_ENTRY("getOperators");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getOperators();
	}
	
	void CtaIncidentReportData::setComments(const std::string& comments)
	{
		FUNCTION_ENTRY("setComments");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (comments != m_incidentReportHelper->getComments())
		{
			m_incidentReportHelper->setComments(comments);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	std::string CtaIncidentReportData::getComments()
	{
		FUNCTION_ENTRY("getComments");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getComments();
	}
	
	std::string CtaIncidentReportData::getAssociatedAlarmDetail()
	{
		FUNCTION_ENTRY("getAssociatedAlarmDetail");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getAssociatedAlarmDetail();
	}
	
	void CtaIncidentReportData::setRecommendation(const std::string& recommendation)
	{
		FUNCTION_ENTRY("setRecommendation");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (recommendation != m_incidentReportHelper->getRecommendation())
		{
			m_incidentReportHelper->setRecommendation(recommendation);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	std::string CtaIncidentReportData::getRecommendation()
	{
		FUNCTION_ENTRY("getRecommendation");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getRecommendation();
	}
	
	//Mintao ++
	//Bug 652 (TD12313)
	
	void CtaIncidentReportData::updateStationReportId(std::string  stationId)
	{
		FUNCTION_ENTRY("updateStationReportId");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		m_incidentReportHelper->updateStationReportId(stationId);
		SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportModified,TA_Base_Core::IncidentAudit::IncidentStationEdit,m_incidentReportHelper->getReportId(), m_incidentReportHelper->getTitle());	
		FUNCTION_EXIT;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	unsigned long CtaIncidentReportData::getCreateLocationId()
	{
		FUNCTION_ENTRY("getCreateLocationId");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getCreateLocationId();
	}
	
	void CtaIncidentReportData::save()
	{
		FUNCTION_ENTRY("save");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		if (m_isModified)
		{
			bool isNew = m_incidentReportHelper->isNew();
			
			m_incidentReportHelper->writeData();
			m_isModified = false;
			
			if (isNew)
			{
				//TD15089 - overloaded the function to pass the operator name when an incident is created
				//SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportAdded, TA_Base_Core::IncidentAudit::IncidentCreated,m_incidentReportHelper->getReportId(), m_incidentReportHelper->getTitle());
				SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportAdded, TA_Base_Core::IncidentAudit::IncidentCreated,m_incidentReportHelper->getReportId(), 
					m_incidentReportHelper->getTitle(),m_incidentReportHelper->getCreator());
			}
			else
			{
				SendBroadcastMessage(IncidentManagerBroadcastComms::IncidentReportModified, TA_Base_Core::IncidentAudit::IncidentEdit,m_incidentReportHelper->getReportId(), m_incidentReportHelper->getTitle());
			}
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaIncidentReportData::reload()
	{
		FUNCTION_ENTRY("reload");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		m_incidentReportHelper->reload();
		
		FUNCTION_EXIT;
	}
	
	bool CtaIncidentReportData::isModified()
	{
		FUNCTION_ENTRY("isModified");
		
		FUNCTION_EXIT;
		return m_isModified;
	}
	
	bool CtaIncidentReportData::isNew()
	{
		FUNCTION_ENTRY("isNew");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->isNew();
	}
	
	unsigned long CtaIncidentReportData::getCreatorProfileId()
	{
		FUNCTION_ENTRY("getCreatorProfileId");
		TA_ASSERT(m_incidentReportHelper != NULL, "Helper is NULL");
		
		FUNCTION_EXIT;
		return m_incidentReportHelper->getCreatorProfileId();
	}
} // end TA_IRS_Core
 