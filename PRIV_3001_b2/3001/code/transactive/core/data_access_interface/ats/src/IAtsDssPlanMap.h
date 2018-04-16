/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/IAtsDssPlanMap.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * IAtsDssPlanMap is an interface to an AtsDssPlanMap object. It allows the AtsDssPlanMap object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IAtsDssPlanMap_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IAtsDssPlanMap_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IAtsDssPlanMap : public virtual TA_Base_Core::IItem
    {

    public:

		virtual unsigned long getDssEvent() =0;
		virtual std::string getPlanPath() =0;
        
        virtual ~IAtsDssPlanMap() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IAtsDssPlanMap*> IAtsDssPlanMaps;
    typedef IAtsDssPlanMaps::iterator    IAtsDssPlanMapsIt;
} //close namespace TA_IRS_Core

#endif // !defined(IAtsDssPlanMap_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
