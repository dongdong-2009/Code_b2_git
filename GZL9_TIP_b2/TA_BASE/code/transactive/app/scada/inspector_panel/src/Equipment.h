#ifndef EQUIPMENT_H
#define EQUIPMENT_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/Equipment.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/07/05 10:04:52 $
  *
  * Last modified by:  $Author: jinmin.huang $
  * 
  * This class provides access to a piece of equipment and its associated data points.
  *
  */
 
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <vector>
#include <deque>
#include "boost/thread/condition_variable.hpp"
#include "boost/thread/mutex.hpp"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

namespace TA_Base_Bus
{
	class ScadaProxyFactory;
}

namespace TA_Base_App
{
	class CEquipment : public TA_Base_Bus::IEntityUpdateEventProcessor,public TA_Base_Core::Thread
	{
		public:
			CEquipment(std::string equipmentName);
			virtual ~CEquipment();
			//interface from Thread
			virtual void		run();
			virtual void		terminate();
			//interface from IEntityUpdateEventProcessor
			void				processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);
			
			bool				loadDataPoints();
			std::string 		getEquipmentName();
			TA_Base_Bus::DataNodeProxySmartPtr&					getDataNode();
			std::vector<TA_Base_Bus::DataPointProxySmartPtr*>&	getChildDataPointVector();
			TA_Base_Bus::DataPointProxySmartPtr&				getChildDataPoint(UINT iPoint);
			TA_Base_Bus::DataPointProxySmartPtr&				getChildDataPointFromEntityKey(unsigned long key);
            
            std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>&  getWfDataPointWithKeyMap(); // Using for WF

			// wenching++ (TD13353)
			// Accessor for vector of indexes
			std::vector<unsigned long>&		getWriteableIndex();
			std::vector<unsigned long>&		getNotWriteableIndex();
			std::vector<unsigned long>&		getLimitIndex();
			std::vector<unsigned long>&		getMeterIndex();
			// ++wenching (TD13353)

			
			UINT				getChildDataPointIndexFromEntityKey(unsigned long key);
			unsigned int		getChildDataPointCount();

			bool				hasControlPoint();
			bool				hasInformationPoint();
			bool				hasMeteredPoint();
			bool				hasLimitedPoint();
			bool				haveAllDataPointsConfigured();
			bool				getPScadaFlag(); //Using for IEC104 Protocol
			int					getControlPointIndex(unsigned long key);
			bool				isEntityBlank();
			unsigned long		getChildKeyByName(std::string& entityName);
            
            long                                    getWfControlState(unsigned long key, time_t inputTime); // Using for WF
            void                                    setWfControlState(unsigned long key, unsigned long controlState, time_t inputTime);  // Using for Wf
            std::map<unsigned long, long>&          getDatapointsRelatedWfMap(); // Using for WF
            boost::condition_variable&              getWfCondtion(); // Using for WF
            boost::mutex&                           getWfMutex();   // Using for WF
            bool                                    isEnableWf();
            bool									getWfErrorCode(unsigned long key, std::string& strError); // Using for WF
            
		protected:
			bool				initialize();
			bool				loadDataNodeData();
			bool 				createDataNodeProxy();
			bool				loadDataPointData();
			bool				createDataPointProxy();            
			void				initMap();	//wenching++ (TD13353)
			void				initListsHelper(); //wenching++ (TD13353)
			void 				clearDataPointObjects();
            
            void                createWfDataPointProxy();  // Using for WF
            void				clearWfDataPointObjects(); // Using for WF
            void                processWfUpdateEvent(unsigned long key, long updateType); // Using for WF

		private:
			std::string								m_equipmentName;
			TA_Base_Bus::DataNodeProxySmartPtr		m_dataNode;
			boost::shared_ptr<TA_Base_Core::DataNodeEntityData>		m_dnEntityData;
			unsigned long							m_dataNodeEntityKey;
			bool									m_isEntityBlank;
			typedef std::vector<boost::shared_ptr<TA_Base_Core::DataPointEntityData> > ChildDataPointData;
			typedef std::vector<TA_Base_Bus::DataPointProxySmartPtr*> ChildDataPoints;
			typedef std::map<unsigned long, unsigned long>	DatapointsKeyIndex; // find the datapoints index by entity key
			typedef DatapointsKeyIndex::iterator			DatapointsKeyIndexIt;
			typedef std::map<unsigned long, long>			ControlDataPointRelatedMap; // find the control data point and its related input data point
			typedef ControlDataPointRelatedMap::iterator	ControlDataPointRelatedMapIt; 
			ChildDataPointData		m_dataPointEntities;
			ChildDataPoints			m_dataPoints;
            ChildDataPoints         m_wfPoints; // Using for WF
			DatapointsKeyIndex		m_datapointsKeyIndexMap;
			const unsigned int		m_maxDataPoints;
			volatile bool			m_hasControlPoint;
			volatile bool			m_hasInformationPoint;
			volatile bool			m_hasMeteredPoint;
			volatile bool			m_hasLimitedPoint;
			volatile bool			m_haveAllDataPointConfigured;

			std::vector<unsigned long>				m_writeableIpoint;		// Vector of indexes (information points)
			std::vector<unsigned long>				m_notWriteableIpoint;		// Vector of indexes (control points)
			std::vector<unsigned long>				m_meterIpoint;				// Vector of indexes (meter points)
			std::vector<unsigned long>				m_limitIpoint;				// Vector of indexes (limit points)

            ControlDataPointRelatedMap				m_datapointsRelatedMap;  // Using for WF
            std::map<unsigned long, TA_Base_Bus::DataPointProxySmartPtr*>  m_wfDatapointsWithKeyMap; // Using for WF, first element is wufang entity key, second is wufang datapoint proxy
            
			TA_Base_Core::ReEntrantThreadLockable	m_indexVectorLock;

			TA_Base_Bus::ScadaProxyFactory*			m_proxyFactory;
			//jinmin++ for thread
			typedef std::pair<unsigned long,TA_Base_Bus::ScadaEntityUpdateType> ScadaProxyMsgQueItem;
			typedef std::deque<ScadaProxyMsgQueItem>	ScadaProxyMsgQue;
			ScadaProxyMsgQue							m_scadaProxyMsgQue;
			TA_Base_Core::NonReEntrantThreadLockable	m_queueLock;
			TA_Base_Core::Semaphore						m_semaphore;
			volatile bool							    m_isRunning;
            
            // Using for WF
            TA_Base_Core::ReEntrantThreadLockable		                m_wfControlStateLock;
            TA_Base_Bus::EDataPointControlState			                m_wfControlState;
            std::map<unsigned long, std::vector<unsigned long> >		m_wfControlStateMap;
            boost::mutex                                                m_wfMutex;
            boost::condition_variable                                   m_wfCondition;
            bool                                                        m_isEnableWf;
            // Using for WF
	};
}
#endif // #ifndef EQUIPMENT_H
