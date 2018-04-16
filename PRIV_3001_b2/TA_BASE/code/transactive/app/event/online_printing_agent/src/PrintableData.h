#if !defined(PRINTABLEDATA_H_INCLUDED)
#define PRINTABLEDATA_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/event/online_printing_agent/src/PrintableData.h $
  * @author:   Bart Golab
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Represents a set of printable items, such as events.
  *
  */

namespace TA_Base_App
{
    class PrintableData
    {
    // Operations
    public:
        PrintableData(const std::string& printer, const PrintableDataElementList& printableDataElements);
        PrintableData(const PrintableData& printableData);
        virtual ~PrintableData();

        virtual PrintableData& operator=(const PrintableData& printableData);

        /**
         * getPrinter
         *
         * Returns the name of the target printer on which to print this data.
         */
        virtual std::string getPrinter() const;

        /**
         * getOutput
         *
         * Returns printable data elements as a string formatted according to the 
         * configured column specification.
         */
        virtual std::string getOutput() const;


    // Attributes
    private:
        std::string m_printer;
        PrintableDataElementList m_printableDataElements;
    };
}

#endif // !defined(PRINTABLEDATA_H_INCLUDED)
