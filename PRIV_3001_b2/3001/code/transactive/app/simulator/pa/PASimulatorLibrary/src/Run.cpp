// Run.cpp: implementation for the Run class.
//
//////////////////////////////////////

#include "Run.h"
#include "RTUSimulator.h"

Run::Run( RTUSimulator& simulator ) : 
m_sim( simulator ),
m_portID( 0 )
{

}

Run::~Run( void )
{

}

int Run::svc( void )
{
	ACE_DEBUG
      ((LM_DEBUG, ACE_TEXT ("(%t) Handler Thread running\n")));
	m_sim.StartSvr( m_portID );
	return 0;
}

void Run::SetID( const int& i )
{
	m_portID = i;
}
