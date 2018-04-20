#if !defined(ONLINEPRINTINGAGENTTYPEDEFS_H_INCLUDED)
#define ONLINEPRINTINGAGENTTYPEDEFS_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/event/online_printing_agent/src/OnlinePrintingAgentTypeDefs.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Common type definitions.
  *
  */

namespace TA_Base_App
{
    typedef unsigned long EventSequenceNumber;
    typedef std::vector<std::string> EntityParameterList;

    typedef std::map<std::string, std::string> PrintableDataElement;       // map column name to value string
    typedef std::vector<PrintableDataElement> PrintableDataElementList;

    enum EJustification { LEFT, RIGHT };

    typedef struct 
    {
        std::string name;
        unsigned short width;
        std::string separator;
        EJustification justification;
    } Column;

    typedef std::vector<Column> ColumnList;
}

#endif // !defined(ONLINEPRINTINGAGENTTYPEDEFS_H_INCLUDED)
