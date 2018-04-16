

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIProtocol/src/CADIEventObserver.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  */

#ifndef CADIEVENTOBSERVER_H
#define CADIEVENTOBSERVER_H


// interface class
namespace TA_IRS_App
{
    // forward declaration
    class CADIEvent;

    class CADIEventObserver  
    {
    public:
        CADIEventObserver() {};
        virtual ~CADIEventObserver(){};

        virtual void notify(CADIEvent &e)=0;

    private:
        // disabled methods
        CADIEventObserver(const CADIEventObserver&);
        void operator=(const CADIEventObserver&);
    };
};
#endif
