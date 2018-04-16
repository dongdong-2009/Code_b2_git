/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/data_access_interface/sound_manager/src/IConfigSoundCategory.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This interface contains methods that provide read and
  * write access to an entry in the so_sound_category table.
  *
  */

#ifndef ICONFIGSOUNDCATEGORY_H
#define ICONFIGSOUNDCATEGORY_H

#include "core/data_access_interface/src/IConfigItem.h"
#include <string>

namespace TA_Base_Core
{
    class IConfigSoundFile : virtual public ISoundCategory, virtual public IConfigItem
    {

    public:

        /** 
          * setCategoryKey
          *
          * Sets the pkey of this category.
          *
          * @param pkey  The primary key of the sound category.
          */
		virtual void setCategoryKey(unsigned long pkey) = 0;


        /** 
          * setName
          *
          * Sets the name of this category.
          *
          * @param name  The sound category's name.
          */
		virtual void setName(std::string name) = 0;


        /** 
          * setPosition
          *
          * Sets the sound category's position in the sequence of sounds.
		  * Used to specify the order in which the categories are played.
          *
          * @param position  The sound category's position.
          */
		virtual void setPosition(unsigned long position) = 0;

    };
    
} // namespace TA_Base_Core

#endif // ICONFIGSOUNDCATEGORY_H
