/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Lucky Cuizon
  * @version: $Revision: #2 $
  *
  * Last modification: $Date: 2013/26/06 $
  * Last modified by:  $Author: $
  *
  */

#ifndef _PEC_CALL_CCTV_H_
#define _PEC_CALL_CCTV_H_

#include "core/threads/src/Thread.h"


// forward declarations
namespace TA_IRS_Bus
{
    class PECCallEntry;
}


namespace TA_IRS_Bus
{
    /**
     * @author Robert van Hugten
     * @version 1.0
     * @created 21-Dec-2007 3:49:57 PM
     */

    class PECCallCCTV : public TA_Base_Core::Thread
    {

    public:

        /**
         * Constructor
         *
         * @param pecCallEntry
         */
        PECCallCCTV( PECCallEntry& pecCallEntry );


        /**
         * Destructor.
         */
        virtual ~PECCallCCTV();


    private:


        /**
         * disabled methods
         */
        PECCallCCTV();
        PECCallCCTV( const PECCallCCTV& prm1 );
        void operator=( const PECCallCCTV& prm1 );


        /**
         * Called by the start method.
         */
        void run();


        /**
         * Terminates the thread
         */
        void terminate();


        /**
         * The call to answer
         */
        PECCallEntry& m_PECCallEntry;

    };

}

#endif // !defined(PECCallCCTV_H)
