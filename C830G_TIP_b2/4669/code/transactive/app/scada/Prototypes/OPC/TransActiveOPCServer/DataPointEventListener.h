/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/DataPointEventListener.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef DATAPOINTEVENTLISTNER_H
#define DATAPOINTEVENTLISTNER_H

#include <map>

#include "CorbaUtil.h"
#include "MiThread.h"
#include "CallbackUser.h"
#include "DataPointStateStub.h"

class CallbackSubmission;
class ManagedDataPointStateList;
class OPCTag;
class INexusCommon;

class DataPointEventListener : public virtual MiThread,
							   public virtual CallbackUser
{
private:

	bool m_threadTerminated;
	INexusCommon * m_nexusInterface;
	ManagedDataPointStateList * m_callbackSubmissionList;
	std::map < HANDLE, OPCTag * > m_datapointHandleMap;

	void purgeCallbackSubmissions();
	void updateWtOPCTag ( const DataPointStateStub & newState );

public:

	DataPointEventListener();
	virtual ~DataPointEventListener();

	virtual void callback ( const CallbackSubmission * submission );
	virtual void run();
	virtual void terminate();

	void createWtOPCTag ( const std::string & attributeDataType,
						  const std::string & dataPointName,
						  const std::string & dataPointCorbaName,
						  unsigned long entityKey );
	bool registerEventChannel ( void );
	void writeNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError );
	void ReadNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp );
	void UnknownTagNotificationHandler ( LPSTR path, LPSTR name );
	void RemoveTagNotificationHandler ( HANDLE handle, LPSTR path, LPSTR name );
};

#endif