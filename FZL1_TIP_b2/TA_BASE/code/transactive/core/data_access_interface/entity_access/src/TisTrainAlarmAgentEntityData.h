/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3002_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/TisTrainAlarmAgentEntityData.h $
  * @author Wang Wang
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2017/03/23 11:10:12 $
  * Last modified by: $Author: Wang Wang $
  * 
  * TisTrainAlarmAgentEntityData implements the interface EntityData to provide read-only access
  * to TrainAlarmAgent entity data.
  */

#ifndef TISTRAINALARMAGENT_ENTITY_DATA_H
#define TISTRAINALARMAGENT_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/EntityData.h"

#include <vector>
#include <string>
#include <utility> // for std::pair

namespace TA_Base_Core
{
    class TisTrainAlarmAgentEntityData : public virtual EntityData
    {
    // public method
	public:

		TisTrainAlarmAgentEntityData();
        TisTrainAlarmAgentEntityData( unsigned long key );

        virtual ~TisTrainAlarmAgentEntityData();
        
        /**
         * getTrainType
         *
         * Returns the train type set for TrainAlarmAgent entity
         *
         * @return The vector includes train type that would be represented by string
         */
        virtual const std::vector<std::string> getTrainTypes();
        
        /**
         * getStartAddresses
         *
         * Returns the every startAddresses of each train type for TrainAlarmAgent entity
         * pair->first represents train type, parin->second represents start address.
         *
         * @return The vector includes all the train start address for each train type
         */        
        virtual const std::vector<std::pair<std::string, unsigned long> > getStartAddresses();        
        
        /**
         * getTrainNumbers
         *
         * Returns the every train numbers of each train type for TrainAlarmAgent entity
         * pair->first represents train type, parin->second represents train numbers.
         *
         * @return The vector includes all the train numbers for each train type
         */        
        virtual const std::vector<std::pair<std::string, unsigned long> > getTrainNumbers();
        
        /**
         * getTrainInfoLen
         *
         * Returns the every train info length of each train type for TrainAlarmAgent entity
         * pair->first represents train type, parin->second represents train info length.
         *
         * @return The vector includes all the train info length for each train type
         */
        virtual const std::vector<std::pair<std::string, unsigned long> > getTrainInfoLens();
                
        std::string getType();
        static  const std::string getStaticType();
        void invalidate();
        static IEntityData* clone ( unsigned long key);
        virtual const std::string getStationName();
        virtual const std::string getAssetName();
        virtual int getAnalogueDataPointUpdateInterval();
        
    // private method    
    private:		
        TisTrainAlarmAgentEntityData( const TisTrainAlarmAgentEntityData & obj );
		TisTrainAlarmAgentEntityData & operator= ( const TisTrainAlarmAgentEntityData &);
        
        // this method provides construct way to let data correspond to each train type, data should be string and will split it.
        void constructDataWithTrainType(const std::string& title, std::vector<std::pair<std::string, unsigned long> >& result);
        void parseTrainData(const std::string& data, std::vector<std::pair<std::string, unsigned long> >& result);

    // private member
    public:
        static const std::string ENTITY_TYPE;
		static const std::string STATIONNAME;		
		static const std::string ASSET_NAME;		
        static const std::string TRAIN_TYPE;
        static const std::string TRAIN_NUMBERS;
        static const std::string TRAIN_INFO_LEN;
        static const std::string TRAIN_START_ADDRESS;
        static const std::string ANALOGUE_DP_UPDATE_INTERVAL;        
    
     private:    
        static const std::string m_spliter;  // using for split data of each train type configuration.
        static const std::string m_typeSpliter;  // using for split train type configuration.
        static const std::string m_rangeSpliter;  // using for split train type configuration.
    };
    
    typedef boost::shared_ptr<TisTrainAlarmAgentEntityData> TisTrainAlarmAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif
