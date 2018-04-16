/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/TransActiveDataSource.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#pragma warning ( disable:4786 )

#include "stdafx.h"

#include "TransActiveOPCServer.h"
#include "TransActiveOPCServerDlg.h"

#include "TransActiveDataSource.h"
#include "INexusCommon.h"
#include "CallbackSubmission.h"
#include "DebugUtil.h"
#include "NexusCommonFactory.h"
#include "ReceivedEvent.h"
//#include "ManagedDataPointStateList.h"
#include "OPCTag.h"
#include "DataPointEventListener.h"

//#include "comdef.h"
#include "opcda.h"
#include "OpcError.h"
#include "opc_ae.h"
#include "WTOPCsvrAPI.h"


//------------------------------------------------------------------------------------------


const std::string EVENT_CHANNEL ( CorbaUtil::EVT_CH_DATAPOINT );

const char * ENTITY_TYPE = "DataPoint";

// {6764A030-070C-11d3-80D6-0060975858BE}
static const GUID CLSID_Svr =
{ 0x6764a030, 0x70c, 0x11d3, { 0x80, 0xd6, 0x0, 0x60, 0x97, 0x58, 0x58, 0xbe } };

const std::string SVR_NAME = "TransActiveDataSource";

const std::string SVR_DESC = "TransActive OPC DataSource";

const std::string SVR_EXE_PATH = "";
//const std::string SVR_EXE_PATH = "C:\\projects\\Manila\\ITS\\Manila\\P-PROD\\P03-TRANSACTIVE_DATASOURCE\\TransActiveOPCServer\\bin\\Debug\\TransActiveDataSource.exe";
//const std::string SVR_EXE_PATH = "C:\\projects\\Manila\\ITS\\Manila\\P-PROD\\P03-TRANSACTIVE_DATASOURCE\\TransActiveOPCServer\\bin\\Release\\TransActiveDataSource.exe";

const UINT SVR_RATE = 50; // in msecs

void CALLBACK EXPORT WriteNotifyProcHV ( HANDLE Handle, VARIANT *pNewValue, DWORD *pDevError )
{
	CTransActiveOPCServerApp * pApp = 0;

	pApp = ( CTransActiveOPCServerApp * ) AfxGetApp();

	if ( pApp != 0 )
	{
		pApp->m_dlg->writeNotificationHandler ( Handle, pNewValue, pDevError );
	}
	else
	{
		// do nothing
	}
}

void CALLBACK EXPORT DeviceReadProcHV ( HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp )
{
	CTransActiveOPCServerApp * pApp = 0;

	pApp = ( CTransActiveOPCServerApp * ) AfxGetApp();

	if ( pApp != 0 )
	{
		pApp->m_dlg->ReadNotificationHandler ( Handle, pNewValue, pQuality, pTimestamp );
	}
	else
	{
		// do nothing
	}
}

void CALLBACK EXPORT UnknownTagProcHV ( LPSTR path, LPSTR name )
{
	CTransActiveOPCServerApp * pApp = 0;

	pApp = ( CTransActiveOPCServerApp * ) AfxGetApp();

	if ( pApp != 0 )
	{
		pApp->m_dlg->UnknownTagNotificationHandler ( path, name );
	}
	else
	{
		// do nothing
	}
}

void CALLBACK EXPORT RemoveTagProcHV ( HANDLE handle, LPSTR path, LPSTR name )
{
	CTransActiveOPCServerApp * pApp = 0;

	pApp = ( CTransActiveOPCServerApp * ) AfxGetApp();

	if ( pApp != 0 )
	{
		pApp->m_dlg->RemoveTagNotificationHandler ( handle, path, name );
	}
	else
	{
		// do nothing
	}
}


//------------------------------------------------------------------------------------------


TransActiveDataSource::TransActiveDataSource()
:
m_dataPointEventListener ( 0 ),
//m_isEventRegistered ( false ),
//m_threadTerminated ( false ),
m_nexusInterface ( 0 )
//m_callbackSubmissionList ( 0 ),
//m_delay ( 50 )		// default polling delay to 50 msecs
{
	// create a default instance of the nexus common interface
	m_nexusInterface = NexusCommonFactory::createNexusCommon();

	if ( m_nexusInterface != 0 )
	{
		// Activate CORBA
		CorbaUtil::GetInstance().Activate();

		// intialise PollingDelay from the database
		//initialisePollingDelay();

		//m_callbackSubmissionList = new ManagedDataPointStateList();
	}
	else
	{
		throw std::bad_alloc();
	}
}

TransActiveDataSource::TransActiveDataSource ( const RunParams & runTimeParamters)
:
//m_isEventRegistered ( false ),
//m_threadTerminated ( false ),
m_nexusInterface ( 0 )
//m_callbackSubmissionList ( 0 ),
//m_delay ( 50 )		// default polling delay to 50 msecs
{
	// create an instance of the nexus common interface based on the runtime
	// parameters passed in via the input list
	m_nexusInterface = NexusCommonFactory::createNexusCommon ( runTimeParamters );

	if ( m_nexusInterface != 0 )
	{
		// Activate CORBA
		CorbaUtil::GetInstance().Activate();

		// intialise PollingDelay from the database
		//initialisePollingDelay();

		//m_callbackSubmissionList = new ManagedDataPointStateList();
	}
	else
	{
		throw std::bad_alloc();
	}
}

