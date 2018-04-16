/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioProtocol/MotorolaElite/ConsoleProtocol/src/IMcdbObserver.h $
  * @author:  
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * Interface for the Mcdb observer class
  *
  */

#if !defined(AFX_ICALLBACK_H__AC1577A1_CCBE_4C43_9068_E08C4273A0A1__INCLUDED_)
#define AFX_ICALLBACK_H__AC1577A1_CCBE_4C43_9068_E08C4273A0A1__INCLUDED_



class IMcdbObserver
{
public:
	virtual ~IMcdbObserver() {};

    /** 
      * notify
      *
      * method to be called when the database is ready
      */
    virtual void notify() = 0;

	virtual void reconnectMCC() = 0;
};

#endif // !defined(AFX_ICALLBACK_H__AC1577A1_CCBE_4C43_9068_E08C4273A0A1__INCLUDED_)
