/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/history/traffic_historian/src/Historian.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
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
