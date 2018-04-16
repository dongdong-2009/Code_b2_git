#pragma once

#include "IIniConfigHandler.h"

class IniConfigAccessFactory 
{
	public:
		IniConfigAccessFactory(void);
		~IniConfigAccessFactory(void);
		
		static IIniConfigHandler * getConfigHandler ();
		static void clearConfigHandler ();

	private:
		static IIniConfigHandler * m_iniConfigHandler;
		static void createConfigHandler ();
										
};
