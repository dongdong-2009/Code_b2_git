#ifndef EQUIPMENT_H
#define EQUIPMENT_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/inspector_panel/src/Equipment.h $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/01/08 11:35:48 $
  *
  * Last modified by:  $Author: builder $
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

#include "core/synchronisation/src/ThreadGuard.h"

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
	class CEquipment : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
		public:

			CEquipment(std::string equipmentName);
			virtual ~CEquipment();

			bool				loadDataPoints();

			TA_Base_Bus::DataNodeProxySmartPtr&		getDataNode();

			std::vector<TA_Base_Bus::DataPointProxySmartPtr*>&
								getChildDataPointVector();
			TA_Base_Bus::DataPointProxySmartPtr&	
								getChildDataPoint(UINT iPoint);
			TA_Base_Bus::DataPointProxySmartPtr&	
								getChildDataPointFromEntityKey(unsigned long key);
			UINT				getChildDataPointIndexFromEntityKey(unsigned long key);
			unsigned int		getChildDataPointCount();
			// wenching++ (TD13353)
			// Accessor for vector of indexes
			std::vector<unsigned long>&		getWriteableIndex();
			std::vector<unsigned long>&		getNotWriteableIndex();
			std::vector<unsigned long>&		getLimitIndex();
			std::vector<unsigned long>&		getMeterIndex();
			// ++wenching (TD13353)

			void				processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

			void				initMap();	//wenching++ (TD13353)
			void				initListsHelper(); //wenching++ (TD13353)
			void				checkAllConfigValid(); //wenching++ (TD13353)
			bool				hasControlPoint();
			bool				hasInformationPoint();
			bool				hasMeteredPoint();
			bool				haveAllDataPointsConfigured();

		protected:

			bool				createDataPointObjects();
			void				clearDataPointObjects();

		private:

			std::vector<TA_Base_Bus::DataPointProxySmartPtr*> m_dataPoints;
			TA_Base_Bus::DataNodeProxySmartPtr		m_dataNode;
			const unsigned int						m_maxDataPoints;
			volatile bool									m_hasControlPoint;
			volatile bool									m_hasInformationPoint;
			volatile bool									m_hasMeteredPoint;
			volatile bool									m_haveAllDataPointConfigured;
			TA_Base_Bus::ScadaProxyFactory*			m_proxyFactory;				// wenching++ (TD12597)
			// wenching++ (TD13353)
			typedef std::map<unsigned long, unsigned long>	DatapointsKeyIndex; // find the datapoints index by entity key
			typedef DatapointsKeyIndex::iterator			DatapointsKeyIndexIt; 
			DatapointsKeyIndex						m_datapointsKeyIndexMap; 

			std::vector<unsigned long>				m_writeableIpoint;			// Vector of indexes (information points)
			std::vector<unsigned long>				m_notWriteableIpoint;		// Vector of indexes (control points)
			std::vector<unsigned long>				m_meterIpoint;				// Vector of indexes (meter points)
			std::vector<unsigned long>				m_limitIpoint;				// Vector of indexes (limit points)
			// ++wenching (TD13353)				

			TA_Base_Core::ReEntrantThreadLockable	m_indexVectorLock;

	};
}
#endif // #ifndef EQUIPMENT_H
