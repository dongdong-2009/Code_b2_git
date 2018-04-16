/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/TransActiveDataSourceFactory.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
//---------------------------------------------------------------------------------------------------
// Class: TransActiveDataSourceFactory
//
// TransActiveDataSourceFactory is a singleton factory that creates a singleton instance of the
// control class TransActiveDataSource.
//
//---------------------------------------------------------------------------------------------------

#ifndef TRANSACTIVEDATASOURCEFACTORY_H
#define TRANSACTIVEDATASOURCEFACTORY_H

#include "RunParams.h"

class ITransActiveDataSource;

class TransActiveDataSourceFactory
{
private:

	// declare constructor as private so that client will not have to create an instance of
	// this class

	//------------------------------------------------------------------------------------------
	// Operation: TransActiveDataSourceFactory
	//	Default Constructor
	//------------------------------------------------------------------------------------------
	TransActiveDataSourceFactory ();

public:

	//------------------------------------------------------------------------------------------
	// Operation: ~ TransActiveDataSourceFactory
	//	Default Destructor
	//------------------------------------------------------------------------------------------
	~TransActiveDataSourceFactory  ();

	//------------------------------------------------------------------------------------------
	// Operation: createTransActiveDataSource
	//	Static method to create an instance of the interface ITransActiveDataSource
	//------------------------------------------------------------------------------------------
	// Parameters:
	//------------------------------------------------------------------------------------------
	static ITransActiveDataSource * createTransActiveDataSource();

	//------------------------------------------------------------------------------------------
	// Operation: createTransActiveDataSource
	//	Static method to create an instance of the interface ITransActiveDataSource
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	paramters
	//		The runtime parameters to be used by this instance
	//------------------------------------------------------------------------------------------
	static ITransActiveDataSource * createTransActiveDataSource ( const RunParams & parameters );

	//------------------------------------------------------------------------------------------
	// Operation: destroyTransActiveDataSource
	//	Static method to destroy an instance of the interface InexusCommon
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	obj
	//		The interface to be destroyed
	//------------------------------------------------------------------------------------------
	static void destroyTransActiveDataSource ( ITransActiveDataSource * obj );

private:

	//------------------------------------------------------------------------------------------
	// Operation: getInstance
	//	Static method to get an instance of this factory
	//------------------------------------------------------------------------------------------
	static TransActiveDataSourceFactory * getInstance();

	//------------------------------------------------------------------------------------------
	// Operation: singletonCreateTransActiveDataSource
	//	Create an instance of the interface ITransActiveDataSource
	//------------------------------------------------------------------------------------------
	// Parameters:
	//------------------------------------------------------------------------------------------
	ITransActiveDataSource * singletonCreateTransActiveDataSource();

	//------------------------------------------------------------------------------------------
	// Operation: singletonCreateTransActiveDataSource
	//	Create an instance of the interface ITransActiveDataSource
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	paramters
	//		The runtime parameters to be used by this instance
	//------------------------------------------------------------------------------------------
	ITransActiveDataSource * singletonCreateTransActiveDataSource ( const RunParams & parameters );

	//------------------------------------------------------------------------------------------
	// Operation: singletonDestroyTransActiveDataSource
	//	Destroy an instance of the interface ITransActiveDataSource
	//------------------------------------------------------------------------------------------
	// Parameters:
	//	obj
	//		The interface to be destroyed
	//------------------------------------------------------------------------------------------
	void singletonDestroyTransActiveDataSource ( ITransActiveDataSource * obj );


private:

	//------------------------------------------------------------------------------------------
	// Attribute: m_transActiveDataSourceFactory
	//	Static pointer to an instance of this factory
	//------------------------------------------------------------------------------------------
	static TransActiveDataSourceFactory * m_transActiveDataSourceFactory;

	//------------------------------------------------------------------------------------------
	// Attribute: m_transActiveDataSource
	//	Pointer to an instance of the interface ITransActiveDataSource
	//------------------------------------------------------------------------------------------
	ITransActiveDataSource * m_transActiveDataSource;

	//------------------------------------------------------------------------------------------
	// Attribute: m_refCounter
	//	The reference counter to keep track of the number of instances of the
	//	interface ITransActiveDataSource that have been created or destroyed.
	//------------------------------------------------------------------------------------------
	int m_refCounter;
};

#endif