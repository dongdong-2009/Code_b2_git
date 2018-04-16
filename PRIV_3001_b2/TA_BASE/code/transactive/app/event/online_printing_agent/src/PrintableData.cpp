/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/event/online_printing_agent/src/PrintableData.cpp $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Represents a set of printable items, such as events.
  *
  */

#include "app/event/online_printing_agent/src/OnlinePrintingAgent.h"

using namespace TA_Base_App;


PrintableData::PrintableData(const std::string& printer, const PrintableDataElementList& printableDataElements) :
    m_printer(printer), m_printableDataElements(printableDataElements)
{
//    FUNCTION_ENTRY("PrintableData(const std::string& printer, const PrintableDataElementList& printableDataElements)");
//    FUNCTION_EXIT;
}


PrintableData::PrintableData(const PrintableData& printableData) :
    m_printer(printableData.m_printer), m_printableDataElements(printableData.m_printableDataElements)
{
//    FUNCTION_ENTRY("PrintableData(const PrintableData& printableData)");
//    FUNCTION_EXIT;
}


PrintableData::~PrintableData()
{
//    FUNCTION_ENTRY("~PrintableData()");
//    FUNCTION_EXIT;
}


PrintableData& PrintableData::operator=(const PrintableData& printableData)
{
//    FUNCTION_ENTRY("operator=(const PrintableData& printableData)");

    if (this != &printableData)
    {
        m_printer = printableData.m_printer;
        m_printableDataElements = printableData.m_printableDataElements;
    }

//    FUNCTION_EXIT;
    return *this;
}


std::string PrintableData::getPrinter() const
{
//    FUNCTION_ENTRY("getPrinter()");
//    FUNCTION_EXIT;
    return m_printer;
}


std::string PrintableData::getOutput() const
{
//    FUNCTION_ENTRY("getOutput()");
//    FUNCTION_EXIT;
    return PrintOutputFormatter::formatData(ConfigData::getInstance().getColumns(), m_printableDataElements);
}
