/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/DataPointEventListener.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#pragma warning ( disable:4786 )

#include "stdafx.h"

#include "DataPointEventListener.h"
#include "CorbaUtil.h"
#include "CommonDmIf.h"
#include "DataPointDmIf.h"
#include "CallbackSubmission.h"
#include "ReceivedEvent.h"
#include "ManagedDataPointStateList.h"
#include "OPCTag.h"
#include "INexusCommon.h"
#include "NexusCommonFactory.h"
#include "DebugUtil.h"

#include "comdef.h"
#include "opcda.h"
#include "OpcError.h"
#include "opc_ae.h"
#include "WTOPCsvrAPI.h"


const std::string EVENT_CHANNEL ( CorbaUtil::EVT_CH_DATAPOINT );
const unsigned int m_delay = 50;

DataPointEventListener::DataPointEventListener()
:
m_callbackSubmissionList ( 0 ),
m_nexusInterface ( 0 ),
m_threadTerminated ( false )
{
	m_nexusInterface = NexusCommonFactory::createNexusCommon();

	m_callbackSubmissionList = new ManagedDataPointStateList();
}

DataPointEventListener::~DataPointEventListener()
{
	if ( m_nexusInterface != 0 )
	{
		NexusCommonFactory::destroyNexusCommon ( m_nexusInterface );
	}

	if ( m_callbackSubmissionList != 0 )
	{
		delete m_callbackSubmissionList;
		m_callbackSubmissionList = 0;
	}

	// remove tags from the WtOPCsvr Tag List
	int blah = 0;
	OPCTag * tag = 0;
	std::map < HANDLE, OPCTag * >::iterator itr;
	for ( itr = m_datapointHandleMap.begin();
		  itr != m_datapointHandleMap.end();
		  itr++ )
	{
		// remove the tag
		RemoveTag ( (*itr).first );

		// then remove the OPCTag associated with the tag
		delete (*itr).second;
	}
	m_datapointHandleMap.clear();
}

bool DataPointEventListener::registerEventChannel ( void )
{
	bool result = false;

	// if the interface to nexus common system is initialised
	if ( m_nexusInterface != 0 )
	{
		try
		{
			// start the thread to run background task
			//setPriority ( 2 ) ;
			//start();

			// get the CORBA event channel and register this TransActiveDataSource together with
			// the specified event channel to the EventListener service for notification of
			// the changes to data points on the specified event channel
			m_nexusInterface->registerForEvents ( EVENT_CHANNEL, this );

			// then set the flag to indicate so
			//m_isEventRegistered = true;
			result = true;
		}
		catch ( ... )
		{
			// do nothing
		}
	}
	else
	{
		// do nothing
	}

	return result;
}

void DataPointEventListener::callback ( const CallbackSubmission * submission )
{
	const ReceivedEvent * theEvent = 0;
	nexus::EventData * data = 0;
	nexus::DataPointDmIf::DataPointEventBundle * datapointStateBundle = 0;
	nexus::DataPointDmIf::DataPointUpdateEvent * datapointUpdateEvent = 0;

	// make sure the callback submission is of the type ReceivedEvent
	theEvent = dynamic_cast< const ReceivedEvent * > ( submission );
	if ( theEvent != 0 )
	{
		try
		{
			// make sure the CORBA::Any of the received event is an EventData object
			if ( * ( theEvent->getData() ) >>= data )
			{
				// make sure the incoming event's data is a DataPointEventBundle object
				if ( data->state >>= datapointStateBundle )
				{
					// step through each of the elements of the DataPointEventBundle object
					for (unsigned int i = 0; i < datapointStateBundle->length(); i++)
					{
						// get the DataPointUpdateEvent
						datapointUpdateEvent = & ( datapointStateBundle->operator[] (i) );

						// create a stub to hold the data point new state
						DataPointStateStub * newState = 0;
						newState = new DataPointStateStub ( datapointUpdateEvent->EntityKey,
															datapointUpdateEvent->State );

						if ( newState != 0 )
						{
							// if valid entity key
							if ( newState->getEntityKey() > 0 )
							{
								// store the new DataPointState for futher processing
								m_callbackSubmissionList->pushBackObject ( newState );

LOG(DebugUtil::DebugDebug, "DataPointEventListener::callback");
char temp[100] = {0};
sprintf ( temp, "m_callbackSubmissionList->size() = %i\n\tentityKey = %i, doubleValue = %i, boolValue = %i", m_callbackSubmissionList->getSize(), newState->getEntityKey(), newState->getValue().doubleValue, newState->getValue().boolValue );
LOGMORE(DebugUtil::DebugDebug, temp );

							}
							else
							{
								// destroy if not a valid entity key
								delete newState;
								newState = 0;
							}
						}

						datapointUpdateEvent = 0;
					}

					datapointStateBundle = 0;
				}
				else
				{
					// do nothing
				}
			}
			else
			{
				// do nothing
			}
		}
		catch ( ... )
		{
			// do nothing
		}
	}
	else
	{
		// do nothing
	}
}

