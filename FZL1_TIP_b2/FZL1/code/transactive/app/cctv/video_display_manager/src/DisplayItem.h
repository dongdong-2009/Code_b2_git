/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/DisplayItem.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This is a generic display item.
  * Display items can be inputs or outputs but this is the
  * common part of those.
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#if !defined(DISPLAYITEM_H)
#define DISPLAYITEM_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <boost/noncopyable.hpp>
#include <string>

namespace TA_IRS_App
{

	class DisplayItem : boost::noncopyable
    {
        public:

            DisplayItem(unsigned long entityKey,
                        const std::string& name,
                        const std::string& description);


            virtual ~DisplayItem();

            /**
              * getKey
              *
              * @return the entity key of this item in the database. -1 if not in database.
              */
            virtual unsigned long getKey();

            /**
              * getName
              *
              * Gets the items name (entity name).
              * this is unique.
              *
              * @return the items name
              */
            virtual std::string getName();

            /**
              * getDescription
              *
              * Gets the items description.
              *
              * @return The description of the item.
              */
            virtual std::string getDescription();

            /**
              * setDescription
              *
              * Locally updates the items description.
              *
              * @param newDescription the new description
              */
            virtual void setDescription(const std::string& newDescription);


            /**
              * getDetails
              *
              * Gets a more detailed description of the item.
              * This is formatted for printing in a text box.
              *
              * @return The detailed description.
              */
            virtual std::string getDetails();

        protected:

            TA_Base_Core::ReEntrantThreadLockable m_lock;

            unsigned long m_entityKey;  // entity key for looking up in maps etc
            std::string m_name;         // entity name
            std::string m_description;  // entity description
    };
}

#endif // !defined(DISPLAYITEM_H)

