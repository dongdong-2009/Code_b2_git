


#pragma once

#include "RadioCommonTypes.h"
#include "../ConfigReaderLib/Parser/IParser.h"
#include "../ConfigReaderLib/ConfigReader/IIniConfigHandler.h"

enum EStratType
{
	ERadioSubsc = 0,
	ERadioProfileRequest,
	ERadioProfileDBUpdate,
	ERadioSystemStatus,
	ERadioDGNAStatus,
	ERadioQueryCallForwarding
};

class IRadioOperationStrategy
{
	public:
		virtual RadioResultData * executeRadioConfigStrategy (const RadioParamData & radioParamdata) = 0;		
		virtual void clearResultData (RadioResultData * resultData) = 0;

};

class AbstractStrat : public IRadioOperationStrategy
{
	public:
		AbstractStrat (IParser * parsetIF,
						IIniConfigHandler * configHandler,
						EStratType stratType) :
				m_stratType (stratType),
				m_parserInterface(parsetIF),
				m_configHandler(configHandler)
		{
		}
		
		EStratType getStratType (){return m_stratType;};
		
		virtual void clearResultData (RadioResultData * resultData)
		{
			delete resultData;
		}
		
		std::vector <std::string> getConfigValue (std::string strSection, std::string strKey, bool bIsCSV = true)
		{
			std::vector <std::string> vctrSeperatedVal;
			
			vctrSeperatedVal.clear();
				
			std::string strConfigResult = m_configHandler->getValueFromKeyAndSection(strSection,strKey);
			
			if (!strConfigResult.empty())
			{
				if (bIsCSV)
				{
					vctrSeperatedVal = m_parserInterface->getSeparatedStringValues(strConfigResult);
				}
				else
				{
					vctrSeperatedVal.push_back(strConfigResult);
				}
			}

			return vctrSeperatedVal;
		}

	protected:
		IParser * m_parserInterface;
		IIniConfigHandler * m_configHandler;

	private:		
		EStratType m_stratType;
};