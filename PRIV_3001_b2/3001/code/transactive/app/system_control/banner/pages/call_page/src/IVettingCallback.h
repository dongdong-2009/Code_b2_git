/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/system_control/banner/pages/call_page/src/IVettingCallback.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Interface to allow a class to be notified when
  * the vetting state is changed.
  *
  */

#if !defined(IVETTINGCALLBACK_H)
#define IVETTINGCALLBACK_H


namespace TA_IRS_App
{

    class IVettingCallback
    {

    public:

        /** 
          * vettingStateChanged
          *
          * Indicates that the vetting state has changed.
          *
          * @param isVetting  True if vetting is now required, and false otherwise.
          */
        virtual void vettingStateChanged(bool isVetting) = 0;


        /**
          *  The destructor.
          */
        virtual ~IVettingCallback() {}

    };

} // TA_IRS_App


#endif // !defined(IVETTINGCALLBACK_H)