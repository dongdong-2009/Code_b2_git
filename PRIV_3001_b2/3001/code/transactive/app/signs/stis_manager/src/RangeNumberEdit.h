#pragma once


// CRangeNumberEdit
class CSpinButtonCtrl;

class CRangeNumberEdit : public CEdit
{
        DECLARE_DYNAMIC(CRangeNumberEdit)

    public:
		CRangeNumberEdit();
        CRangeNumberEdit(short min,
                         short max,
                         short defaultValue);
        virtual ~CRangeNumberEdit();
        void setBunndySpinCtrl(CSpinButtonCtrl* spinCtrl);
        void setInputNumberRange(short min, unsigned max);
        void focusKilled();
		void setValue(short value);
        short getValue();
		void setValueRange(short min, short max);
		void setDefaultValue(short defaultValue);
    protected:
        DECLARE_MESSAGE_MAP()
    private:
        bool isInputValid();
        CSpinButtonCtrl* m_spinCtrl;
        short m_minInputNumber;
        short m_maxInputNumber;
        short m_defaultValue;
};


