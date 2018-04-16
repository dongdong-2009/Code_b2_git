/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/banner_pages/call_page_library/src/ICallBanner.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This interface is implemented by the CallPage class.
  * It provides a way for the CallPageLibrary to call back the Call Page
  * of the Banner Application.
  */

#if !defined(ICallBanner_A2A43A6F_9F06_4928_831B_636D77B04668__INCLUDED_)
#define ICallBanner_A2A43A6F_9F06_4928_831B_636D77B04668__INCLUDED_


#include "bus/banner_pages/call_page_library/src/ICallEntry.h"


namespace TA_IRS_Bus
{

    /**
     * @version 1.0
     * @created 11-Feb-2004 11:45:26 AM
     */
    class ICallBanner
    {

    public:

	    /**
	     * Informs the Call Banner that a call has been added.
	     */
	    virtual void addCall(const ICallEntry& call) =0;
	    
        
        /**
	     * Informs the Call Banner that a call with the given id
         * has been deleted.
	     */
	    virtual void removeCall(unsigned long callId) =0;


        /**
         * Informs the Call Banner that it should refresh its list of
         * calls.
         */
        virtual void refreshCalls() =0;

        /**
         * Informs the Call Banner that the fields of a call entry have
         * changed and it should reload them.
         */
        virtual void refreshCall(const ICallEntry& call) =0;
    };

}// TA_IRS_Bus

#endif // !defined(ICallBanner_A2A43A6F_9F06_4928_831B_636D77B04668__INCLUDED_)
