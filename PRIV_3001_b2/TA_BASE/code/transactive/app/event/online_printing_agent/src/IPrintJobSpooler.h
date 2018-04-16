#if !defined(IPRINTJOBSPOOLER_H_INCLUDED)
#define IPRINTJOBSPOOLER_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/event/online_printing_agent/src/IPrintJobSpooler.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class provides the interface to the actual printing system (e.g. lpd).
  *
  */

namespace TA_Base_App
{
    class IPrintJobSpooler
    {
    public:
        /**
         * submit
         *
         * Submits the specified data to the underlying printing system.
         * Returns true if successfully spooled, false otherwise.
         */
        virtual bool submit(const PrintableData& printableData) = 0;

        virtual ~IPrintJobSpooler() {};
    };
}

#endif // !defined(IPRINTJOBSPOOLER_H_INCLUDED)
