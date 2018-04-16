/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/trains/TrainBorne_PA/src/GenericBroadcastModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * GenericBroadcastModel contains shared functionality between broadcast models
  *
  */

#if !defined(AFX_GenericBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
#define AFX_GenericBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_

class IGenericBroadcastPage;

class GenericBroadcastModel
{
public:

    GenericBroadcastModel(IGenericBroadcastPage& page);
    virtual ~GenericBroadcastModel();

    IGenericBroadcastPage&  m_broadcastPage;

private:

    // Copy constructor not used so made private
    GenericBroadcastModel( const GenericBroadcastModel&);
    // Assignment not used so it is made private
    GenericBroadcastModel& operator=(const GenericBroadcastModel&);

};

#endif // !defined(AFX_GenericBroadcastModel_H__4EBD5CE7_C4D0_43BC_A835_518CA9141975__INCLUDED_)
