// Run.h: interface for the Run class.
//
//////////////////////////////////////

#pragma once
#include "ace/Task.h"
class RTUSimulator;

class Run : public ACE_Task_Base
{
public:
	Run( RTUSimulator& );
	~Run( void );
	virtual int svc( void );
	void SetID( const int& );

private:
	RTUSimulator & m_sim;
	int m_portID;
};
