/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/AlarmTypeAccessFactory.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * AlarmTypeAccessFactory is a singleton that is used to retrieve AlarmTypeData objects from the database.
 * It provides both read-write, and read-only objects, but does not create new objects as they are not
 * able to be defined through the system.
 */

#pragma warning(disable:4786)

#include "core/data_access_interface/src/AlarmTypeAccessFactory.h"
#include "core/data_access_interface/src/ConfigAlarmType.h"
#include "core/data_access_interface/src/AlarmTypeData.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

    AlarmTypeAccessFactory* AlarmTypeAccessFactory::m_instance = 0;

    AlarmTypeAccessFactory& AlarmTypeAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new AlarmTypeAccessFactory();
        }
        return *m_instance;
    }


	void AlarmTypeAccessFactory::getAlarmTypeBySql(const std::string& sql, std::vector<IAlarmTypeData*>& alarmTypes, const bool readWrite)
	{
        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);

		// Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
		columnNames.push_back("MEATYP_ID");
        columnNames.push_back("NAME");
        columnNames.push_back("MEMCONID");
        columnNames.push_back("CONTEXTNAME");
        columnNames.push_back("DESCRIPTION");
        columnNames.push_back("SEVERITY");
        columnNames.push_back("PRINTALARM");
        columnNames.push_back("SYSTEM_ALARM");
		columnNames.push_back("ISVISIBLE");
        columnNames.push_back("PERSISTALARM");
        columnNames.push_back("ALARM_VALUE");
        columnNames.push_back("NORMAL_VALUE");
		columnNames.push_back("MMS_STATE");	//TD17682 marc_bugfix Support mms type
        columnNames.push_back("DATEMODIFIED");
        columnNames.push_back("DATECREATED");
		columnNames.push_back("PHYSICAL_ALARM"); //20449++

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        IData* data = databaseConnection->executeQuery(sql,columnNames);
        
        do
        {
		    // Loop through and create all the objects
		    for(unsigned int i = 0; i < data->getNumRows(); i++)
		    {
				try
				{
					// decide which Alarm type object to return
					if (readWrite) // need a config Alarm type
					{
						alarmTypes.push_back( new ConfigAlarmType(i, *data) );
					}
					else // need a standard Alarm type
					{
						alarmTypes.push_back( new AlarmTypeData(i, *data) );
					}
				}
				catch (TA_Base_Core::DataException&)
				{
					// Clean up the data pointer
					delete data;
					data = NULL;
					throw;
				}
            }
			delete data;
			data = NULL;
        }
        while ( databaseConnection->moreData(data) );
    }

    
    IAlarmTypeData* AlarmTypeAccessFactory::getAlarmTypeByKey(const std::string& key,const bool readWrite)
    {
        FUNCTION_ENTRY("getAlarmTypeByKey");
		
        // create the SQL string to retrieve the key of the Alarm type
        // this also doubles as a check to ensure an operator with the specified
        // PKEY actually exists.
		//TD17682 marc_bugfix Support mms type
		std::ostringstream sql;
        sql << " select a.meatyp_id, a.type_name, a.memcon_id, mc.context_name, nvl(a.type_description,'NULL') as type_description, a.measev_id, a.print_alarm, a.system_alarm,"
            << " a.isvisible, a.persist_alarm, a.alarm_value, a.normal_value, a.mms_state, TO_CHAR(a.date_modified,'YYYYMMDDHH24MISS'), TO_CHAR(a.date_created,'YYYYMMDDHH24MISS'), a.physical_alarm "
            << " from me_alarm_type a, me_message_context mc where a.memcon_id = mc.memcon_id and a.meatyp_id = " << key;

		std::vector<IAlarmTypeData*> alarmTypes;
		getAlarmTypeBySql(sql.str(), alarmTypes, readWrite);

        if ( 0 == alarmTypes.size())
        {
            std::ostringstream message;
			message << "No data found for AlarmType with key " << key;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == alarmTypes.size(), "AlarmType key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return alarmTypes[0];
    }

	IAlarmTypeData* AlarmTypeAccessFactory::getAlarmTypeByKey(unsigned long key,const bool readWrite)
    {
        FUNCTION_ENTRY("getAlarmTypeByKey");
		
        // create the SQL string to retrieve the key of the Alarm type
        // this also doubles as a check to ensure an operator with the specified
        // PKEY actually exists.
		std::ostringstream sql;
        sql << " select a.meatyp_id, a.type_name, a.memcon_id, mc.context_name, nvl(a.type_description,'NULL') as type_description, a.measev_id, a.print_alarm, a.system_alarm,"
            << " a.isvisible, a.persist_alarm, a.alarm_value, a.normal_value, a.mms_state, TO_CHAR(a.date_modified,'YYYYMMDDHH24MISS'), TO_CHAR(a.date_created,'YYYYMMDDHH24MISS'), a.physical_alarm "
            << " from me_alarm_type a, me_message_context mc where a.memcon_id = mc.memcon_id and a.meatyp_id = " << key;

		std::vector<IAlarmTypeData*> alarmTypes;
		getAlarmTypeBySql(sql.str(), alarmTypes, readWrite);

        if ( 0 == alarmTypes.size())
        {
            std::ostringstream message;
			message << "No data found for AlarmType with key " << key;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == alarmTypes.size(), "AlarmType key unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return alarmTypes[0];
    }

    IConfigAlarmType* AlarmTypeAccessFactory::createAlarmType(
        const std::string name,
        const std::string description,
        unsigned long severityKey,
	    bool toBePrinted,
		bool toBePersisted,
		bool isSystemAlarm)
    {
        return new ConfigAlarmType(name, description, severityKey, toBePrinted, toBePersisted, isSystemAlarm);
    }

    IConfigAlarmType* AlarmTypeAccessFactory::copyAlarmType(
        const IConfigAlarmType* alarmTypeToCopy)
    {
        FUNCTION_ENTRY("copyAlarmType");

        TA_ASSERT(alarmTypeToCopy != NULL, "The alarmType to copy was NULL");
        
        const ConfigAlarmType* cast = dynamic_cast<const ConfigAlarmType*>(alarmTypeToCopy);

        TA_ASSERT(cast != NULL, "AlarmType passed could not be converted into a ConfigAlarmType");

        FUNCTION_EXIT;
        return new ConfigAlarmType(*cast);
    }


    IAlarmTypeData* AlarmTypeAccessFactory::getAlarmType(const std::string& name,const bool readWrite)
    {
		FUNCTION_ENTRY("getAlarmType( name )");

        // get a connection to the database
        IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Alarms_Cd, Read);

        // create the SQL string to retrieve the key of the Alarm type
        // this also doubles as a check to ensure an operator with the specified
        // PKEY actually exists.
		//TD17682 marc_bugfix Support mms type
		std::ostringstream sql;
        sql << " select a.meatyp_id, a.type_name, a.memcon_id, mc.context_name, nvl(a.type_description,'NULL') as type_description, a.measev_id, a.print_alarm, a.system_alarm,"
            << " a.isvisible, a.persist_alarm, a.alarm_value, a.normal_value, a.mms_state, TO_CHAR(a.date_modified,'YYYYMMDDHH24MISS'), TO_CHAR(a.date_created,'YYYYMMDDHH24MISS'), a.physical_alarm "
            << " from me_alarm_type a, me_message_context mc where a.memcon_id = mc.memcon_id and a.type_name = '"
			<< databaseConnection->escapeQueryString(name) << "'";

		std::vector<IAlarmTypeData*> alarmTypes;
		getAlarmTypeBySql(sql.str(), alarmTypes, readWrite);

        if ( 0 == alarmTypes.size())
        {
            std::ostringstream message;
			message << "No data found for AlarmType with name " << name;
            TA_THROW( DataException( message.str().c_str(), DataException::NO_VALUE, sql.str() ) );
        }

		TA_ASSERT(1 == alarmTypes.size(), "AlarmType name unique constraint violated");

        // Return the pointer. The class that recieves this pointer is responsible for deleting it.
		FUNCTION_EXIT;
        return alarmTypes[0];
    }
	

	std::vector< IAlarmTypeData*> AlarmTypeAccessFactory::getAllAlarmTypes(const bool readWrite)
	{
		FUNCTION_ENTRY("getAllAlarmTypes");

		// create the SQL string to retrieve the data of the alarm type
        // based upon the key
        std::ostringstream sql;
        sql << "select a.meatyp_id, a.type_name, a.memcon_id, mc.context_name, nvl(a.type_description,'NULL') as type_description, a.measev_id, a.print_alarm, a.system_alarm, "
            << "a.isvisible, a.persist_alarm, a.alarm_value, a.normal_value, a.mms_state, TO_CHAR(a.date_modified,'YYYYMMDDHH24MISS'), TO_CHAR(a.date_created,'YYYYMMDDHH24MISS') , a.physical_alarm "
            << "from me_alarm_type a, me_message_context mc where a.memcon_id = mc.memcon_id and a.isvisible = 1 order by a.meatyp_id asc";

		std::vector<IAlarmTypeData*> alarmTypes;
		getAlarmTypeBySql(sql.str(), alarmTypes, readWrite);
        
        // Return the Alarm types pointer. The class that recieves this is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return alarmTypes;
    }


    std::vector< IAlarmTypeData*> AlarmTypeAccessFactory::getAllAlarmTypesAndAttributes()
	{
		FUNCTION_ENTRY("getAllAlarmTypesAndAttributes");

        // create the SQL string to retrieve the key of the Alarm type
        // this also doubles as a check to ensure an operator with the specified
        // PKEY actually exists.
        std::ostringstream sql;
        sql << "select a.meatyp_id, a.type_name, a.memcon_id, mc.context_name, nvl(a.type_description,'NULL') as type_description, a.measev_id, a.print_alarm, a.system_alarm, "
            << "a.isvisible, a.persist_alarm, a.alarm_value, a.normal_value, a.mms_state, TO_CHAR(a.date_modified,'YYYYMMDDHH24MISS'), TO_CHAR(a.date_created,'YYYYMMDDHH24MISS'), a.physical_alarm "
            << "from me_alarm_type a, me_message_context mc where a.memcon_id = mc.memcon_id and a.isvisible = 1";

		std::vector<IAlarmTypeData*> alarmTypes;
		getAlarmTypeBySql(sql.str(), alarmTypes, false);

        // Return the Alarm types pointer. The class that recieves this is responsible
        // for deleting it.
		FUNCTION_EXIT;
		return alarmTypes;
	}

} //closes TA_Base_Core


