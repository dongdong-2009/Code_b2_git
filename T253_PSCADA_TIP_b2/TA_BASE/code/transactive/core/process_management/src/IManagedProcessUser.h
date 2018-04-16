/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/process_management/src/IManagedProcessUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last mofified by:  $Author: CM $
  *
  * This abstract interface should be implemented by any class that uses
  * a ManagedProcess
  */

#if !defined(IMANAGEDPROCESSUSER)
#define IMANAGEDPROCESSUSER

namespace TA_Base_Core
{
    class IManagedProcessUser
    {
    public:
        /**
          * Destructor
          */
        virtual ~IManagedProcessUser(){}

        /**
          * onTerminate
          *
          * This method ensures that ManagedProcess can inform
          * all ManagedProcessUsers when the they are required to
          * terminate.
          *
          * @exception TODO
          */
        virtual void onTerminate() = 0;
    };
}

#endif // !defined(IMANAGEDPROCESSUSER)
