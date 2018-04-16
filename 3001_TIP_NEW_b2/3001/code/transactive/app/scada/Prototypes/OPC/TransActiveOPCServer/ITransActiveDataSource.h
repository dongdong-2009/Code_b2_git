/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/ITransActiveDataSource.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
//---------------------------------------------------------------------------------------------------
// Class: ITransActiveDataSource
//
// ITransActiveDataSource defines a set of public methods for accessing datapoints managed
// by a TransActiveDataSource.
//
//---------------------------------------------------------------------------------------------------
#ifndef ITRANSACTIVEDATASOURCE_H
#define ITRANSACTIVEDATASOURCE_H

#include <vector>

#include "comdef.h"

#include "AttributeTypeEnum.h"
#include "AttributeDataTypeEnum.h"
#include "AttributeValueUnion.h"

class DataPointObject;
class DataPointAttribute;

class ITransActiveDataSource
{
public:


	//------------------------------------------------------------------------------------------
	// Operation: ~ITransActiveDataSource
	//	Default destructor
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	none
	//------------------------------------------------------------------------------------------
	virtual ~ITransActiveDataSource() {};

	//------------------------------------------------------------------------------------------
	// Operation: startTransActiveDataSource
	//	Start up the TransActiveDataSource
	//
	//	Returns true to indicate successful otherwise false will be returned.
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	none
	//------------------------------------------------------------------------------------------
	virtual bool startTransActiveDataSource ( void ) = 0;

	//------------------------------------------------------------------------------------------
	// Operation: stopTransActiveDataSource
	//	Stop the running TransActiveDataSource
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	none
	//------------------------------------------------------------------------------------------
	virtual void stopTransActiveDataSource ( void ) = 0;

	//------------------------------------------------------------------------------------------
	// Operation: setPollingDelay
	//	Set the delay between polling of CORBA events which contains the new datapoint's state
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	delay
	//		The delay between polling cycle
	//------------------------------------------------------------------------------------------
	virtual void setPollingDelay ( unsigned int delay ) = 0;

	//------------------------------------------------------------------------------------------
	// Operation: performFailOver
	//	Instruct the TransActiveDataSource to perform a fail over
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	none
	//------------------------------------------------------------------------------------------
	virtual void performFailOver() = 0;

	virtual void writeNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError ) = 0;

	virtual void ReadNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp ) = 0;

	virtual void UnknownTagNotificationHandler ( LPSTR path, LPSTR name ) = 0;

	virtual void RemoveTagNotificationHandler ( HANDLE handle, LPSTR path, LPSTR name ) = 0;
};

#endif