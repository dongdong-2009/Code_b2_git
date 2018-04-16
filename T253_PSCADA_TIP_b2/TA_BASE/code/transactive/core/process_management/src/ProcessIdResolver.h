/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/process_management/src/ProcessIdResolver.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * The ProcessIdResolver simply provides a static method for returning
  * a process Id. The reason it has been separated from the rest of 
  * ManagedProcess is to keep all the operating system specific stuff
  * in the same place.
  */
#if !defined(PROCESSIDRESOLVER)
#define PROCESSIDRESOLVER

namespace TA_Base_Core
{
    class ProcessIdResolver
    {
    public:
        
        /**
          * Destructor
          */
        virtual ~ProcessIdResolver(){}

        /**
          * Static method for returning the process id
          *
          * @return The process id
          *
          * @exception ManagedProcessException thrown if we fail to 
          *            return the process id
          */
        static unsigned long getProcessId();
    };
}
#endif // !defined(PROCESSIDRESOLVER)
