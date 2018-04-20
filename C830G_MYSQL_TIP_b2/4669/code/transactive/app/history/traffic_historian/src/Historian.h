/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/history/traffic_historian/src/Historian.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:
  *
  * The Historian controls the HistorianProcess.
  *
  */ 

#ifndef HISTORIAN_H
#define HISTORIAN_H

#include "GenericDriver.h"

class Historian : public GenericDriver  
{
public:
    /**
     * Historian()
     *
     * This is the standard constructor.  
     *
     */
    Historian();


    /**
     * ~Historian()
     *
     * This is the standard destructor.  
     *
     */
    virtual ~Historian();


protected:
    /**
     * CreateProcess()
     *
     * This method returns a pointer to the instance of the
     * HistorianProcess.
     *
     * @return A pointer to the instance of the HistorianProcess.
     *
     */
    virtual GenericDriverProcess* CreateProcess();
};

#endif // HISTORIAN_H
