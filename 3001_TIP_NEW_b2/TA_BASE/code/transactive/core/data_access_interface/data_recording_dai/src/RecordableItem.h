#ifndef RECORDABLE_ITEM_H
#define RECORDABLE_ITEM_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/data_recording_dai/src/RecordableItem.h $
 * @author:  Jade Welton
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * <description>
 */

#include "core/data_access_interface/data_recording_dai/src/IRecordableItem.h"

namespace TA_Base_Core
{
    class RecordableItem : public IRecordableItem
    {

    public:

        /**
         * Constructor
         *
         */
        RecordableItem( unsigned long id );


        /**
         * Constructor
         *
         */
        RecordableItem( unsigned long id, unsigned long type, bool recordingEnabled );


        /**
         * Virtual Destructor
         *
         */
        virtual ~RecordableItem();

        
        /**
         * getTypeKey
         *
         * @return  the type key for this RecordableItem
         *
         */
        virtual unsigned long getTypeKey();


        /**
         * getId
         *
         * @return  the id for this RecordableItem
         *
         */
        virtual unsigned long getId();


        /**
         * isRecordingEnabled
         *
         * @return whether or not recording is enabled for the item
         *
         */
        virtual bool isRecordingEnabled();


        /**
         * invalidate
         *
         * Marks data values as potentially redundant and requiring
         * a refresh from the database.
         *
         */
        virtual void invalidate();


    private:

        /**
         * Prevent copying
         */
        RecordableItem( const RecordableItem& copyMe );
        RecordableItem& operator=( const RecordableItem& copyMe );


        /**
         * reload
         *
         */
        void reload();


        unsigned long   m_id;
        unsigned long   m_typeKey;
        bool            m_enabled;
        bool            m_isValid;

    };
}

#endif // IRECORDABLE_ITEM_H
