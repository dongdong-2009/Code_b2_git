/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/GenericBroadcastModel.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
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
