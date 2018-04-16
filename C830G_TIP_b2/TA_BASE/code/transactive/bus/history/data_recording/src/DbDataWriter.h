#ifndef DB_DATA_WRITER_H
#define DB_DATA_WRITER_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/history/data_recording/src/DbDataWriter.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * IDataWriter implementation for persisting cached data to
 * a database
 *
 */

#include <ctime>
#include <string>

#include "bus/history/data_recording/src/IDataWriter.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
    class DbDataWriter : public IDataWriter
    {

    public:

        /**
         * Constructor
         *
         */
        DbDataWriter();


        /**
         * Destructor
         *
         */
        virtual ~DbDataWriter();


        /************************************************
         * 
         * IDataWriter functions
         * 
         ************************************************/

        /**
         * write 
         *
         * Writes the data within the DataCache to the database.
         * Field/table names are derived from the DataCache's
         * name and column map.
         *
         * @param   data    the DataCache to write
         *
         */
        virtual void write( DataCachePtr& data );


    private:

        /**
         * getSourceDate
         *
         * @param   timeStruct  a timestamp in struct tm format
         * @return  an SQL string for converting the timestamp
         *          to a date and time string suitable for the 
         *          DataRecording tables.
         *
         */
        std::string getSourceTime( struct tm& timeStruct ) const;


        /**
         * getTableName
         *
         * @param   typeName    entity type being recorded
         * @returns the table/view name to write data to
         *
         */
        std::string getTableName( const std::string& typeName ) const;


        /**
         * getValueAsDbString
         *
         * Converts a Datum's value into a string suitable for use
         * in an SQL UPDATE command.
         *
         * @param   datum       the Datum value to convert
         * @return  a string suitable for use in an SQL statement.
         *
         */
        std::string getValueAsDbString( const Datum& datum ) const;

		std::string getLocalDatabaseName();

		// Yanrong++ TD19620
		std::string getLocalDatabaseName( const CachedData& data );
		// ++Yanrong TD19620

		std::string m_localDatabase;
        /**
         * DataRecording Base Database Field names
         */
        static const std::string  FIELD_SOURCETIME;
        static const std::string  FIELD_ENTITYKEY;
    };
}

#endif // DB_DATA_WRITER_H
