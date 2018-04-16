

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventSubject.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#ifndef CADIEVENTSUBJECT_H
#define CADIEVENTSUBJECT_H


#include "app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEvent.h"
#include <vector>

namespace TA_IRS_App
{
    // forward declaration
    class CADIEventObserver;

    class CADIEventSubject  
    {
    public:
        CADIEventSubject();
        virtual ~CADIEventSubject();

        void deregisterObserver(const CADIEventObserver& o);
        void registerObserver(CADIEventObserver& o);

    protected:
        // disabled methods
        CADIEventSubject(const CADIEventSubject&);
        void operator=(const CADIEventSubject&);

        void notifyObservers(CADIEvent& e);
        std::vector<CADIEventObserver*> m_observers;
    };
};
#endif
