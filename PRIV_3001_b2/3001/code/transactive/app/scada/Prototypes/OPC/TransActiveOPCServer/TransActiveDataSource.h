/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/TransActiveDataSource.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
//---------------------------------------------------------------------------------------------------
// Class: TransActiveDataSource
//
// Object of the class TransActiveDataSource implements public methods of the
// ITransActiveDataSource, MiThread and CallbackUser interfaces.  The class manages two
// lists of objects: one for storing callback events and one for storing objects representing
// the actual data points of the special data source.
//
// Since TransActiveDataSource has to access various components of TransActive software
// system, it uses the NexusCommonFactory to create an instance of the INexusCommon
// interface.
//
//---------------------------------------------------------------------------------------------------

#ifndef TRANSACTIVEDATASOURCE_H
#define TRANSACTIVEDATASOURCE_H

#include <vector>
#include <map>
#include <string>

#include "CorbaUtil.h"
#include "ITransActiveDataSource.h"
#include "MiThread.h"
#include "CallbackUser.h"
#include "RunParams.h"
#include "DataPointStateStub.h"

class INexusCommon;
class CallbackSubmission;
//class ManagedDataPointStateList;
class OPCTag;
class DataPointEventListener;

class TransActiveDataSource : public virtual ITransActiveDataSource
							  //public virtual CallbackUser,
							  //public virtual MiThread
{

	friend class TransActiveDataSourceFactory;

private:

	TransActiveDataSource();
	TransActiveDataSource ( const RunParams & paramters);

public:

	virtual ~TransActiveDataSource();


	//
	// methods required by ITransActiveDataSource interface
	//


	virtual bool startTransActiveDataSource ( void );
	virtual void stopTransActiveDataSource ( void );
	virtual void setPollingDelay ( unsigned int delay );
	virtual void performFailOver ( void );

/*
	//
	// methods required by CallbackUser interface
	//


	virtual void callback ( const CallbackSubmission * submission );


	//
	// methods required by MiThread interface
	//


	virtual void run();
	virtual void terminate();
*/
private:

	void initialiseWtOPCsvr ( void );
	void createWtOPCTag ( void );
	//void updateWtOPCTag ( const DataPointStateStub & newState );
	//std::string getEventChannel ( void );
	//bool registerEventChannel ( void );
	//void purgeCallbackSubmissions();
	//void initialisePollingDelay();
	void writeNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError );
	void ReadNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp );
	void UnknownTagNotificationHandler ( LPSTR path, LPSTR name );
	void RemoveTagNotificationHandler ( HANDLE handle, LPSTR path, LPSTR name );


private:

	unsigned int m_delay;
	//bool m_isEventRegistered;
	//bool m_threadTerminated;
	INexusCommon * m_nexusInterface;
	//ManagedDataPointStateList * m_callbackSubmissionList;
	std::map < HANDLE, OPCTag * > m_datapointHandleMap;
	DataPointEventListener * m_dataPointEventListener;

};

#endif