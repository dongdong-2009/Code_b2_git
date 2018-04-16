/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/TisTrainAlarmAgentEntityData.cpp $
  * @author Dhanshri
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by: $Author: grace.koh $
  * 
  * TisTrainAlarmAgentEntityData implements the interface EntityData to provide read-only access
  * to TrainAlarmAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/TisTrainAlarmAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericAgentException.h"

#include "boost/algorithm/string/classification.hpp" // Include boost::for is_any_of
#include "boost/algorithm/string/split.hpp" // Include for boost::split
#include "boost/lexical_cast.hpp" // Include for lexical_cast<T>

#include <sstream>
#include <algorithm>
#include "TisTrainAlarmAgentEntityData.h"

namespace TA_Base_Core
{

    const std::string TisTrainAlarmAgentEntityData::ENTITY_TYPE                            = "TisAgent";
    const std::string TisTrainAlarmAgentEntityData::STATIONNAME                            = "StationName";
	const std::string TisTrainAlarmAgentEntityData::ASSET_NAME                             = "AssetName";		
    const std::string TisTrainAlarmAgentEntityData::TRAIN_TYPE                             = "TrainType";
    const std::string TisTrainAlarmAgentEntityData::TRAIN_NUMBERS                          = "TrainNumber";
    const std::string TisTrainAlarmAgentEntityData::TRAIN_INFO_LEN                         = "TrainInfoLen";
    const std::string TisTrainAlarmAgentEntityData::TRAIN_START_ADDRESS                    = "TrainStartAddress";
    const std::string TisTrainAlarmAgentEntityData::ANALOGUE_DP_UPDATE_INTERVAL            = "AnalogueDataPointUpdateInterval";    
    
    const std::string TisTrainAlarmAgentEntityData::m_spliter = ",;() ";
    const std::string TisTrainAlarmAgentEntityData::m_typeSpliter = ":";
    const std::string TisTrainAlarmAgentEntityData::m_rangeSpliter = "-";

    // just constructors without anything
    TisTrainAlarmAgentEntityData::TisTrainAlarmAgentEntityData()
    {
        
    }
    
    TisTrainAlarmAgentEntityData::TisTrainAlarmAgentEntityData(unsigned long key)
    :
    EntityData(key, getStaticType()) // init base class
    {
        
    }
    
    TisTrainAlarmAgentEntityData::~TisTrainAlarmAgentEntityData()
    {
        
    }    
    
    std::string TisTrainAlarmAgentEntityData::getType()
	{
		return getStaticType();
	}
    
    const std::string TisTrainAlarmAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
    
