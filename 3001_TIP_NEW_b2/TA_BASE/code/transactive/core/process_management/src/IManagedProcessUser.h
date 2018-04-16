/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/process_management/src/IManagedProcessUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
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
