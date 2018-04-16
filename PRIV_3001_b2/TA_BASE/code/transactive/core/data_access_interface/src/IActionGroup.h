/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/data_access_interface/src/IActionGroup.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * IActionGroup is an interface to an ActionGroup object. It allows the ActionGroup object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IACTIONGROUP_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IACTIONGROUP_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{

    class IActionGroup : public IItem
    {

    public:
        virtual ~IActionGroup() 
			{ 
			};

		
        /**
         * isControlType
         *
         * Returns whether or not the isControlType flag is set in the database.
         *
         * @return boolean - True if the action group is a control type. False otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isControlType() = 0;
	
    };
} //close namespace TA_Base_Core

#endif // !defined(IActionGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
