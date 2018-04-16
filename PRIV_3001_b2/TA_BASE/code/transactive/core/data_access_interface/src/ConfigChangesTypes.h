/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/data_access_interface/src/ConfigChangesTypes.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#ifndef CONFIG_CHANGES_TYPES_H
#define CONFIG_CHANGES_TYPES_H

#include <map>
#include <string>
       
namespace TA_Base_Core
{

    /**
    * @version 1.0
    * @created 22-Aug-2007 12:04:26 PM
    */
    struct Values
    {

    public:

        /**
        * This is the old value of the attribute that changed
        */
        std::string oldValue;


        /**
        * This is the new value of the attribute that changed
        */
        std::string newValue;

    };

    /**
    * This is a map of all changes made to an item. It is keyed on the attribute name.
    *
    * @version 1.0
    * @created 22-Aug-2007 12:04:26 PM
    */
    typedef std::map< std::string, Values> ItemChanges;

    /**
     * Not a class, used to hold static function
     * @author adamr
     * @version 1.0
     * @created 22-Aug-2007 12:04:26 PM
     */
    class ConfigChangesTypes
    {

    public:

        ConfigChangesTypes();


        virtual ~ConfigChangesTypes();

        /**
        * Updates the given set of item changes, a smart helper for managing item changes.
        *
        *
        * @param itemChanges    The item changes to update.
        * @param parameterName    The parameter name
        * @param oldValue    the old value
        * @param newValue    the new value
        */
        static void updateChanges( ItemChanges& itemChanges,
                                   const std::string& parameterName,
                                   const std::string& oldValue,
                                   const std::string& newValue );

    };

}

#endif // !defined(ConfigChangesTypes_H)

