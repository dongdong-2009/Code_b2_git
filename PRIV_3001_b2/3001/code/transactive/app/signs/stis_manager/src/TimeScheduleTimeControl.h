#ifndef TIMESCHEDULETIMECONTROL_H
#define TIMESCHEDULETIMECONTROL_H

#include "CallbackButton.h"
#include "CallbackDateTimeCtrl.h"

#include <vector>
#include "core/data_access_interface/tis_agent_4669/IDL/src/TisMessageDataDefinitions.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"


class CRangeNumberEdit;
namespace TA_IRS_App
{
    class TimeScheduleTimeControl : public IDateTimeListener
    {

    public:
        TimeScheduleTimeControl( CComboBox& priorityCombo,
                            CallbackDateTimeCtrl& startDate,
                            CallbackDateTimeCtrl& startTime,
	                        CallbackDateTimeCtrl& endDate,
                            CallbackDateTimeCtrl& endTime,
	                        CRangeNumberEdit& repeatInterval
							);

        ~TimeScheduleTimeControl();

		virtual void Init();

        unsigned short getPriority() const;
		void setPriority(unsigned short priority);

        CTime getStartTime() const;
		void setStartTime(CTime startTime);

        CTime getEndTime() const;
		void setEndTime(CTime endTime);

        unsigned short getRepeatInterval() const;
        void setRepeatInterval( unsigned short repeatInterval );

        virtual void dateTimeChanged(CallbackDateTimeCtrl* control);

		std::string convertCTimeToString(CTime time);
		CTime convertStringToCTime(const std::string time);
		void setDefaultValue();

		unsigned short getValidMinRepeatInterval();
		unsigned short getValidMaxRepeatInterval();
    private:

        TimeScheduleTimeControl();


        // The display attribute controls
        CComboBox& m_priorityCombo;
        CallbackDateTimeCtrl& m_startDate;
        CallbackDateTimeCtrl& m_startTime;
        CallbackDateTimeCtrl& m_endDate;
	    CallbackDateTimeCtrl& m_endTime;
	    CRangeNumberEdit& m_repeatInterval;
    };

} // TA_IRS_App

#endif // !defined(TimeScheduleTimeControl_H)
