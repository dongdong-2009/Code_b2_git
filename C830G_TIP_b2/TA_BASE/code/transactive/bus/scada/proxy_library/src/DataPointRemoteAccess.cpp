/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File$
  * @author:	Darren Sampson
  * @version:	$Revision$
  *
  * Last modification:	$DateTime$
  * Last modified by:	$Author$
  *
  * Description:
  *
  *	DataPoint CORBA interface, used to work with DataPoints
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include "bus/scada/proxy_library/src/DataPointRemoteAccess.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	DataPointRemoteAccess::DataPointRemoteAccess(TA_Base_Core::DataPointEntityDataPtr dpEntityData)
		: m_entityData(dpEntityData),
        m_dpAccessInterface(NULL)
	{
        std::string interfaceName(DP_ACCESS_INTERFACE);
        
        m_dpAccessInterface = new DpAccessInterface();
        m_dpAccessInterface->setCorbaName(m_entityData->getAgentName(), m_entityData->getAgentName() + interfaceName);
        //set timeout to 5 seconds.
        static const unsigned long DATAPOINT_AGENT_CORABA_TIMEOUT = 5;
        m_dpAccessInterface->setObjectTimeout(DATAPOINT_AGENT_CORABA_TIMEOUT);
	};

	DataPointRemoteAccess::~DataPointRemoteAccess()
	{
        delete m_dpAccessInterface;
        m_dpAccessInterface = NULL;
	};


	/**
	 *
	 * GetFieldValue
	 *
	 * Gets the  field value of the DataPoint
	 *
	 */
	boost::shared_ptr<TA_Base_Bus::DataPointValue> DataPointRemoteAccess::getFieldValue()
	{
        TA_Base_Bus::ScadaCorbaTypes::UTag* corbaDpValue;
        CORBA_CALL_RETURN( corbaDpValue,
                           (*m_dpAccessInterface),
                           getFieldValue,
                           ( m_entityData->getKey() ) );
        DataPointValue* dpValue = new DataPointValue();
        convertDataPointValue(*corbaDpValue, *dpValue);
        
		return boost::shared_ptr<TA_Base_Bus::DataPointValue>(dpValue);
	};


	/*submitWriteRequest
	*new function used to send control to void doubled control
	*@param 	cancelTime	the request shoud be send before cancelTime
	*@param		controlUUID	the uuid used to identify the transaction used to void double control
	*/
	void DataPointRemoteAccess::submitWriteRequest(const std::string& sessionId, 
																		TA_Base_Bus::DataPointValue& value, 
																		unsigned long triggeringEntityKey, 
																		const char * triggeringEntityValue,
																		const bool isLogingEvent,          
																		timeb cancelTime,
																		std::string controlUUID)
	{
		TA_Base_Bus::ScadaCorbaTypes::UTag corbaDpValue;
		convertDataPointValue(value, corbaDpValue);

		TA_Base_Bus::ScadaCorbaTypes::STimeStamp timeStamp;
		converTimeStamp(cancelTime, timeStamp);

		std::string triggeringValue("");
		if (triggeringEntityValue)
		{
			triggeringValue = triggeringEntityValue;
		}
	    CORBA_CALL( (*m_dpAccessInterface),
	                   submitWriteRequest,
	                   ( m_entityData->getKey(), sessionId.c_str(), corbaDpValue, triggeringEntityKey, triggeringValue.c_str(), 1, timeStamp, controlUUID.c_str()) );
	}

	
	/**
	 *
	 * SetIsForcedState
	 *
	 * Set the data point manual state to the specified indication.
	 * Throw AccessDeniedException exception if the client has no authorisation to do so.
	 *
	 * @param    sessionId	The session ID of the client issueing the command
	 * @param    indication	True to enable data point's manual override state,
	 *						false otherwise.
	 *
	*/
	void DataPointRemoteAccess::setIsForcedState ( const std::string&  sessionId, bool indication )
	{
        TA_ASSERT(indication == false, "To enalbe override, should call setForcedStateWithValue");
        CORBA_CALL( (*m_dpAccessInterface),
                    removeOverride,
                    ( m_entityData->getKey(), sessionId.c_str() ) );
	}


	/**
	 *
	 * setForcedStateWithValue
	 *
	 * Force the data point to manual state and update data point's value with the
	 * specified manually entered value.  Throw the following exceptions:
	 *
	 *		OperationModeException, if the data point is in monitor mode,
	 *		AccessDeniedException, if the client has no authorisation to do so,
	 *		NotWriteAble, if the specified value cannot be updated
	 *		BadParameterException, if the specified value is out of range.
	 *
	 * @param    sessionId	The session ID of the client issueing the command
	 * @param    value		The manually entered value to be written to the data point
	 *
	*/
	void DataPointRemoteAccess::setForcedStateWithValue ( const std::string&  sessionId, const TA_Base_Bus::DataPointValue & value )
	{
        TA_Base_Bus::ScadaCorbaTypes::UTag corbaDpValue;
        convertDataPointValue(value, corbaDpValue);
        CORBA_CALL( (*m_dpAccessInterface),
                     setOverride,
                     ( m_entityData->getKey(), sessionId.c_str(), corbaDpValue ) );
	}


	/**
	  * setAlarmProperties
	  *
	  * This method changes the alarm properties for the datapoint.
	  * Throw the following exceptions:
	  *
	  *		OperationModeException, if the data point is in monitor mode,
	  * 	BadParameterException, if the specified value is out of range.
	  *
	  * @param	newAlarmProperty	The new alarm property of the data point
	  * @param	sessionId			The operator's session id
	  *
	  */
	void DataPointRemoteAccess::setAlarmProperty ( const std::string&  sessionId, const TA_Base_Bus::AlarmProperty& newAlarmProperty )
	{
        TA_Base_Bus::DataPointCorbaTypes::SAlarmProperty corbaAlarmProperty;
        convertAlarmProperty(newAlarmProperty, corbaAlarmProperty);

		CORBA_CALL( (*m_dpAccessInterface),
		            setAlarmProperty,
		            ( m_entityData->getKey(), sessionId.c_str(), corbaAlarmProperty ) );
	}


		
	/**
	 *
	 * SetServiceState
	 *
	 * Change the data point's service state.  Throw AccessDeniedException exception
	 * if the client has no authorisation to do so.
	 *
	 * @param    sessionId	The session ID of the client issueing the command
	 * @param    indication	True to put the data point in service, false
	 *						to take the data point out of service
	 *
	 */
	void DataPointRemoteAccess::setInputInhibitState ( const std::string&  sessionId, EInputInhibitState inputInhibitState )
	{
        TA_Base_Bus::ScadaCorbaTypes::EInputInhibitState corbaInputInhibitState;
        convertInputInhibitState(inputInhibitState, corbaInputInhibitState);
		CORBA_CALL( (*m_dpAccessInterface),
		            setInputInhibitState,
		            ( m_entityData->getKey(), sessionId.c_str(), corbaInputInhibitState ) );
	}


	/**
	 *
	 * SetIsControlInhibited
	 *
	 * Inhibit or un-inhibit the data point's control.  Throw AccessDeniedException exception
	 * if the client has no authorisation to do so.
	 *
	 * @param    sessionId	The session ID of the client issueing the command
	 * @param    indication	True to inhibit the data point's control, false
	 *						to un-inhibit the control
	 *
	 */
	void DataPointRemoteAccess::setOutputInhibitState ( const std::string&  sessionId, EOutputInhibitState outputInhibitState )
	{
        TA_Base_Bus::ScadaCorbaTypes::EOutputInhibitState corbaOutputInhibitState;
        convertOutputInhibitState(outputInhibitState, corbaOutputInhibitState);
        CORBA_CALL( (*m_dpAccessInterface),
                    setOutputInhibitState,
                    ( m_entityData->getKey(), sessionId.c_str(), corbaOutputInhibitState ) );
    }


	/**
	 *
	 * SetMeteredCorrectionValue
	 *
	 * Set the correction value of the Metered DataPoint.
	 *
	 * @param    newValue	The new correction value
	 *
	 */
	void DataPointRemoteAccess::setMeteredCorrectionValue ( const std::string& sessionId, double newValue )
	{
		CORBA_CALL( (*m_dpAccessInterface),
		            setMeterCorrectionValue,
		            ( m_entityData->getKey(), sessionId.c_str(), newValue ) );
	}




};
