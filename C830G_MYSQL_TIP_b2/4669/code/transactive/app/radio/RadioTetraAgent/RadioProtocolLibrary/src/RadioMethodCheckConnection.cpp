// RadioMethodCheckConnection.cpp: implementation of the RadioMethodCheckConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "RadioMethodCheckConnection.h"
#include "RadioProtocol.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RadioMethodCheckConnection::RadioMethodCheckConnection( bool enabled ):
RadioMethod( 5, METHOD_CHECKCONNECTION, 0xFEFE /*TcpServer will ignore this*/ )
{
	enableCheckConnection( enabled );
}

RadioMethodCheckConnection::~RadioMethodCheckConnection()
{

}

void RadioMethodCheckConnection::enableCheckConnection( bool enable )
{
	setParameterByte( 0, enable?1:0 );
}

void RadioMethodCheckConnection::setCheckInterval(unsigned long interval)
{
	setParameterLong( 1, interval );
}