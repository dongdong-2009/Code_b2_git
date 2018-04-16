/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:		$File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/datapoint_library/src/CommonContext.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2008/11/28 16:26:01 $
  * Last modified by:	$Author: builder $
  *
  * Description:
  *
  *	Base class for BooleanContext and RealNumberContext...
  *
  */

#include <sstream>

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/derived_datapoints/src/ConfiguredInputAssociation.h"
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "bus/scada/datapoint_library/src/IExpressionNodeNotification.h"
#include "bus/scada/datapoint_library/src/InputAssociation.h"
#include "bus/scada/datapoint_library/src/DataPointState.h"
#include "bus/scada/datapoint_library/src/CommonContext.h"
#include "bus/scada/datapoint_library/src/EventDetailsFactory.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	CommonContext::CommonContext()
	    :
	    m_userOfExpression ( 0 ),
	    m_toCallback ( false ),
        m_eventDetailsFactory (NULL)
	{
		// initialise the list of input dependencies
		m_listOfInputDependencies.clear();
	}


	CommonContext::CommonContext ( IExpressionNodeNotification * userOfExpression )
	    :
	    m_userOfExpression ( 0 ),
        m_toCallback ( false ),
        m_eventDetailsFactory (NULL)
	{
		// initialise the list of input dependencies
		m_listOfInputDependencies.clear();

		// save pointer pointing to the user for callback later
		m_userOfExpression = userOfExpression;
	}


	CommonContext::~CommonContext()
	{
		// don't delete the object since it is not created/owned by us
		m_userOfExpression = 0;

        // clean up the list of input dependencies
		std::vector < InputAssociation * >::iterator itr;
		for ( itr = m_listOfInputDependencies.begin();
			  itr != m_listOfInputDependencies.end();
			  itr++ )
		{
			delete (*itr);
		}
		m_listOfInputDependencies.clear();
        
        if (m_eventDetailsFactory)
        {
            EventDetailsFactory::removeInstance( m_eventDetailsFactory );
            m_eventDetailsFactory = NULL;
        }
	}


	double CommonContext::getDoubleValue ( const std::string & variableName )
	{
		double doubleValue = -99999999;	// default value if the quality of the variable is not good

		// find an input dependency whose name matches with the specified variable name
		InputAssociation * ipAssocation = 0;
		ipAssocation = findInputDependency ( variableName );

		if ( 0 != ipAssocation )
		{
			// check quality status
			EQualityStatus qualityStatus = ipAssocation->getCurrentState().getStatus();

			// only proceed if quality is good
			if ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == qualityStatus ) ||
				 ( QUALITY_GOOD_LOCAL_OVERRIDE == qualityStatus ) )
			{
				switch ( ipAssocation->getInputDataPointDataType() )
				{
					case DPT_NULL_TYPE: // fall through
					case DPT_TEXT_TYPE:
					{
                        //TD16581, should thow exception to indicate this expression is not double
                        std::string msg = variableName + " can't convert to double value";
                        TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
                        break;
					}
					case DPT_BOOL_TYPE:
					{
						doubleValue = (double) ipAssocation->getCurrentState().getValue().getBoolean();
						break;
					}
					// derived, enum, safety output, combined output
					case DPT_ENUM_TYPE:
					{
						doubleValue = (double) ipAssocation->getCurrentState().getValue().getEnum();
						break;
					}

					default:
					{
						doubleValue = ipAssocation->getCurrentState().getValue().getFloat();
						break;
					}
				}

				return doubleValue;
			}
		}

		return doubleValue;
	}


	bool CommonContext::getBooleanValue ( const std::string & variableName )
	{
		bool booleanValue = false;	// default value if the quality of the variable is not good

		// find an input dependency whose name matches with the specified variable name
		InputAssociation * ipAssocation = 0;
		ipAssocation = findInputDependency ( variableName );

		if ( 0 != ipAssocation )
		{
			// check quality status
			EQualityStatus qualityStatus = ipAssocation->getCurrentState().getStatus();

			// only proceed if quality is good
			if ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == qualityStatus ) ||
				 ( QUALITY_GOOD_LOCAL_OVERRIDE == qualityStatus ) )
			{
				switch ( ipAssocation->getInputDataPointDataType() )
				{
					case DPT_BOOL_TYPE:
					{
						booleanValue = ipAssocation->getCurrentState().getValue().getBoolean();
						break;
					}
					default:
					{
                        //TD16581, should thow exception to indicate this expression is not boolean
                        std::string msg = variableName + " can't be converted to boolean value";
                        TA_THROW(TA_Base_Core::MathematicalEvaluationException(msg));
                        
                        break;
					}
				}

				return booleanValue;
			}
		}
		
		return booleanValue;
	}


	InputAssociation * CommonContext::findInputDependency ( const std::string & inputName )
	{
		InputAssociation * ipAssocation = 0;
		bool foundExistingInput = false;

        //TD16581, lock list of input dependencies to prevent multi thread accessing
        TA_Base_Core::ThreadGuard guard( m_lock );
        
        // for each of the input dependency in the list
		std::vector < InputAssociation * >::iterator itr;
		for ( itr = m_listOfInputDependencies.begin();
			  itr != m_listOfInputDependencies.end();
			  itr++ )
		{
			// if found an existing object whose name matches with the
			// specified name
			if ( inputName == (*itr)->getInputDataPointName() )
			{
				// get the handle of the input dependency and exit loop
				ipAssocation = (*itr);
				foundExistingInput = true;
				break;
			}
		}


		// if the specified name is not in the list yet
		if ( false == foundExistingInput )
		{
			// create an instance of the input dependency
			ipAssocation = createInputDependency ( inputName );

			if ( 0 != ipAssocation )
			{
				ipAssocation->setOperationMode ( true );

				// insert the newly created dependency into the list for later usage
				m_listOfInputDependencies.push_back ( ipAssocation );
			}
		}

		return ipAssocation;
	}


	InputAssociation * CommonContext::createInputDependency ( const std::string & inputName )
	{
		InputAssociation * ipAssocation = 0;

        unsigned long   inputDataPointKey = 0;
        std::string     inputDataPointName;
        std::string     inputDataPointAgentName;
        unsigned long   inputDataPointSubsystemKey = 0;
        unsigned long   inputDataPointLocationKey = 0;

		try
		{
            // first try to get the entity from DataNodeAccessFactory cache
            TA_Base_Core::IEntityDataPtr entityData = DataNodeAccessFactory::getInstance()->getEntityData(inputName);
            if (entityData.get() != NULL)
            {
                inputDataPointKey = entityData->getKey();
                inputDataPointName = entityData->getName();
                inputDataPointAgentName = entityData->getAgentName();
                inputDataPointSubsystemKey = entityData->getSubsystem();
                inputDataPointLocationKey = entityData->getLocation();
            }
            else
            {
			    // get configuration data from database
			    std::auto_ptr<IEntityData> inputEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity ( inputName ));
                inputDataPointKey = inputEntityData->getKey();
                inputDataPointName = inputEntityData->getName();
                inputDataPointAgentName = inputEntityData->getAgentName();
                inputDataPointSubsystemKey = inputEntityData->getSubsystem();
                inputDataPointLocationKey = inputEntityData->getLocation();
            }
		}
		catch ( ... )
		{
            std::string exceptionMsg = "Failed to get datapoint: " + inputName + " from database";
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", exceptionMsg);
		}

		// if successfull to get the database entity
		if ( 0 != inputDataPointKey )
		{
			// create an instance of ConfiguredInputAssociation
			TA_Base_Core::ConfiguredInputAssociation * configuredInputAssociation = 0;
			configuredInputAssociation = new TA_Base_Core::ConfiguredInputAssociation ( inputDataPointKey,
																				   inputDataPointName,
																				   inputDataPointAgentName,
																				   inputDataPointSubsystemKey,
																				   inputDataPointLocationKey );

			// create InputAssocation
			ipAssocation = new InputAssociation ( configuredInputAssociation );

			// if successfull
			if ( 0 != ipAssocation )
			{
				// pass the callback object to the InputAssociation
				ipAssocation->registerForStateChangeNotification ( this );
                
                //TD16581
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "InputAssociation created for %s", inputName.c_str());
            }
		}
		else
		{
			std::ostringstream ss;
            ss << "Invalid input entity data (" << inputName << "), no input dependency to be created.";
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, ss.str().c_str());

			m_invalidDependencyErrorString = inputName + " is an invalid input dependency";
		}


		return ipAssocation;
	}


	EQualityStatus CommonContext::getCombinedStatus() const
	{
		EQualityStatus combinedStatus = QUALITY_BAD_NOT_CONNECTED;
		EQualityStatus worstStatus = QUALITY_GOOD_NO_SPECIFIC_REASON;
		bool forcedInputDataPoint = false;

        // for each of the input data point
		std::vector < InputAssociation * >::const_iterator itr;
		for ( itr = m_listOfInputDependencies.begin();
			  itr != m_listOfInputDependencies.end();
			  itr++ )
		{
			// if current status of input data point is less than the
			// worst status, reassign the worst status
			if ( (*itr)->getCurrentState().getStatus() < worstStatus )
			{
				worstStatus = (*itr)->getCurrentState().getStatus();

                //TD16581
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "InputAssociation for %s is in bad quality %d", (*itr)->getInputDataPointName().c_str(), worstStatus);
            }

			// if current status of the input data point is forced
			if ( QUALITY_GOOD_LOCAL_OVERRIDE == (*itr)->getCurrentState().getStatus() )
			{
				// set the flag to indicate so
				forcedInputDataPoint = true;
			}
		}

		// assign the current status of the derived data point to the
		// worst status
		combinedStatus = worstStatus;

		// if all input data points have good status
		if ( QUALITY_GOOD_NO_SPECIFIC_REASON == worstStatus )
		{
			// and if one of the input data point is forced
			if ( true == forcedInputDataPoint )
			{
				// assign the current status of the derived data point to this
				combinedStatus = QUALITY_GOOD_LOCAL_OVERRIDE;
			}
		}

		return combinedStatus;
	}


	std::string CommonContext::getExpressionResultString()
	{
		if (false == m_invalidDependencyErrorString.empty())
		{
			return m_invalidDependencyErrorString;
		}

		std::ostringstream result;

		// for each of the input data point
		std::vector < InputAssociation * >::const_iterator itr;
		for ( itr = m_listOfInputDependencies.begin();
			  itr != m_listOfInputDependencies.end();
			  ++itr )
		{
			if ( m_listOfInputDependencies.begin() != itr )
			{
				result << ", ";
			}

            // get the handle to an instance of the EventDetailsFactory if not already done so
            if ( NULL == m_eventDetailsFactory )
            {
                m_eventDetailsFactory = EventDetailsFactory::getInstance();
            }
            
            const TA_Base_Core::DataPointEventDetails& eventDetails = 
                m_eventDetailsFactory->getEventDetails((*itr)->getInputDataPointKey(), "");

            // TD16211, add asset_name before datapoint description
            result << eventDetails.assetName << " ";
            
            result << (*itr)->getDescription() << " is " << (*itr)->getValueAsString();

            if ((*itr)->getCurrentState().getStatus() < QUALITY_GOOD_NO_SPECIFIC_REASON)
            {
                result << "(BAD QUALITY)";
            }
		}

		return result.str();
	}


	bool CommonContext::isGoodCombinedStatus() const
	{
		bool result = false;
		EQualityStatus combinedStatus = getCombinedStatus();

		// if the combined status is either one of these
		if ( ( QUALITY_GOOD_NO_SPECIFIC_REASON == combinedStatus ) ||
			 ( QUALITY_GOOD_LOCAL_OVERRIDE == combinedStatus ) )
		{
			// then set the flag
			result = true;
		}

		return result;
	}


	void CommonContext::processEntityUpdateEvent ( unsigned long entityKey, ScadaEntityUpdateType updateType )
	{
		// lets assume that the update event is destined for this context,
		// then notify the user of the expression that the value of one of
		// the variable in the expression has changed, without having to check for
		// validity of the specified entity key
		if ( 0 != m_userOfExpression)
		{
			// only proceed if callback is enabled
			if ( true == m_toCallback )
			{
				m_userOfExpression->processExpressionChanges();
			}
		}
		// do nothing if there is no user for callback operation
	}



	void CommonContext::enableCallback ( bool enabled )
	{
		m_toCallback = enabled;
	}

}