TransActiveDataSource::~TransActiveDataSource()
{
	/*
	// if this object is registered with the CORBA Event Service
	if ( m_isEventRegistered )
	{
		// stop the thread that runs the background task
		terminateAndWait();
	}
	*/

	// clean up the list of callback submissions
	//delete m_callbackSubmissionList;
	//m_callbackSubmissionList = 0;

	// clean up the nexus common interface
	if ( m_nexusInterface != 0 )
	{
		NexusCommonFactory::destroyNexusCommon ( m_nexusInterface );
		m_nexusInterface = 0;
	}
}

bool TransActiveDataSource::startTransActiveDataSource ( void )
{
	// initialise DCOM and OPC Server stuffs
	initialiseWtOPCsvr();

	if ( m_dataPointEventListener == 0 )
	{
		m_dataPointEventListener = new DataPointEventListener();
	}

	// create OPCTag for client application
	createWtOPCTag();

	m_dataPointEventListener->registerEventChannel();
	m_dataPointEventListener->start();

	// register and start listening to CORBA event
	//registerEventChannel();

	return true;
}

void TransActiveDataSource::stopTransActiveDataSource ( void )
{
	// check if any clients connected to the TransActiveOPCServer
	if ( NumbrClientConnections() > 0 )
	{
		// if there is, then request the clients to disconnect
		RequestDisconnect();
		Sleep ( 10000 ); // 10 secs
	}
/*
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
*/

	if ( m_dataPointEventListener != 0 )
	{
		m_dataPointEventListener->terminateAndWait();

		delete m_dataPointEventListener;
		m_dataPointEventListener = 0;
	}

	// remove the TransActiveOPCServer entry from Windows Registry
	if ( UnregisterServer ( (BYTE *) & CLSID_Svr, SVR_NAME.c_str() ) == FALSE )
	{
		// do nothing
	}

	// uninitializes COM/DCOM stuffs
	if ( UninitWTOPCsvr() == FALSE )
	{
		// do nothing
	}
}

void TransActiveDataSource::setPollingDelay ( unsigned int delay )
{
	m_delay = delay;
}

