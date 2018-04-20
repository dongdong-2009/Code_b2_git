#ifndef HISTORYCONFIG_H
#define HISTORYCONFIG_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/history/scada_historian/src/HistoryConfig.h $
  * @author:   Anita Lee
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include <string>
#include "app/history/scada_historian/scada_historian_dmif/src/DataCollectionDmIf.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/corba/src/ServantBase.h"
#include "bus/generic_agent/src/IEntity.h"


using namespace TA_Core;


namespace TA_App
{
    class HistoryConfig : public virtual TA_Bus::IEntity,
                          public virtual POA_TA_App::DataCollectionDmIf,
                          public virtual TA_Core::ServantBase
    {

    public:

		// TODO: Put this into a Transactive Exception class
	    class HistorianCfgErrorException
	    {
	    public:

		    enum ErrorType
		    {
			    MissingParam,
			    InvalidParam
		    };

		    HistorianCfgErrorException(const std::string& description, ErrorType type, unsigned long entityKey) :
			    m_description(description), m_type(type), m_eKey(entityKey) {}

            ~HistorianCfgErrorException() {}

		    std::string Description() { return m_description; }

		    void GenerateAlarm();

	    private:

		    std::string m_description;
		    ErrorType m_type;
		    unsigned long m_eKey;
	    };

	    friend class HistorianCfgErrorException;

	    /**
          * HistoryConfig
          *
          * Constructor.
		  *
		  * @param entityData Entity data passed from Generic Agent
          *
          */
	    HistoryConfig(IEntityData& entityData);
		    //throw(HistorianCfgErrorException);

	    /**
          * ~HistoryConfig
          *
          * Destructor.
		  *
          */
	    virtual ~HistoryConfig();

	    /**
          * isValid
          *
          * This method satisfies the IEntity pure virtual method. 
	      * It checks that the mandatory parameters are present and valid.
		  *
		  * @return True if the parameters are present and valid
		  *
          */
	    virtual bool isValid();

	    /**
          * start
          *
          * This method satisfies the IEntity pure virtual method.
	      * It currently does nothing
		  *
          */
	    virtual void start() {}

	    /**
          * stop
          *
          * This method satisfies the IEntity pure virtual method.
	      * It currently does nothing
		  *
          */
	    virtual void stop() {}

        /**
          * update
          *
		  * This method updates any configuration parameters used after a
		  * configuration event is received.
		  *
		  * @param updateEvent The configuration event details
		  *
          */
        virtual void update(const TA_Core::ConfigUpdateDetails& updateEvent);

        /**
          * remove
          *
		  * Called when a configuration update event is received to delete an 
		  * entity.This method commences any clean up operations.
          * 
          * The agent then still has to remove the object from its vector of entities.
		  *
          */
        virtual void remove();

        /**
          * setToControlMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to control. This enables the
          * entity to perform tasks such as connecting to any field devices it is
          * responsible for communicating with.
		  *
          */
        virtual void setToControlMode();

        /**
          * setToMonitorMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to monitor. This enables the
          * entity to perform tasks such as disconnecting from any field devices it was
          * communicating with.
		  *
          */
        virtual void setToMonitorMode();

	    /**
          * DataType
          *
          * Get the History Config data type. This will be either: 
		  * SAMPLE, AVERAGE, MINIMUM or MAXIMUM.
		  *
		  * @return The data type
		  *
          */
        TA_App::DataCollectionDmIf::DataCollectionType DataType() { return m_dataType; }

	    /**
          * GetDataCollection
          *
          * Get the History Config attributes
		  *
		  * @return A structure containing the attributes
		  *
          */
	    TA_App::DataCollectionDmIf::DataCollection* GetDataCollection();

	    /**
          * SetDataCollection
          *
          * Set the History Config attributes
		  *
		  * @param newDataCollection A structure containing the attributes
		  * @param sessionId The session id
		  *
          */
        void SetDataCollection(const TA_App::DataCollectionDmIf::DataCollection& newDataCollection, const char* sessionId);
		    //throw(nexus::NoPermission, nexus::BadParam);

        /**
          * getEntityName
          *
          * Get the name of the entity
		  *
		  * @return Name of entity
		  *
          */
        std::string getEntityName()
        {
            return m_entityName;
        }

        /**
          * getBaseDataName
          *
          * Get the name of the base data entity
		  *
		  * @return Name of base data entity
		  *
          */
        std::string getBaseDataName()
        {
            return m_baseData;
        }

	    /**
          * process
          *
          * This method does the actual processing, where it executes a database
		  * insertion or calls a database procedure, or both. What it actually does
		  * depends of the data type the History Config represents. This needs to
		  * be implemented by all History Configs.
		  *
		  * @param timestamp The timestamp used for the database
		  *
          */
	    virtual void process(time_t timestamp) = 0;

        /**
          * getNextProcessTime
          *
          * Get the time when the next process() call is due
		  *
		  * @return Time in time_t format
		  *
          */
        time_t getNextProcessTime()
        {
            return m_nextProcessTime;
        }

        /**
          * isTimeToProcess
          *
          * Indicates if it is time to call process(). Updates
          * the timer if it it time to process.
		  *
          * @param currentTime Current time in time_t format
          *
		  * @return TRUE if it is time to process
		  *
          */
        bool isTimeToProcess(time_t currentTime);

    protected:

		TA_App::DataCollectionDmIf::DataCollectionType m_dataType; // SAMPLE, AVERAGE, MIMUMUM or MAXIMUM

	    unsigned long m_entityKey;		// pkey of history config entity

		std::string m_entityName;		// name of history config entity

        unsigned long m_timePeriod;		// frequency of sample or calculation (seconds)		
	    
	    std::string m_baseData;			// name of base entity (dependency)
	     
	    std::string m_shortDesc;		// short character description, eg. "6 Min Avg"

	    std::string m_longDesc;			// long character description, eg. "Six Minute Average"
	    
	    bool m_archiveFlag;				// TRUE: archive data, FALSE: do not archive

	    unsigned long m_storageTime;	// time data is stored in database (days)

		//int m_qualityMask;				// mask for quality flag

	    std::string m_tableName;		// table name where data is stored

		TA_App::DataCollectionDmIf::SimpleTime m_startOfDay;	  // time to start processing
	    
	    bool m_isValid;

        enum ErrorType
        {
            MISSING,
            INVALID,
        };

        void logError(const std::string& parameterName, ErrorType eType, const std::string& details = "");

    private:

		HistoryConfig();
	    
	    bool baseDataValid(const std::string& baseData);
        void setBeginProcessTime();
        time_t calculateBeginProcessTime(time_t currentTime, int currentHour, int currentMinute, bool pastStartOfDay);

        std::string m_tempDataType;
        std::string m_tempStartOfDay;
        time_t m_nextProcessTime;

    };

} // TA_App

#endif // HISTORYCONFIG_H