void DataPointEventListener::run()
{
	// keep going forever...unless terminated
	while ( !m_threadTerminated )
	{
		// process callback objects
		purgeCallbackSubmissions();

		// go to sleep for a little while
		MiThread::sleep ( m_delay );
	}
}

void DataPointEventListener::terminate()
{
	// set the flag to indicate the thread is to be terminated.
	m_threadTerminated = true;
}

void DataPointEventListener::purgeCallbackSubmissions()
{
	unsigned int limit = 200;
	if ( m_callbackSubmissionList->getSize() <= limit )
	{
		limit = m_callbackSubmissionList->getSize();
	}

	// process up to "limit" events at a time
	for ( int i = 0; i < limit; i++ )
	{
		// get a DataPointStateStub object from the beginning of the list
		DataPointStateStub * obj = 0;
		obj = m_callbackSubmissionList->popFrontObject();

		// if valid object
		if ( obj != 0 )
		{
			// update the OPCTag with the new state
			updateWtOPCTag ( * obj );

			// destroy the object once it is finished with
			delete obj;
			obj = 0;

//LOG(DebugUtil::DebugDebug, "DataPointEventListener::purgeCallbackSubmissions");
//char temp[100] = {0};
//sprintf ( temp, "m_callbackSubmissionList->size() = %i", m_callbackSubmissionList->getSize() );
//LOGMORE(DebugUtil::DebugDebug, temp );

		}
		else
		{
			// do nothing
		}
	}
}

void DataPointEventListener::createWtOPCTag ( const std::string & attributeDataType,
											  const std::string & dataPointName,
											  const std::string & dataPointCorbaName,
											  unsigned long entityKey )
{
	OPCTag * tag = 0;
	tag = new OPCTag ( attributeDataType, 
					   dataPointName,
					   dataPointCorbaName,
					   entityKey );

	// create a tag in WTOPCsvr and then insert both the handle
	// and the tag into the internal list for further references
	HANDLE handle = CreateTag ( tag->getDataPointName().c_str(),
								tag->getValue(),
								tag->getQuality(),
								TRUE );

	// suspend the updating of the tag until it is subscribed by the client
	//SuspendTagUpdates ( handle, TRUE );

	// insert both the tag and its handle to the list for further
	// references
	m_datapointHandleMap [ handle ] = tag;
	tag = 0;
}
/*
void DataPointEventListener::updateWtOPCTag ( const DataPointStateStub & newState )
{
	// look for the OPCTag whose entity key matches with the one of the newState
	// passed in via parameter list
	OPCTag * tag = 0;
	std::map < HANDLE, OPCTag * >::iterator itr;
	for ( itr = m_datapointHandleMap.begin();
		  itr != m_datapointHandleMap.end();
		  itr++ )
	{
		tag = (*itr).second;

		// found the one that matches
		//if ( tag->getEntityKey() == newState.getEntityKey() )
		if ( tag->getDataPointName() == "PLC.DataPoint.CLOCK_HRS" )
		{
			// get the new value
			VARIANT newValue;
			VariantInit ( & newValue );
			srand( (unsigned)time( NULL ) );

			// find the data type of tag
			switch ( tag->getAttributeDataType() )
			{
				case ATTRIBUTE_DATA_TYPE_BYTE :
				{
					newValue.vt = VT_UI1;
					//newValue.bVal = (char) newState.getValue().doubleValue;
					newValue.bVal = (char) rand();
					break;
				}
				case ATTRIBUTE_DATA_TYPE_UNSIGNED_BYTE :
				{
					newValue.vt = VT_UI1;
					//newValue.bVal = (unsigned char) newState.getValue().doubleValue;
					newValue.bVal = (unsigned char) rand();
					break;
				}
				case ATTRIBUTE_DATA_TYPE_SHORT :
				{
					newValue.vt = VT_I2;
					//newValue.iVal = (short) newState.getValue().doubleValue;
					newValue.iVal = (short) rand();
					break;
				}
				case ATTRIBUTE_DATA_TYPE_UNSIGNED_SHORT :
				{
					newValue.vt = VT_I2;
					//newValue.iVal = (unsigned short) newState.getValue().doubleValue;
					newValue.iVal = (unsigned short) rand();
					break;
				}
				case ATTRIBUTE_DATA_TYPE_LONG :
				{
					newValue.vt = VT_I4;
					//newValue.lVal = (long) newState.getValue().doubleValue;
					newValue.lVal = (long) rand();
					break;
				}
				case ATTRIBUTE_DATA_TYPE_UNSIGNED_LONG :
				{
					newValue.vt = VT_I2;
					//newValue.lVal = (unsigned long) newState.getValue().doubleValue;
					newValue.lVal = (unsigned long) rand();
					break;
				}
				case ATTRIBUTE_DATA_TYPE_FLOAT :
				{
					newValue.vt = VT_R4;
					//newValue.fltVal = (float) newState.getValue().doubleValue;
					newValue.fltVal = (float) rand();
					break;
				}
				case ATTRIBUTE_DATA_TYPE_DOUBLE :
				{
					newValue.vt = VT_R8;
					//newValue.dblVal = newState.getValue().doubleValue;
					newValue.dblVal = rand();
					break;
				}
				case ATTRIBUTE_DATA_TYPE_TEXT:
				{
					newValue.vt = VT_BSTR;
					_bstr_t bstr_string ( newState.getValue().getStringValue() );
					newValue.bstrVal = bstr_string;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_BOOLEAN:
				{
					newValue.vt = VT_BOOL;
					newValue.boolVal = newState.getValue().boolValue;
					break;
				}
				default:
				{
					newValue.vt = VT_R8;
					//newValue.dblVal = newState.getValue().doubleValue;
					newValue.dblVal = rand();
					break;
				}
			}


			// get the new quality --> hard coded for now !!!
			WORD newQuality = OPC_QUALITY_GOOD;

			// update tag
			UpdateTag ( (*itr).first, newValue, newQuality );

			// clean up the VARIANT variable
			if ( newValue.vt == VT_BSTR )
			{
				_bstr_t bstr_string ( "" );
				newValue.bstrVal = bstr_string;
			}

			VariantClear ( & newValue );

		}
	}
}
*/
void DataPointEventListener::updateWtOPCTag ( const DataPointStateStub & newState )
{
	// look for the OPCTag whose entity key matches with the one of the newState
	// passed in via parameter list
	OPCTag * tag = 0;
	std::map < HANDLE, OPCTag * >::iterator itr;
	for ( itr = m_datapointHandleMap.begin();
		  itr != m_datapointHandleMap.end();
		  itr++ )
	{
		tag = (*itr).second;

		// found the one that matches
		if ( tag->getEntityKey() == newState.getEntityKey() )
		{
			// get the new value
			VARIANT newValue;
			VariantInit ( & newValue );

			// find the data type of tag
			switch ( tag->getAttributeDataType() )
			{
				case ATTRIBUTE_DATA_TYPE_BYTE :
				{
					newValue.vt = VT_UI1;
					newValue.bVal = (char) newState.getValue().doubleValue;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_UNSIGNED_BYTE :
				{
					newValue.vt = VT_UI1;
					newValue.bVal = (unsigned char) newState.getValue().doubleValue;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_SHORT :
				{
					newValue.vt = VT_I2;
					newValue.iVal = (short) newState.getValue().doubleValue;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_UNSIGNED_SHORT :
				{
					newValue.vt = VT_I2;
					newValue.iVal = (unsigned short) newState.getValue().doubleValue;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_LONG :
				{
					newValue.vt = VT_I4;
					newValue.lVal = (long) newState.getValue().doubleValue;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_UNSIGNED_LONG :
				{
					newValue.vt = VT_I2;
					newValue.lVal = (unsigned long) newState.getValue().doubleValue;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_FLOAT :
				{
					newValue.vt = VT_R4;
					newValue.fltVal = (float) newState.getValue().doubleValue;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_DOUBLE :
				{
					newValue.vt = VT_R8;
					newValue.dblVal = newState.getValue().doubleValue;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_TEXT:
				{
					newValue.vt = VT_BSTR;
					//_bstr_t bstr_string ( newState.getValue().getStringValue() );
					_bstr_t bstr_string ( "" );
					newValue.bstrVal = bstr_string;
					break;
				}
				case ATTRIBUTE_DATA_TYPE_BOOLEAN:
				{
					newValue.vt = VT_BOOL;
					newValue.boolVal = newState.getValue().boolValue;
					break;
				}
				default:
				{
					newValue.vt = VT_R8;
					newValue.dblVal = newState.getValue().doubleValue;
					break;
				}
			}


			// get the new quality --> hard coded for now !!!
			WORD newQuality = OPC_QUALITY_GOOD;

			// update tag
			UpdateTag ( (*itr).first, newValue, newQuality );

			// clean up the VARIANT variable
			if ( newValue.vt == VT_BSTR )
			{
				_bstr_t bstr_string ( "" );
				newValue.bstrVal = bstr_string;
			}

			VariantClear ( & newValue );

		}
	}
}

void DataPointEventListener::writeNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError )
{
	OPCTag * tag = 0;
	std::map < HANDLE, OPCTag * >::iterator itr = 0;

	itr = m_datapointHandleMap.find ( Handle );

	// if found a handle that matches with the one passed in via parameter list
	if ( itr != m_datapointHandleMap.end() )
	{
		// get the OPCTag associated with the matched handle
		tag = (*itr).second;

		// pass the OPCTag and the new value to CustomDataSource to
		// do the job

		//
	}
	else
	{
	}
}

void DataPointEventListener::ReadNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp )
{
	OPCTag * tag = 0;
	SYSTEMTIME	systime;
	std::map < HANDLE, OPCTag * >::iterator itr = 0;
	
	itr = m_datapointHandleMap.find ( Handle );

	// if found a handle that matches with the one passed in via parameter list
	if ( itr != m_datapointHandleMap.end() )
	{
		// get the OPCTag associated with the matched handle
		tag = (*itr).second;

		// pass the value, quality and timestamp back to the user
		VariantCopy  ( pNewValue, & ( tag->getValue() ) );
		*pQuality = OPC_QUALITY_GOOD;
		GetSystemTime (& systime);
		SystemTimeToFileTime ( & systime, pTimestamp );
	}
	else
	{
	}
}

void DataPointEventListener::UnknownTagNotificationHandler ( LPSTR path, LPSTR name )
{
	// look for the OPCTag whose logical name matches with the name
	// passed in via parameter list
	OPCTag * tag = 0;
	std::string specifiedName  ( name );
	std::map < HANDLE, OPCTag * >::iterator itr;
	for ( itr = m_datapointHandleMap.begin();
		  itr != m_datapointHandleMap.end();
		  itr++ )
	{
		tag = (*itr).second;

		if ( specifiedName == tag->getDataPointName() )
		{
			// start the updating of the tag
			BOOL result = SuspendTagUpdates ( (*itr).first, FALSE );
			int blah = 1;

			// found the data point, exit loop
			break;
		}
		else
		{
			// do nothing
		}
	}
}

void DataPointEventListener::RemoveTagNotificationHandler ( HANDLE handle, LPSTR path, LPSTR name )
{
	// suspend the updating of the tag until it is subscribed by the client
	BOOL result = SuspendTagUpdates ( handle, TRUE );
	int blah = 1;
}
