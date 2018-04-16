#pragma once
#include "DlgVideoPlayImos.h"

namespace TA_IRS_App
{
	class OccDlgVideoPlayImos : public DlgVideoPlayImos
	{
            DECLARE_DYNAMIC(OccDlgVideoPlayImos)
        public:
            OccDlgVideoPlayImos(TA_IRS_App::Monitor* local,
                            CWnd* pParent);
            virtual bool assignMonitor(TA_IRS_App::Monitor* monitor, TA_IRS_App::VideoInput* input);
            virtual ~OccDlgVideoPlayImos();
        protected:
            virtual void CheckShowOrHideWindow();
            virtual BOOL OnInitDialog();
			afx_msg void OnClose();
            DECLARE_MESSAGE_MAP()
		private:
            TA_IRS_App::Monitor* m_localMonitor;
            virtual bool assignVideoToLocal(TA_IRS_App::VideoInput* videoInput);
	};
}