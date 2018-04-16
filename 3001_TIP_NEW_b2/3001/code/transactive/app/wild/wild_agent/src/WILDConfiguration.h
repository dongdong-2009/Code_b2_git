/* 
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/wild/wild_agent/src/WILDConfiguration.h $
 * @author:  <Andy Siow>
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * This class is responsible for caching the configuration parameters during 
 * startup. Parameters will be set via the configuration editor and online update
 * mechanism.
 */

#if !defined(AFX_WILDCONFIGURATION_H__840B66C7_24EF_472C_B741_EB115D63A17B__INCLUDED_)
#define AFX_WILDCONFIGURATION_H__840B66C7_24EF_472C_B741_EB115D63A17B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include <string>

namespace TA_IRS_App
{
	class WILDConfiguration  
	{
	public:

			//
			// getInstance
			//
			// @return Returns the single instance of the DataPoint Cache
			//
			static WILDConfiguration* getInstance();

			//
			// removeInstance
			//
			// Removes the instance of the class (if already created) and cleans up 
			// the DataPoint Cache.
			//
			static void removeInstance();

			// The set methods for entity configurations
			void setAgentKey(unsigned long param);
			void setAgentLocationKey(unsigned long param);
			void setAgentSubsystemKey(unsigned long param);
			void setAgentTypeKey(unsigned long param);
			void setCorrelationThreshold(unsigned long param);
			void setAgentName(const std::string& param);
			void setAgentAssetName(const std::string& param);
			void setAgentLocationName(const std::string& param);
			void setAgentSubsystemName(const std::string& param);
			void setAgentATSWILDQueueSize(unsigned int param);
			void setWILDDetailsToken(const std::string& param);
			void setCorrelationThresholdTimeoutSecs(long param);

			// The get methods for the corresponding entity configurations
			unsigned long  getAgentKey() const; 
			unsigned long getAgentLocationKey() const;
			unsigned long getAgentSubsystemKey() const;
			unsigned long getAgentTypeKey() const;
			unsigned long getCorrelationThreshold() const;
			unsigned int getAgentATSWILDQueueSize() const;
			long getCorrelationThresholdTimeoutSecs() const;
			std::string getAgentName() const;
			std::string getAgentAssetName() const;
			std::string getAgentLocationName() const;
			std::string getAgentSubsystemName() const;
			std::string getWILDDetailsToken() const;		

	private:
			// Disable the copy constructor and assignment operator so the compiler won't create its own version.
			// Reference: Effective C++
			WILDConfiguration( const WILDConfiguration& WILDConfiguration);
			WILDConfiguration& operator=(const WILDConfiguration &);

			// Thread guard locks
			static TA_Base_Core::NonReEntrantThreadLockable m_configLock;

			// The singleton's instance..
			static WILDConfiguration* m_instance;

			WILDConfiguration();
			virtual ~WILDConfiguration();

			// Private configuration variables for the WILD Agent entity
			unsigned long m_agentKey;
			unsigned long m_agentLocationKey;
			std::string m_agentName;
			std::string m_agentLocationName;
			std::string m_agentSubsystemName;
			std::string m_agentAssetName;
			std::string m_wildDetailsToken;
			unsigned long m_agentSubsystemKey;
			unsigned long m_agentTypeKey;
			unsigned long m_correlationThreshold;
			unsigned int m_agentATSWILDQueueSize;
			unsigned int m_correlationTimeoutSecs;

	};
}

#endif // !defined(AFX_WILDCONFIGURATION_H__840B66C7_24EF_472C_B741_EB115D63A17B__INCLUDED_)