    void TisTrainAlarmAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }
    
    IEntityData* TisTrainAlarmAgentEntityData::clone ( unsigned long key )
    {
        return new TisTrainAlarmAgentEntityData(key);
    }
    
    const std::string TisTrainAlarmAgentEntityData::getStationName()
	{
        return getHelper()->getParameter(STATIONNAME);
	}
    
    const std::string TisTrainAlarmAgentEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}

    int TisTrainAlarmAgentEntityData::getAnalogueDataPointUpdateInterval()
    {
        int analogueDataPointUpdateInterval = 0;
        std::string value = getHelper()->getParameter(ANALOGUE_DP_UPDATE_INTERVAL);
        if (!value.empty())
        {
            analogueDataPointUpdateInterval = EntityHelper::getIntegerData(value);
        }

        return analogueDataPointUpdateInterval;
    }
    
    const std::vector<std::string> TisTrainAlarmAgentEntityData::getTrainTypes()
    {   
        std::vector<std::string> trainTypes;
        std::string trainType = getHelper()->getParameter(TRAIN_TYPE);
        // if configuration exists
        if (!trainType.empty())
        {            
            boost::split(trainTypes, trainType, boost::is_any_of(m_typeSpliter), boost::token_compress_on);
        }
        else
        {
            std::ostringstream desc;
			desc << "Parameter [" << TRAIN_TYPE << "] not configured properly in database, it must be existed and the value cant be null.";
			TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
        }
        
        std::sort(trainTypes.begin(), trainTypes.end());
        
        // log
        for (unsigned int i; i < trainTypes.size(); i++)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Get Train Type: %s", trainTypes[i].c_str());
        }

        return trainTypes;
    }
	
    const std::vector<std::pair<std::string, unsigned long> > TisTrainAlarmAgentEntityData::getStartAddresses()
    {
        std::vector<std::pair<std::string, unsigned long> > startAddresses;
        constructDataWithTrainType(TRAIN_START_ADDRESS, startAddresses);
        // startAddress result may has 2 format, one is only address and others follow it. You can calculate them base on train len and train number.
        // another is detailed addresses for each train.
        // e.g. SM(1):SF(2000, 2040,2080). SM type start address with 1, if SM has 5 train and len is 10, so remained addresses are 11, 21, 31...; another case is SF has detailed address for each train. SF train numbers should meet the addresses number.
        std::vector<std::string> trainTypes = getTrainTypes();
        std::vector<std::pair<std::string, unsigned long> > trainNumbers = getTrainNumbers();
        std::vector<std::pair<std::string, unsigned long> > trainInfoLens = getTrainInfoLens();
        std::vector<std::pair<std::string, unsigned long> > tempStartAddresses = startAddresses;        
        for (unsigned int i = 0; i < trainTypes.size(); i++)
        {
            std::vector<unsigned long> address;
            unsigned int trainCount = 0;
            unsigned long trainLen = 0;
            
            for (std::vector<std::pair<std::string, unsigned long> >::iterator it = tempStartAddresses.begin(); it != tempStartAddresses.end(); it++)
            {
                if ( trainTypes[i] == it->first )
                {
                    address.push_back(it->second);
                }
            }
            
            for (std::vector<std::pair<std::string, unsigned long> >::iterator it = trainNumbers.begin(); it != trainNumbers.end(); it++)
            {
                if ( trainTypes[i] == it->first )
                {
                    trainCount++;
                }
            }
            
            for (std::vector<std::pair<std::string, unsigned long> >::iterator it = trainInfoLens.begin(); it != trainInfoLens.end(); it++)
            {
                if ( trainTypes[i] == it->first )
                {
                    trainLen = it->second;
                    break;
                }
            }
            
            // calcuate each train address for format 1 except 1st train
            if (address.size() == 1 && trainCount > 1)
            {
                for (unsigned int j = 1; j < trainCount; j++)
                {
                    // address[0] means start address
                    std::pair<std::string, unsigned long> p = std::make_pair(trainTypes[i], address[0]+j*trainLen);
                    startAddresses.push_back(p);
                }
            }
            // if not format 1, so format 2 requires train number amount equal to train address amount. unequal will throw exception
            else if (address.size() != trainCount)
            {
                std::ostringstream desc;
                desc << "address amount doesn't meet train amount";
                TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
            }
        }
        
        for(std::vector<std::pair<std::string, unsigned long> >::iterator itAddr = startAddresses.begin();
            itAddr != startAddresses.end();)
            {
                std::vector<std::string>::iterator itType = std::find(trainTypes.begin(), trainTypes.end(), itAddr->first);
                if (itType != trainTypes.end())
                    itAddr++;
                else
                    startAddresses.erase(itAddr);
            }
        
        std::sort(startAddresses.begin(), startAddresses.end());
        
        // log
        for (unsigned int i = 0; i < startAddresses.size(); i++)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Get Train Type: %s, start address: %d ", 
                        startAddresses[i].first.c_str(), startAddresses[i].second);
        }
            
        return startAddresses;
    }

    const std::vector<std::pair<std::string, unsigned long> > TisTrainAlarmAgentEntityData::getTrainNumbers()
    {
        std::vector<std::pair<std::string, unsigned long> > trainNumbers;
        constructDataWithTrainType(TRAIN_NUMBERS, trainNumbers);
        
        std::vector<std::string> trainTypes = getTrainTypes();
        for(std::vector<std::pair<std::string, unsigned long> >::iterator itNums = trainNumbers.begin();
        itNums != trainNumbers.end();)
        {
            std::vector<std::string>::iterator itType = std::find(trainTypes.begin(), trainTypes.end(), itNums->first);
            if (itType != trainTypes.end())
                itNums++;
            else
                trainNumbers.erase(itNums);
        }
        
        std::sort(trainNumbers.begin(), trainNumbers.end());
        
        // log        
        for (unsigned int i = 0; i < trainNumbers.size(); i++)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Get Train Type: %s, numbers: %d ", 
                        trainNumbers[i].first.c_str(), trainNumbers[i].second);
        }
        
        return trainNumbers;
    }
    
    const std::vector<std::pair<std::string, unsigned long> > TisTrainAlarmAgentEntityData::getTrainInfoLens()
    {
        std::vector<std::pair<std::string, unsigned long> > trainInfoLens;
        constructDataWithTrainType(TRAIN_INFO_LEN, trainInfoLens);
        
        std::vector<std::string> trainTypes = getTrainTypes();
        for(std::vector<std::pair<std::string, unsigned long> >::iterator itLens = trainInfoLens.begin();
        itLens != trainInfoLens.end();)
        {
            std::vector<std::string>::iterator itType = std::find(trainTypes.begin(), trainTypes.end(), itLens->first);
            if (itType != trainTypes.end())
                itLens++;
            else
                trainInfoLens.erase(itLens);
        }
        
        std::sort(trainInfoLens.begin(), trainInfoLens.end());
        
        // log
        for (unsigned int i = 0; i < trainInfoLens.size(); i++)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Get Train Type: %s, data length: %d ", 
                        trainInfoLens[i].first.c_str(), trainInfoLens[i].second);
        }
        
        return trainInfoLens;
    }
    
    void TisTrainAlarmAgentEntityData::constructDataWithTrainType(const std::string& title, std::vector<std::pair<std::string, unsigned long> >& result)
    {                 
        std::string data = getHelper()->getParameter(title); // get title value from database
        if (!data.empty()) // data is not null
        {
            std::vector<std::string> vecData; // store the result that has been split to vector
            boost::split(vecData, data, boost::is_any_of(m_typeSpliter), boost::token_compress_on); // support format as SM(1, 2, 13-20, 27):SF(3,7-9). SM and SF are train types
            for (unsigned int i = 0; i < vecData.size(); i++)
            {                
                parseTrainData(vecData[i], result); // the expect vecData[i] format likes train_types(1, 2, 13-20, 27)
            }
        }
    }
    
    void TisTrainAlarmAgentEntityData::parseTrainData(const std::string& data, std::vector<std::pair<std::string, unsigned long> >& result)
    {
        TA_ASSERT(false == data.empty(), "train data cant be null string!");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "train data %s", data.c_str());
        
        std::vector<std::string> vecData;
        boost::split(vecData, data, boost::is_any_of(m_spliter), boost::token_compress_on);
        
        std::vector<std::string> vecTypes = getTrainTypes();
        
        // vecData[0] should be represented train type, so started from 1        
        if (std::find(vecTypes.begin(), vecTypes.end(), vecData[0]) == vecTypes.end())
            return;
        
        for (unsigned int i = 1; i < vecData.size(); i++)
        {
            if (vecData[i].empty())
                continue;                       
            
            std::vector<std::string> range;
            boost::split(range, vecData[i], boost::is_any_of(m_rangeSpliter), boost::token_compress_on); // try to find whether it's included '-'
            if (range.size() == 1)
            {
                unsigned long n = boost::lexical_cast<unsigned long>(range[0].c_str());
                std::pair<std::string, unsigned long> p = std::make_pair(vecData[0], n);
                result.push_back(p);
            }
            else if (range.size() == 2) // pars 1-30 range case
            {                
                unsigned long start = boost::lexical_cast<unsigned long>(range[0].c_str());
                unsigned long end = boost::lexical_cast<unsigned long>(range[1].c_str());
                               
                if (start > end) // swap them when occur this case
                {
                    unsigned long temp = end;
                    end = start;
                    start = temp;
                }                
                
                for (unsigned int i = start; i <= end; i++)
                {
                    std::pair<std::string, unsigned long> p = std::make_pair(vecData[0], i);
                    result.push_back(p);
                }
            }
            else
            {
                std::ostringstream desc;
                desc << "data range is not configured properly in database, it must be existed and the value cant be null.";
                TA_THROW( GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, desc.str().c_str()) );
            }
        }                         
    }
    
} //close namespace TA_Base_Core
