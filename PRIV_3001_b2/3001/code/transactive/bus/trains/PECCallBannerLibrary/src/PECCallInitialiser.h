/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(PECCallInitialiser_H)
#define PECCallInitialiser_H


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

    class PECCallInitialiser : public TA_Base_Core::Thread
    {

    public:

        /**
         * Constructor
         *
         * @param pecCallEntry
         */
        PECCallInitialiser( PECCallEntry& pecCallEntry );


        /**
         * Destructor.
         */
        virtual ~PECCallInitialiser();


    private:


        /**
         * disabled methods
         */
        PECCallInitialiser();
        PECCallInitialiser( const PECCallInitialiser& prm1 );
        void operator=( const PECCallInitialiser& prm1 );


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

#endif // !defined(PECCallInitialiser_H)
