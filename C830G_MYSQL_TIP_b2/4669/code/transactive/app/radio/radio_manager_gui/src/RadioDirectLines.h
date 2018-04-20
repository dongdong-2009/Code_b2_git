/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/RadioDirectLines.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// RadioSession.h: interface for the CRadioSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(RADIODIRECTLINES_H)
#define RADIODIRECTLINES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

class CRadioDirectLines
{
	public:

		CRadioDirectLines();
		~CRadioDirectLines();
		void InitEntries();

		typedef struct DirectLineEntry
		{
			std::string     label;
			std::string     number;
		};

		typedef std::vector<DirectLineEntry>   DirectLineVector;

		// Direct line services
		bool GetDirectLines(std::string name, DirectLineVector& lines);
		bool GetDirectHotLines(DirectLineVector& lines);

	protected:

        /**
         * PRE: Position is within the confies of the DirectLineVector
         */
		void AddEntry(DirectLineVector& lines,std::string name,std::string label,unsigned int position);

	private:
        struct NamedVector
        {
            DirectLineVector& vector;
            std::string name;

            NamedVector(DirectLineVector& v,const std::string& n) : vector(v),name(n)
            {};

            NamedVector& operator=(const NamedVector& rhs)
            {
                vector = rhs.vector;
                name = rhs.name;

                return *this;
            };
        };

        void InitVectors();
		// A DirectLineVector for each tab.
		DirectLineVector m_userEntries;
		DirectLineVector m_stationsEntries;
		DirectLineVector m_OCREntries;
		DirectLineVector m_DCREntries;
		DirectLineVector m_otherEntries;
        DirectLineVector m_hotLineEntries;

        static const std::string USER;
        static const std::string STATION;
        static const std::string OCR;
        static const std::string DCR;
        static const std::string OTHER;
        static const std::string HOTLINE;

};

#endif // !defined(RADIODIRECTLINES_H)
