/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source : $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/InputAssociation.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *
  *	InputAssociation represent the DataPoint entity that is associated
  * with a Derive DataPoint.
  *
  */

#ifndef DERIVED_INPUT_ASSOCIATION_H
#define DERIVED_INPUT_ASSOCIATION_H

#include <sys/timeb.h>
#include <map>
#include <vector>

#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/IWorkItem.h"

namespace TA_Base_Core
{
    // forward declaration
	class ConfiguredInputAssociation;
	class ThreadPoolSingletonManager;
};

namespace TA_Base_Bus
{
    // forward declaration
	class IDataPointProxyWithoutAlarmAckStatus;
	class DataPointState;
	class DpValue;
	class ScadaProxyFactory;

    class InputAssociation : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:

		InputAssociation ( TA_Base_Core::ConfiguredInputAssociation * configuredInputAssociation );
		virtual ~InputAssociation();


		//
		// operations specific to IEntityUpdateEventProcessor
		//


		virtual void processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType );


		//
		// Operations specific to IWorkItemCallback
		//


		virtual void executeWorkItem();


		//
		// operations specific to InputAssociation
		//


		void setOperationMode ( bool control );
		void registerForStateChangeNotification ( TA_Base_Bus::IEntityUpdateEventProcessor * callback );


		/**
		 * getInputDataPointKey
		 *
		 * Get the primary key of the input DataPoint associated with the Derived DataPoint
		 *
		 */
		unsigned long getInputDataPointKey() const;


		/**
		 * getInputDataPointName
		 *
		 * Get the name of the input DataPoint associated with the Derived DataPoint
		 *
		 */
		const std::string & getInputDataPointName() const;


		/**
		 * getInputDataPointSubsystemKey
		 *
		 * Get the subsystem key of the input DataPoint associated with the Derived DataPoint
		 *
		 */
		unsigned long getInputDataPointSubsystemKey() const;


		/**
		 * getInputDataPointLocationKey
		 *
		 * Get the location key of the input DataPoint associated with the Derived DataPoint
		 *
		 */
		unsigned long getInputDataPointLocationKey() const;


		/**
		 * getCurrentState
		 *
		 * Get the current state (value, timestamp and quality status) of the input DataPoint
		 * associated with the Derived DataPoint
		 *
		 */
		const DataPointState & getCurrentState() const;


		/**
		 * getValueAsString
		 *
		 * Get the current value, as string, of the input DataPoint
		 * associated with the Derived DataPoint
		 *
		 */
		std::string getValueAsString();


		const std::string & getDescription();


		/**
		 * getInputDataPointDataType
		 *
		 * Get the data type of the value of the input DataPoint associated with the Derived DataPoint
		 *
		 */
		EDataPointDataType getInputDataPointDataType() const;


		/**
		 * getInputCalculationOrder
		 *
		 * Get the calculation order of the value of the input DataPoint associated with the Derived DataPoint
		 *
		 */
		int getInputCalculationOrder() const;


	private:


		/**
		 * resolveCurrentDataPointState
		 *
		 * Update the current state of the input DataPoint with the data presented in
		 * in the corba message
		 *
		 */
		void resolveCurrentDataPointState();


		/**
		 * updateCurrentState
		 *
		 * Update the current state of the input DataPoint with the specified data
		 *
		 */
		void updateCurrentState ( const DpValue & dpv,
								  const timeb & tmstamp,
								  EQualityStatus status );


		DataPointState * m_currentState;
		TA_Base_Core::ConfiguredInputAssociation * m_configuredInputAssociation;
		TA_Base_Bus::DataPointProxySmartPtr m_inputDataPointRef;
		TA_Base_Bus::ScadaProxyFactory *	m_scadaProxyFactory;
		TA_Base_Bus::IEntityUpdateEventProcessor * m_derivedDataPointCallBack;
		TA_Base_Core::ThreadPoolSingletonManager * m_threadPool;
		TA_Base_Core::ReEntrantThreadLockable m_lock;
		volatile bool m_controlMode;
		TA_Base_Core::IWorkItemPtr m_workItem;

		InputAssociation() {};
        InputAssociation ( const InputAssociation & theInstance );
		InputAssociation & operator= ( const InputAssociation & rhs );
	};


    class InputAssociationWorkItem : public TA_Base_Core::IWorkItem
	{

	public:

		InputAssociationWorkItem(InputAssociation& parent);

		virtual ~InputAssociationWorkItem();

		//
		// Operations specific to IWorkItem
		//
		virtual void executeWorkItem();

	protected:

		InputAssociation& m_parent;
	};

}

#endif
