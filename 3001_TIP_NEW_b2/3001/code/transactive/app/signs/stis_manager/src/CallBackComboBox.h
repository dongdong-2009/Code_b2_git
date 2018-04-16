#pragma once
namespace TA_IRS_App
{
	class CallBackComboBox;
	class IComboBoxListener
	{
    public:
        virtual void selectedChanged(CallBackComboBox* control) = 0;
	};

    class CallBackComboBox : public CComboBox
    {

    public:
	    CallBackComboBox();
        virtual ~CallBackComboBox();

        void setCallback(IComboBoxListener* listener);

    public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CallBackComboBox)
	    //}}AFX_VIRTUAL

	    // Generated message map functions
    protected:
		virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

    private:
        IComboBoxListener* m_listener;

    };

} // TA_IRS_App
