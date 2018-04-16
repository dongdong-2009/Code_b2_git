/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/incidents/incident_rights/src/CtaIncidentRights.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Incident Rights Class
  */

#include <sstream>

#include "bus/incidents/incident_rights/src/CtaIncidentRights.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "core/data_access_interface/incident_dai/src/ItaIncidentReportData.h"
#include "core/data_access_interface/incident_dai/src/ItaStationReportData.h"

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/IProfile.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/exceptions/src/IncidentException.h"

#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/incident_dai/src/CtaReportAccessFactory.h"

#include "core/configuration_updates/src/OnlineUpdateListener.h" // TD15203
using namespace std;
using namespace TA_Base_Core;

namespace TA_IRS_Bus
{
	
	CtaIncidentRights::CtaIncidentRights()
		:m_bOnline(true)//when starting up, should set it to true
	{
		FUNCTION_ENTRY("Constructor");
		m_incidentReportAccess = new CtaReportAccessFactory();
		
		TA_ASSERT(m_incidentReportAccess != NULL, "The report access right factory is NULL!");
		m_incidentReportAccessMatrix = m_incidentReportAccess->getAccessMetrix(INCIDENT_REPORT);
		m_stationReportAccessMatrix = m_incidentReportAccess->getAccessMetrix(STATION_REPORT);
		m_profileGroupMap.clear();
		
		vector<IProfile*> profileData = ProfileAccessFactory::getInstance().getAllProfiles();
		vector<IProfile*>::iterator it;
		for (it = profileData.begin(); it != profileData.end(); ++it)
		{
			unsigned long profilekey = (*it)->getKey();
			unsigned long profilegroupkey = (*it)->getAccessControlGroup();
			m_profileGroupMap.insert(pair<unsigned long, unsigned long>(profilekey, profilegroupkey));
		}
		
		//after we are done with the Iprofile data, clean up
		for (it = profileData.begin(); it != profileData.end(); ++it)
		{
			delete *it;   
		}
		
		RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
		m_authMgr = new TA_Base_Bus::AuthenticationLibrary(true);
		
		// TD15203 - added to listen for Online updates
		try
		{
			TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::INCIDENT_RIGHTS ,*this );
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Listening to online  updates for INCIDENT_RIGHTS");
			
		}
		catch (...) 
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "from registerAllInterests");
		}
		
		try
		{
			//TD12923 - keep track of OCC locationkey
			m_occLocationKey = LocationAccessFactory::getInstance().getOccLocationKey();
			//Get Accessor Profile///////////////////////////////////////////////////////////////
			string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
			
			ISession* session = SessionAccessFactory::getInstance().getSession(sessionId); 
			//TD12923 - keep track of the physical location key
			IEntityData *pEntityData = EntityAccessFactory::getInstance().getEntity(session->getConsoleKey());
			
			m_consoleLocationKey = pEntityData->getLocation();
			
			delete pEntityData;
			
			//TD12923
			// sant - call onRunParamChange to get it handled the same way
			onRunParamChange(RPARAM_SESSIONID, sessionId);
			
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Successfully obtained session profiles from the Authentication Agent");
		}
		catch(CORBA::Exception& ex)                                                                                     
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); 
		}                                                                                                               
		catch(TA_Base_Core::ObjectResolutionException& ex)                                                                   
		{                                                                                                               
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());                                    
		}                                                                                                               
		catch (...) 
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Corba Exception", "Fail to get session profiles from the Authentication Agent");
		}
		
		FUNCTION_EXIT;
	}
	
	CtaIncidentRights::~CtaIncidentRights()
	{
		FUNCTION_ENTRY("Destructor");
		
		// TD15203 - added to stop listening for Online updates
		try
		{		
			TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::INCIDENT_RIGHTS, *this );
			LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
				"Stopped listening to online  updates for INCIDENT_RIGHTS");
		}
		catch (...) 
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "from deregisterAllInterests");
		}	
		
		if (m_authMgr != NULL)
		{
			delete m_authMgr;
			m_authMgr = NULL;
		}
		
		if (m_incidentReportAccess != NULL)
		{
			delete m_incidentReportAccess;
			m_incidentReportAccess = NULL;
		}
		FUNCTION_EXIT;
	}
	
	void CtaIncidentRights::onRunParamChange(const std::string& name, const std::string& value)
	{
		FUNCTION_ENTRY("CtaIncidentRights::onRunParamChange");
		
		if (name.compare(RPARAM_SESSIONID) == 0)
		{
			try
			{
				string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
				ISession* session = SessionAccessFactory::getInstance().getSession(sessionId);    
				// get the new list of profiles associated with the session
				m_sessionProfiles = m_authMgr->getProfiles(sessionId);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Successfully obtained session profiles from the Authentication Agent");
				
				//TD12923
				// cache the location key so that we don't have to figure out the location key each time in isViewPermitted/isControlPermitted
				m_locationKey = session->getLocationKey();
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Successfully obtained location from the database");
				
				delete session;
				session = NULL;
			}
			catch(CORBA::Exception& ex)                                                                                     
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); 
			}                                                                                                               
			catch(TA_Base_Core::ObjectResolutionException& ex)                                                                   
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());                                    
			}                                                                                                               
			catch(TA_Base_Core::TransactiveException&)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Data/Database Exception", "Caught either data/database exception when attempting to get session/location from database"); 
			}                                                                                                               
			catch (...) 
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Corba Exception", "Fail to get session profiles from the Authentication Agent");
			}
			
		}
		FUNCTION_EXIT;
	}
	
	void CtaIncidentRights::setServerState(bool isOnline)
	{
		FUNCTION_ENTRY("setServerState");
		
		if(isOnline)
		{
			m_bOnline = true;
		}
		else
		{
			m_bOnline = false;
		}
		
		FUNCTION_EXIT;
	}
	
	//Mintao ++
	//Bug 772(TD12924)
	bool CtaIncidentRights::isViewPermitted(ItaIncidentReportData& incident)
	{
		FUNCTION_ENTRY("isViewPermitted(IncidentReportData)");
		
		bool bReturn = false;
		if(m_bOnline)
		{        
			EAccessType right = NO_CONTROL;
			
			std::vector<unsigned long>::iterator itPro;
			for (itPro = m_sessionProfiles.begin(); itPro != m_sessionProfiles.end();++itPro)
			{
				//Get Access Profile Group////////////////////////////////////////////////////////////////
				unsigned long accessorProfileKey = *itPro;
				unsigned long AccessorProfileGroupId;
				std::map<unsigned long, unsigned long>::iterator itaccess;
				itaccess = m_profileGroupMap.find(accessorProfileKey);
				if ( itaccess != m_profileGroupMap.end())
				{
					AccessorProfileGroupId = (*itaccess).second;
				}
				else
				{
					//TD15125 -a profile could be deleted therefore the this method
					// should not assert. rather, break out from the loop.
					// it could not be possible that the accessing profile is from a 
					// deleted profile
					//TA_ASSERT(false , "Can not find the specific profile group!");
					try
					{
                        // TD15125 - the session might be using a new profile that has been added after the incident manager started
                        IProfile* profileData = NULL;
						profileData = ProfileAccessFactory::getInstance().getProfile(accessorProfileKey);						
						if (profileData != NULL)
						{   // must be a new profile
							AccessorProfileGroupId = profileData->getAccessControlGroup();
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(accessorProfileKey, AccessorProfileGroupId));
						}
						else 
						{	// could be a deleted profile							
							AccessorProfileGroupId = ProfileAccessFactory::getInstance().getDeletedProfileGroup(accessorProfileKey);
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(accessorProfileKey, AccessorProfileGroupId));
						}
						delete profileData;
						profileData = NULL;
						
					}
					catch(DataException&)
					{
                        LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", "Profile never existed");
                        // if this profile is not in the database for some reason, just ignore it and go to the next profile
                        // in the list
                        continue;
					}               					
					//++lizettejl
				}
				//Get Creator Profile Group////////////////////////////////////////////////////////////////
				unsigned long creatorProfileKey = incident.getCreatorProfileId();
				unsigned long creatorProfileGroupId;
				std::map<unsigned long, unsigned long>::iterator itcreate;
				itcreate = m_profileGroupMap.find(creatorProfileKey);
				if ( itcreate != m_profileGroupMap.end())
				{
					creatorProfileGroupId = (*itcreate).second;
				}
				else
				{
					//TD15125 -the creating profile must have been deleted earlier
					// since it no longer exists in the profile group map, it needs
					// to be cached in. everytime this incident report is reloaded, 
					// it only queries in the database once.
					// try searching the profile on one of the deleted groups
					try
					{
						creatorProfileGroupId = ProfileAccessFactory::getInstance().getDeletedProfileGroup(creatorProfileKey);
						m_profileGroupMap.insert(pair<unsigned long, unsigned long>(creatorProfileKey, creatorProfileGroupId));
						
					}
					catch(DataException&)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", "Profile never existed");
					}		
					
					//++lizettejl
				}
				
				AccessMatrix::iterator matIter;
				ProfileAccess::iterator proIter;
				ProfileAccess creatorProfileAccess;
				
				matIter = m_incidentReportAccessMatrix.find(AccessorProfileGroupId);
				if(matIter != m_incidentReportAccessMatrix.end() )   
				{
					creatorProfileAccess = (*matIter).second;
					proIter = creatorProfileAccess.find(creatorProfileGroupId);
					
					//When both creator and accessor are found
					if(proIter != creatorProfileAccess.end())
					{
						right = (*proIter).second;
						break;
					}
				}
			}
			
			switch(right)
			{
            case NO_CONTROL:
				{
					bReturn = false;
					break;
				}
            case CONTROL_OWN:
				{
					//TD12923
					if (m_locationKey == incident.getCreateLocationId())
					{
						bReturn = true;
					}
					break;
				}
            case CONTROL_OCC:
				{                
					//TD12923 - compare physical location key instead of the virtual location key
					//if(m_locationKey == m_occLocationKey)				
					if(m_consoleLocationKey == m_occLocationKey)
					{
						bReturn = true;
					}            
					break;
				}
            default:
				{
					bReturn = true;
					break;
				}
			}
		}
		else
		{
			bReturn = false;
		}
		
		FUNCTION_EXIT;
		return bReturn; 
	}

	bool CtaIncidentRights::isViewPermitted(ItaStationReportData& stationReport)
	{
		FUNCTION_ENTRY("isViewPermitted(StationReportData)");
		
		bool bReturn = false;
		if(m_bOnline)
		{
			EAccessType right = NO_CONTROL;
			
			std::vector<unsigned long>::iterator itPro;
			for (itPro = m_sessionProfiles.begin(); itPro != m_sessionProfiles.end();++itPro)
			{
				//Get Access Profile Group////////////////////////////////////////////////////////////////
				unsigned long accessorProfileKey = *itPro;
				unsigned long AccessorProfileGroupId;
				std::map<unsigned long, unsigned long>::iterator itaccess;
				itaccess = m_profileGroupMap.find(accessorProfileKey);
				if ( itaccess != m_profileGroupMap.end())
				{
					AccessorProfileGroupId = (*itaccess).second;
				}
				else
				{
					
					//TD15125 -a profile could be deleted therefore the this method
					// should not assert. rather, break out from the loop.
					// it could not be possible that the accessing profile is from a 
					// deleted profile
					//TA_ASSERT(false , "Can not find the specific profile group!");
					try
					{
                        // TD15125 - the session might be using a new profile that has been added after the incident manager started
                        IProfile* profileData = NULL;
						profileData = ProfileAccessFactory::getInstance().getProfile(accessorProfileKey);						
						if (profileData != NULL)
						{   // must be a new profile
							AccessorProfileGroupId = profileData->getAccessControlGroup();
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(accessorProfileKey, AccessorProfileGroupId));
						}
						else 
						{	// could be a deleted profile							
							AccessorProfileGroupId = ProfileAccessFactory::getInstance().getDeletedProfileGroup(accessorProfileKey);
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(accessorProfileKey, AccessorProfileGroupId));
						}
						delete profileData;
						profileData = NULL;
					}
					catch(DataException&)
					{
                        LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", "Profile never existed");
                        // if this profile is not in the database for some reason, just ignore it and go to the next profile
                        // in the list
                        continue;
					}               
					//++lizettejl
				}
				//Get Creator Profile Group////////////////////////////////////////////////////////////////
				unsigned long creatorProfileKey = stationReport.getCreatorProfile();
				unsigned long creatorProfileGroupId;
				std::map<unsigned long, unsigned long>::iterator itcreate;
				itcreate = m_profileGroupMap.find(creatorProfileKey);
				if ( itcreate != m_profileGroupMap.end())
				{
					creatorProfileGroupId = (*itcreate).second;
				}
				else
				{
					//TD15125 -the creating profile must have been deleted earlier
					// since it no longer exists in the profile group map, it needs
					// to be cached in. everytime this incident report is reloaded, 
					// it only queries in the database once.
					// try searching the profile on one of the deleted groups
					// TA_ASSERT(false , "Can not find the specific profile group!");
					try
					{
						creatorProfileGroupId = ProfileAccessFactory::getInstance().getDeletedProfileGroup(creatorProfileKey);
						m_profileGroupMap.insert(pair<unsigned long, unsigned long>(creatorProfileKey, creatorProfileGroupId));
						
					}
					catch(DataException&)
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", "Profile never existed");
					}		
				}
				
				AccessMatrix::iterator matIter;
				ProfileAccess::iterator proIter;
				ProfileAccess creatorProfileAccess;
				
				matIter = m_stationReportAccessMatrix.find(AccessorProfileGroupId);
				if(matIter != m_stationReportAccessMatrix.end() )   
				{
					creatorProfileAccess = (*matIter).second;
					proIter = creatorProfileAccess.find(creatorProfileGroupId);
					//When both creator and accessor are found
					if(proIter != creatorProfileAccess.end())
					{
						right = (*proIter).second;
						break;
					}
				}
			}
			
			switch(right)
			{
			case NO_CONTROL:
				{
					bReturn = false;
					break;
				}
			case CONTROL_OWN:
				{
					if (m_locationKey == stationReport.getCreateLocationId())
					{
						bReturn = true;
					}
					break;
				}
			case CONTROL_OCC:
				{                
					//TD12923 - compare physical location key instead of the virtual location key
					//if(m_locationKey == m_occLocationKey)
					if(m_consoleLocationKey == m_occLocationKey) 
					{
						bReturn = true;
					}            
					break;
				}
			default:
				{
					bReturn = true;
					break;
				}
			}
		}
		else
		{
			bReturn = false;
		}
    
		FUNCTION_EXIT;
		return bReturn;
	}
	//Mintao ++
	//Bug 772(TD12924)

	bool CtaIncidentRights::isControlPermitted(ItaIncidentReportData* incident)
	{
		FUNCTION_ENTRY("isControlPermitted(IncidentReportData)");
		
		bool bReturn = false;
		if(m_bOnline)
		{
			EAccessType right = NO_CONTROL;
			
			std::vector<unsigned long>::iterator itPro;
			for (itPro = m_sessionProfiles.begin(); itPro != m_sessionProfiles.end();++itPro)
			{
				//Get Access Profile Group////////////////////////////////////////////////////////////////
				unsigned long accessorProfileKey = *itPro;
				unsigned long AccessorProfileGroupId;
				std::map<unsigned long, unsigned long>::iterator itaccess;
				itaccess = m_profileGroupMap.find(accessorProfileKey);
				if ( itaccess != m_profileGroupMap.end())
				{
					AccessorProfileGroupId = (*itaccess).second;
				}
				else
				{
					//TD15125 -a profile could be deleted therefore the this method
					// should not assert. rather, break out from the loop.
					// it could not be possible that the accessing profile is from a 
					// deleted profile
					//TA_ASSERT(false , "Can not find the specific profile group!");
					try
					{
                        // TD15125 - the session might be using a new profile that has been added after the incident manager started
                        IProfile* profileData = NULL;
						profileData = ProfileAccessFactory::getInstance().getProfile(accessorProfileKey);						
						if (profileData != NULL)
						{   // must be a new profile
							AccessorProfileGroupId = profileData->getAccessControlGroup();
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(accessorProfileKey, AccessorProfileGroupId));
						}
						else 
						{	// could be a deleted profile							
							AccessorProfileGroupId = ProfileAccessFactory::getInstance().getDeletedProfileGroup(accessorProfileKey);
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(accessorProfileKey, AccessorProfileGroupId));
						}
						delete profileData;
						profileData = NULL;
					}
					catch(DataException&)
					{
                        LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", "Profile never existed");
                        // if this profile is not in the database for some reason, just ignore it and go to the next profile
                        // in the list
                        continue;
					}               
					//++lizettejl
				}
				
				AccessMatrix::iterator matIter;
				ProfileAccess::iterator proIter;
				ProfileAccess creatorProfileAccess;
				//When a new incident Report was created 
				if(incident == NULL)
				{
					matIter = m_incidentReportAccessMatrix.find(AccessorProfileGroupId);
					if(matIter != m_incidentReportAccessMatrix.end() )   
					{
						creatorProfileAccess = (*matIter).second;
						//When Accessor and Creator are the same person
						proIter = creatorProfileAccess.find(AccessorProfileGroupId);
						
						if(proIter != creatorProfileAccess.end())
						{
							right = (*proIter).second;
							break;
						}
					}
				}
				else
				{
					//Get Creator Profile Group////////////////////////////////////////////////////////////////
					unsigned long creatorProfileKey = incident->getCreatorProfileId();
					unsigned long creatorProfileGroupId;
					std::map<unsigned long, unsigned long>::iterator itcreate;
					itcreate = m_profileGroupMap.find(creatorProfileKey);
					if ( itcreate != m_profileGroupMap.end())
					{
						creatorProfileGroupId = (*itcreate).second;
					}
					else
					{
						//TD15125 -the creating profile must have been deleted earlier
						// since it no longer exists in the profile group map, it needs
						// to be cached in. everytime this incident report is reloaded, 
						// it only queries in the database once.
						// try searching the profile on one of the deleted groups
						try
						{
							creatorProfileGroupId = ProfileAccessFactory::getInstance().getDeletedProfileGroup(creatorProfileKey);
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(creatorProfileKey, creatorProfileGroupId));
							
						}
						catch(DataException&)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", "Profile never existed");
						}		
					}
					
					
					matIter = m_incidentReportAccessMatrix.find(AccessorProfileGroupId);
					if(matIter != m_incidentReportAccessMatrix.end() )   
					{
						//Both Accessor and Creator has the same ID
						creatorProfileAccess = (*matIter).second;
						proIter = creatorProfileAccess.find(creatorProfileGroupId);
						
						if(proIter != creatorProfileAccess.end())
						{
							right = (*proIter).second;
							break;
						}
					}
				}
			}
			
			switch(right)
			{
			case CONTROL_ALL:
				{
					bReturn = true;
					break;
				}
				
			case CONTROL_OCC:
				{   
					//TD12923 - compare physical location key instead of the virtual location key
					//if(m_locationKey == m_occLocationKey)
					if(m_consoleLocationKey == m_occLocationKey)
					{
						bReturn = true;
					}            
					break;
				}
				
			case CONTROL_OWN:
				{
					//Mintao ++
					//Bug 771(TD12922)
					//If it is created at the same session location
					//if ((incident == NULL) || (incident != NULL) && (m_locationKey == incident->getCreateLocationId()))
					if ((incident == NULL) || ((incident != NULL) && (m_locationKey == incident->getCreateLocationId())))
					{
						bReturn = true;
					}
					//Mintao ++
					//Bug 771(TD12922)
					break;
				}
				
			default:
				{
					bReturn = false;
					break;
				}
			}
		}
		else
		{
			bReturn = false;
		}
		
		FUNCTION_EXIT;
		return bReturn;
	}

	bool CtaIncidentRights::isControlPermitted(ItaStationReportData* stationReport)
	{
		FUNCTION_ENTRY("isControlPermitted(StationReportData)");
		
		bool bReturn = false;
		if(m_bOnline)
		{
			EAccessType right = NO_CONTROL;
			
			std::vector<unsigned long>::iterator itPro;
			for (itPro = m_sessionProfiles.begin(); itPro != m_sessionProfiles.end();++itPro)
			{
				//Get Access Profile Group////////////////////////////////////////////////////////////////
				unsigned long accessorProfileKey = *itPro;
				unsigned long AccessorProfileGroupId;
				std::map<unsigned long, unsigned long>::iterator itaccess;
				itaccess = m_profileGroupMap.find(accessorProfileKey);
				if ( itaccess != m_profileGroupMap.end())
				{
					AccessorProfileGroupId = (*itaccess).second;
				}
				else
				{
					//TD15125 -a profile could be deleted therefore the this method
					// should not assert. rather, break out from the loop.
					// it could not be possible that the accessing profile is from a 
					// deleted profile
					//TA_ASSERT(false , "Can not find the specific profile group!");
					try
					{
                        // TD15125 - the session might be using a new profile that has been added after the incident manager started
						 IProfile* profileData = NULL;
						profileData = ProfileAccessFactory::getInstance().getProfile(accessorProfileKey);						
						if (profileData != NULL)
						{   // must be a new profile
							AccessorProfileGroupId = profileData->getAccessControlGroup();
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(accessorProfileKey, AccessorProfileGroupId));
						}
						else 
						{	// could be a deleted profile							
							AccessorProfileGroupId = ProfileAccessFactory::getInstance().getDeletedProfileGroup(accessorProfileKey);
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(accessorProfileKey, AccessorProfileGroupId));
						}
						delete profileData;
						profileData = NULL;
					}
					catch(DataException&)
					{
                        LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", "Profile never existed");
                        // if this profile is not in the database for some reason, just ignore it and go to the next profile
                        // in the list
                        continue;
					}               
					//++lizettejl
				}
				
				AccessMatrix::iterator matIter;
				ProfileAccess::iterator proIter;
				ProfileAccess creatorProfileAccess;
				
				//When the new station Report is created
				if(stationReport == NULL)
				{
					matIter = m_stationReportAccessMatrix.find(AccessorProfileGroupId);
					if(matIter != m_stationReportAccessMatrix.end() )   
					{
						creatorProfileAccess = (*matIter).second;
						//When the accessor and creator are the same person
						proIter = creatorProfileAccess.find(AccessorProfileGroupId);
						
						if(proIter != creatorProfileAccess.end())
						{
							right = (*proIter).second;
							break;
						}
					}
				}
				else
				{
					//Get Creator Profile Group////////////////////////////////////////////////////////////////
					unsigned long creatorProfileKey = stationReport->getCreatorProfile();
					unsigned long creatorProfileGroupId;
					std::map<unsigned long, unsigned long>::iterator itcreate;
					itcreate = m_profileGroupMap.find(creatorProfileKey);
					if ( itcreate != m_profileGroupMap.end())
					{
						creatorProfileGroupId = (*itcreate).second;
					}
					else
					{
						//TD15125 -the creating profile must have been deleted earlier
						// since it no longer exists in the profile group map, it needs
						// to be cached in. everytime this incident report is reloaded, 
						// it only queries in the database once.
						// try searching the profile on one of the deleted groups
						// TA_ASSERT(false , "Can not find the specific profile group!");
						try
						{
							creatorProfileGroupId = ProfileAccessFactory::getInstance().getDeletedProfileGroup(creatorProfileKey);
							m_profileGroupMap.insert(pair<unsigned long, unsigned long>(creatorProfileKey, creatorProfileGroupId));
							
						}
						catch(DataException&)
						{
							LOG_EXCEPTION_CATCH(SourceInfo, "Data Exception", "Profile never existed");
						}		
					}
					
					matIter = m_stationReportAccessMatrix.find(AccessorProfileGroupId);
					if(matIter != m_stationReportAccessMatrix.end() )   
					{
						creatorProfileAccess = (*matIter).second;
						
						proIter = creatorProfileAccess.find(creatorProfileGroupId);
						
						//When both creator and accessor are found
						if(proIter != creatorProfileAccess.end())
						{
							right = (*proIter).second;
							break;
						}
					}
				}
			}
			switch(right)
			{
			case CONTROL_ALL:
				{
					bReturn = true;
					break;
				}
				
			case CONTROL_OCC:
				{
					//TD12923 - compare physical location key instead of the virtual location key
					//if(m_locationKey == m_occLocationKey)
					if(m_consoleLocationKey == m_occLocationKey)                  
					{
						bReturn = true;
					}            
					break;
				}
				
			case CONTROL_OWN:
				{
					//Mintao ++
					//Bug 771(TD12922)
					//If it is created at the same session location
					if((stationReport == NULL) || ((stationReport != NULL) && (m_locationKey == stationReport->getCreateLocationId())))
					{
						bReturn = true;
					}
					break;
					//Mintao ++
					//Bug 771(TD12922)
				}
				
			default:
				{
					bReturn = false;
					break;
				}
			}
		}
		else
		{
			bReturn = false;
		}
		
		FUNCTION_EXIT;
		return bReturn;
	}

	bool CtaIncidentRights::doesIncidentSessionMatch(ItaIncidentReportData& incident)
	{
		FUNCTION_ENTRY("doesIncidentSessionMatch");
		
		//TD12923 - modified implementation
		
		bool bSessionMatch = false;
		
		if(!m_bOnline)
		{
			return bSessionMatch;
		}
		
		//Get Profile////////////////////////////////////////////////////////////////////////
		unsigned long creatorProfileId = incident.getCreatorProfileId();
		
		// need to check against all session profiles rather than just the login profile
		std::vector<unsigned long>::iterator itPro;
		for (itPro = m_sessionProfiles.begin(); itPro != m_sessionProfiles.end();++itPro)
		{
			if (creatorProfileId == *itPro)
			{
				bSessionMatch = (incident.getCreateLocationId() == m_locationKey);
				break;
			}
		}
		
		FUNCTION_EXIT;
		return bSessionMatch;
	}

	//TD15203 - added to support Online Update 
	void CtaIncidentRights::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("processUpdate");
		
		bool bIsIncident = true;
		
		if (updateEvent.getType() == TA_Base_Core::INCIDENT_RIGHTS)
		{
			if (updateEvent.getModifications() == TA_Base_Core::Update)
			{
				//expected to return nonzero if Incident report matrix needs to be updated
				if (updateEvent.getKey() == INCIDENT_REPORT) 
				{
					m_incidentReportAccessMatrix = m_incidentReportAccess->getAccessMetrix(INCIDENT_REPORT);
					LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
						"Incident Report Access Rights Matrix has been updated");
				}
				else
				{
					m_stationReportAccessMatrix = m_incidentReportAccess->getAccessMetrix(STATION_REPORT);
					LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
						"Station Report Access Rights Matrix has been updated");
				}
				
				string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
				TA_Base_Core::RunParams::getInstance().set(RPARAM_SESSIONID,sessionId.c_str());
			}
		}
		FUNCTION_EXIT;
	}
}// end TA_IRS_Bus