void TransActiveDataSource::performFailOver ( void )
{
	try
	{
		if ( m_nexusInterface != 0 )
		{
			m_nexusInterface->performFailOver();
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
/*
void TransActiveDataSource::callback ( const CallbackSubmission * submission )
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

void TransActiveDataSource::run()
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

void TransActiveDataSource::terminate()
{
	// set the flag to indicate the thread is to be terminated.
	m_threadTerminated = true;
}
*/
void TransActiveDataSource::initialiseWtOPCsvr ( void )
{
	try
	{
		// update Windows Registry with an instance of the TransActiveOPCServer
		// to allow other applications to find and execute the server.
		if ( UpdateRegistry ( ( BYTE * ) & CLSID_Svr,
							  SVR_NAME.c_str(),
							  SVR_DESC.c_str(),
							  SVR_EXE_PATH.c_str() ) == TRUE )
		{
			// initialise COM/DCOM, Security, and creates the Server ClassFactory
			if ( InitWTOPCsvr ( (BYTE *) & CLSID_Svr, SVR_RATE ) == TRUE )
			{
				// callback indicating that an OPC Client has requested
				// to write a value to a tag
				EnableWriteNotification ( & WriteNotifyProcHV, TRUE );

				// callback indicating that an OPC Client has requested
				// a read from device
				EnableDeviceRead ( & DeviceReadProcHV );

				// callback indicating client application is subscribing a tag
				//EnableUnknownItemNotification ( & UnknownTagProcHV );

				// callback indicating the tag is no longer subscribed by
				// any client
				//EnableItemRemovalNotification ( & RemoveTagProcHV );

				SetVendorInfo ( "TransActive OPC Server" );
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

void TransActiveDataSource::createWtOPCTag ( void )
{
	// construct query string for datapoints' entities
	std::string sql ( "SELECT s.name, t.name, e.name, e.pkey, pv.value " );
	sql += "FROM ENTITY e, ENTITYTYPE t, SUBSYSTEM s, ENTITYPARAMETER p, ENTITYPARAMETERVALUE pv ";
	sql += "WHERE t.name = 'DataPoint' ";
	sql += "AND p.name = 'DataPointType' ";
	sql += "AND t.pkey = e.typekey ";
	sql += "AND s.pkey = e.subsystemkey ";
	sql += "AND pv.entitykey = e.pkey ";
	sql += "AND pv.parameterkey = p.pkey ";
	sql += "ORDER BY e.name";
	SimpleDb::Buffer resultset;

	// request the nexus common interface for a list of datapoints' logical names and keys
	if ( m_nexusInterface->execQueryStatement ( sql.c_str(), 5, resultset ) )
	{
		bool hasMoreData;
		//OPCTag * tag = 0;
		do
		{
			hasMoreData = false;

			// for each rows of the result set
			for ( SimpleDb::Buffer::iterator itr = resultset.begin();
				  itr != resultset.end();
				  itr++ )
			{
				// get the info from the result set
				std::string subSystem = (*itr)[0];
				std::string entityType = (*itr)[1];
				std::string logicalName = (*itr)[2];
				unsigned long entityKey = atoi ( (*itr)[3].c_str() );
				std::string dataType = (*itr)[4];

				// make up the corba name of the data point
				std::string corbaName ( "" );
				corbaName += "/";
				corbaName += subSystem;
				corbaName += "/";
				corbaName += entityType;
				corbaName += "/";
				corbaName += logicalName;

				// create an OPCTag associated with the data point
				std::string OPCLogicalName = subSystem;
				OPCLogicalName += ".";
				OPCLogicalName += entityType;
				OPCLogicalName += ".";
				OPCLogicalName += logicalName;				

				/*
				tag = new OPCTag ( dataType, OPCLogicalName, corbaName, entityKey );

				// create a tag in WTOPCsvr and then insert both the handle
				// and the tag into the internal list for further references
				HANDLE handle = CreateTag ( tag->getDataPointName().c_str(),
								tag->getValue(),
								tag->getQuality(),
								TRUE );

				// insert both the tag and its handle to the list for further
				// references
				m_datapointHandleMap [ handle ] = tag;
				tag = 0;
				*/

				m_dataPointEventListener->createWtOPCTag ( dataType, OPCLogicalName, corbaName, entityKey );
			}

			if ( m_nexusInterface->hasMore() )
			{
				// set the flag
				hasMoreData = true;

				// clear the buffer
				resultset.empty();

				// get more data from the database
				m_nexusInterface->fetchMore ( 5, resultset );
			}
		}
		while ( hasMoreData );
	}
	else
	{
		// do nothing
	}
}
/*
void TransActiveDataSource::updateWtOPCTag ( const DataPointStateStub & newState )
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
					newValue.dblVal = newState.getValue().doubleValue;
					break;
				}
			}


			// get the new quality --> hard coded for now !!!
			WORD newQuality = OPC_QUALITY_GOOD;

			// update tag
			UpdateTagByName ( tag->getDataPointName().c_str(),
							  newValue,
							  newQuality );

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
/*
std::string TransActiveDataSource::getEventChannel ( void )
{
	return EVENT_CHANNEL;
}

bool TransActiveDataSource::registerEventChannel ( void )
{
	bool result = false;

	// if the interface to nexus common system is initialised
	if ( m_nexusInterface != 0 )
	{
		try
		{
			// start the thread to run background task
			setPriority ( 2 ) ;
			start();

			// get the CORBA event channel and register this TransActiveDataSource together with
			// the specified event channel to the EventListener service for notification of
			// the changes to data points on the specified event channel
			m_nexusInterface->registerForEvents ( getEventChannel(), this );

			// then set the flag to indicate so
			m_isEventRegistered = true;
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
*/
/*
void TransActiveDataSource::purgeCallbackSubmissions()
{
	unsigned int limit = 100;
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
		}
		else
		{
			// do nothing
		}
	}
}
*/
/*
void TransActiveDataSource::initialisePollingDelay()
{
	// construct query string to get the parameters from the database
	std::string sql ( "" );
	sql += "SELECT ep.name, epv.value ";
	sql += "FROM ENTITY e, ENTITYPARAMETER ep, ENTITYPARAMETERVALUE epv ";
	sql += "WHERE e.name = 'ScadaViewer' ";
	sql += "AND epv.entitykey = e.pkey ";
	sql += "AND ep.pkey = epv.parameterkey ";
	sql += "AND ( ep.name = 'PollingDelay' OR ";
	sql += "ep.name = 'IdleTimeout' )";

	SimpleDb::Buffer resultset;
	m_nexusInterface->execQueryStatement ( sql.c_str(), 2, resultset );

	// for each of the parameters returned from the database
	for ( SimpleDb::Buffer::iterator itr = resultset.begin();
		  itr != resultset.end();
		  itr++ )
	{
		// get the parameter name
		std::string name = (*itr)[0];

		// get the parameter value
		std::string value = (*itr)[1];

		// insert new parameter into the runtime parameter list
		RunParams::GetInstance().Set ( name.c_str(), value.c_str() );

		if ( name == "PollingDelay" )
		{
			m_delay = atoi ( value.c_str() );
		}
	}
}
*/

void TransActiveDataSource::writeNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError )
{
	m_dataPointEventListener->writeNotificationHandler ( Handle, pNewValue, pDeviceError );
}

void TransActiveDataSource::ReadNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp )
{
	m_dataPointEventListener->ReadNotificationHandler ( Handle, pNewValue, pQuality, pTimestamp );
}

void TransActiveDataSource::UnknownTagNotificationHandler ( LPSTR path, LPSTR name )
{
	m_dataPointEventListener->UnknownTagNotificationHandler ( path, name );
}

void TransActiveDataSource::RemoveTagNotificationHandler ( HANDLE handle, LPSTR path, LPSTR name )
{
	m_dataPointEventListener->RemoveTagNotificationHandler ( handle, path, name );
}
