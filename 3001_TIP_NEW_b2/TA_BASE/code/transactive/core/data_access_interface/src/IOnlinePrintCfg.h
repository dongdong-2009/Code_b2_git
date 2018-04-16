/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/src/IOnlinePrintCfg.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 14:06:39 $
  * Last modified by: $Author: huirong.luo $
  * 
  * IOnlinePrintCfg is an interface to an OnlinePrintCfg object. It allows the OnlinePrintCfg object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IOnlinePrintCfg_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IOnlinePrintCfg_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
    class IOnlinePrintCfg : public virtual IItem
    {

    public:

        virtual std::string getDefaultPrinter() =0;
        virtual unsigned long getLocation() =0;
        virtual std::string getLocationName() =0;

        virtual ~IOnlinePrintCfg() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IOnlinePrintCfg*> IOnlinePrintCfgs;
    typedef IOnlinePrintCfgs::iterator    IOnlinePrintCfgsIt;
} //close namespace TA_IRS_Core

#endif // !defined(IOnlinePrintCfg_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
