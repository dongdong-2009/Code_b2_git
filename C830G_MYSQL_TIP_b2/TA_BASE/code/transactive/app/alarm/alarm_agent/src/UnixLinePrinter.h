/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/alarm/alarm_agent/src/UnixLinePrinter.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
#if !defined(UNIXLINEPRINTER)
#define UNIXLINEPRINTER

#include "app/alarm/alarm_agent/src/AbstractPrinter.h"
#include <stdio.h>

namespace TA_Base_App
{
    class UnixLinePrinter : public virtual AbstractPrinter
    {
    public:

        /**
          * Constructer
          *
          * @param Printer name
          * @param Printer port
          * @param Printer host name
          */
        UnixLinePrinter( const std::string& p_name, 
                         const std::string& p_port, 
                         const std::string& p_host );

        /**
          * Destructer
          */
        virtual ~UnixLinePrinter();

        /**
          * print
          *
          * @param The string to print
          */
        virtual void print( const std::string& p_stringToPrint );

    private:

        FILE* m_printer;
    };
}

#endif // !defined(UNIXLINEPRINTER)
