#pragma once

#include "StdAfx.h"
#include "RadioCommonTypes.h"
#include "IRadioOperationStrategy.h"

#include "../ConfigReaderLib/Parser/IParser.h"
#include "../ConfigReaderLib/ConfigReader/IIniConfigHandler.h"

class RadioStratFactory
{
	public:
		RadioStratFactory();
		~RadioStratFactory();

		IRadioOperationStrategy * createRadioStrategy (EStratType stratType);

	private:
		IParser * m_parserInterface;
		IIniConfigHandler * m_configHandler;
};
