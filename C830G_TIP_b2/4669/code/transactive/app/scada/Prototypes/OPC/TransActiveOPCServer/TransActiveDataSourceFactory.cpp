/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/TransActiveDataSourceFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
//---------------------------------------------------------------------------------------------------
// Class: TransActiveDataSourceFactory
//
// TransActiveDataSourceFactory is a singleton factory that creates a singleton instance of the
// control class TransActiveDataSource.
//
//---------------------------------------------------------------------------------------------------

#pragma warning ( disable:4786 )

#include "ITransActiveDataSource.h"
#include "TransActiveDataSource.h"
#include "TransActiveDataSourceFactory.h"

// initialise static data members
TransActiveDataSourceFactory * TransActiveDataSourceFactory::m_transActiveDataSourceFactory = 0;

//------------------------------------------------------------------------------------------
// Operation: TransActiveDataSourceFactory
//	Default Constructor
//------------------------------------------------------------------------------------------
TransActiveDataSourceFactory::TransActiveDataSourceFactory()
:
m_transActiveDataSource ( 0 ),
m_refCounter ( 0 )
{

}


//------------------------------------------------------------------------------------------
// Operation: ~ TransActiveDataSourceFactory
//	Default Destructor
//------------------------------------------------------------------------------------------
TransActiveDataSourceFactory::~TransActiveDataSourceFactory()
{
	if ( m_transActiveDataSource != 0 )
	{
		delete m_transActiveDataSource;
		m_transActiveDataSource = 0;
		m_refCounter = 0;
	}
}


//------------------------------------------------------------------------------------------
// Operation: createTransActiveDataSource
//	Static method to create an instance of the interface ITransActiveDataSource
//------------------------------------------------------------------------------------------
// Parameters:
//------------------------------------------------------------------------------------------
ITransActiveDataSource * TransActiveDataSourceFactory::createTransActiveDataSource()
{
	return getInstance()->singletonCreateTransActiveDataSource();
}


//------------------------------------------------------------------------------------------
// Operation: createTransActiveDataSource
//	Static method to create an instance of the interface ITransActiveDataSource
//------------------------------------------------------------------------------------------
// Parameters:
//	paramters
//		The runtime parameters to be used by this instance
//------------------------------------------------------------------------------------------
ITransActiveDataSource * TransActiveDataSourceFactory::createTransActiveDataSource ( const RunParams & parameters )
{
	return getInstance()->singletonCreateTransActiveDataSource ( parameters );
}


//------------------------------------------------------------------------------------------
// Operation: destroyTransActiveDataSource
//	Static method to destroy an instance of the interface InexusCommon
//------------------------------------------------------------------------------------------
// Parameters:
//	obj
//		The interface to be destroyed
//------------------------------------------------------------------------------------------
void TransActiveDataSourceFactory::destroyTransActiveDataSource ( ITransActiveDataSource * obj )
{
	getInstance()->singletonDestroyTransActiveDataSource ( obj );
}


//------------------------------------------------------------------------------------------
// Operation: getInstance
//	Static method to get an instance of this factory
//------------------------------------------------------------------------------------------
TransActiveDataSourceFactory * TransActiveDataSourceFactory::getInstance()
{
	if ( m_transActiveDataSourceFactory == 0 )
	{
		m_transActiveDataSourceFactory = new TransActiveDataSourceFactory();
	}

	return m_transActiveDataSourceFactory;
}


//------------------------------------------------------------------------------------------
// Operation: singletonCreateTransActiveDataSource
//	Create an instance of the interface ITransActiveDataSource
//------------------------------------------------------------------------------------------
// Parameters:
//------------------------------------------------------------------------------------------
ITransActiveDataSource * TransActiveDataSourceFactory::singletonCreateTransActiveDataSource()
{
	if ( m_refCounter == 0 )
	{
		m_transActiveDataSource = new TransActiveDataSource();
	}

	m_refCounter++;

	return m_transActiveDataSource;
}


//------------------------------------------------------------------------------------------
// Operation: singletonCreateTransActiveDataSource
//	Create an instance of the interface ITransActiveDataSource
//------------------------------------------------------------------------------------------
// Parameters:
//	paramters
//		The runtime parameters to be used by this instance
//------------------------------------------------------------------------------------------
ITransActiveDataSource * TransActiveDataSourceFactory::singletonCreateTransActiveDataSource ( const RunParams & parameters )
{
	if ( m_refCounter == 0 )
	{
		m_transActiveDataSource = new TransActiveDataSource ( parameters );
	}

	m_refCounter++;

	return m_transActiveDataSource;
}


//------------------------------------------------------------------------------------------
// Operation: singletonDestroyTransActiveDataSource
//	Destroy an instance of the interface ITransActiveDataSource
//------------------------------------------------------------------------------------------
// Parameters:
//	obj
//		The interface to be destroyed
//------------------------------------------------------------------------------------------
void TransActiveDataSourceFactory::singletonDestroyTransActiveDataSource ( ITransActiveDataSource * obj )
{
	// make sure the object passed in is one of the object created by this
	// factory
	TransActiveDataSource * theCDS = 0;
	theCDS = dynamic_cast< TransActiveDataSource * > ( obj );

	// if it is
	if ( theCDS != 0 )
	{
		// then decrement the reference counter
		m_refCounter--;

		// if the counter is nil
		if ( m_refCounter <= 0 )
		{
			// then destroy the only instance
			delete m_transActiveDataSource;
			m_transActiveDataSource = 0;
			m_refCounter = 0;
		}
	}
}
