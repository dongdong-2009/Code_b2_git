#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"

namespace TA_Base_App
{
    class ShutdownSystemStepParameter;
    class ShutdownSystemStepEditorParameter : public InteractiveParameter, public IBoolEditor
    {
    public:
        ShutdownSystemStepEditorParameter(ShutdownSystemStepParameter& refParameter);
        ~ShutdownSystemStepEditorParameter();

        // InteractiveParameter
        //virtual void defineAddonParameters();
        virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
		virtual IBoolEditor* getBoolEditor(const std::string& strName);

		// IBoolEditor
	    virtual bool updateBooleanValue( const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
	    virtual void getBooleanValue( const std::string& strName, bool& bValue, const unsigned int nRow = 0 );

    protected:
        virtual bool failureIgnorable();
        virtual bool responseRequired();
        virtual bool skippable();
        virtual bool delayable();

    private:
        ShutdownSystemStepParameter& m_refParameter;
    };

}
