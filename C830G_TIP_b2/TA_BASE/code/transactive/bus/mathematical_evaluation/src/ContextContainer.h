#ifndef M_Eval_ContextContainer_H
#define M_Eval_ContextContainer_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mathematical_evaluation/src/ContextContainer.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#include <vector>

namespace TA_Base_Bus
{
    class IBooleanContext;      // Forward declaration
    class IRealNumberContext;   // Forward declaration
    class IStringContext;       // Forward declaration

    class ContextContainer
    {
    public:

        /**
          * Constructor
          */
        ContextContainer();

        /**
          * Destructor
          */
        virtual ~ContextContainer();

        /**
          * addBooleanContext
          *
          * NOTE: The lifetimes of contexts need to be managed.  The context must exist for the lifetime of the
          *       ContextContainer.
          *
          * @param Boolean Context
          */
        void addBooleanContext( const TA_Base_Bus::IBooleanContext* p_context );

        /**
          * addRealNumberContext
          *
          * NOTE: The lifetimes of contexts need to be managed.  The context must exist for the lifetime of the
          *       ContextContainer.
          *
          * @param Real Number Context
          */
        void addRealNumberContext( const TA_Base_Bus::IRealNumberContext* p_context );

        /**
          * addStringContext
          *
          * NOTE: The lifetimes of contexts need to be managed.  The context must exist for the lifetime of the
          *       ContextContainer.
          *
          * @param String Context
          */
        void addStringContext( const TA_Base_Bus::IStringContext* p_context );

        /**
          * getBooleanValue
          *
          * @exception MathematicalEvaluationException - if variable cannot be found
          */
        bool getBooleanValue(const std::string& name) const;

        /**
          * getRealNumberValue
          *
          * @exception MathematicalEvaluationException - if variable cannot be found
          */
        double getRealNumberValue(const std::string& name) const;

        /**
          * getStringValue
          *
          * @exception MathematicalEvaluationException - if variable cannot be found
          */
        std::string getStringValue(const std::string& name) const;

        /**
          * isBooleanVariable
          *
          * @param A variable to test
          *
          * @return TRUE if the variable can be resolved using the m_booleanContexts.
          */
        bool isBooleanVariable(const std::string& name) const;

        /**
          * isRealNumberVariable
          *
          * @param A variable to test
          *
          * @return TRUE if the variable can be resolved using the m_realNumberContexts.
          */
        bool isRealNumberVariable(const std::string& name) const;

        /**
          * isStringVariable
          *
          * @param A variable to test
          *
          * @return TRUE if the variable can be resolved using the m_stringContexts.
          */
        bool isStringVariable(const std::string& name) const;

    private:

        // Disable copy constructor and assignment operator.
        ContextContainer( const ContextContainer& theContextContainer);
        ContextContainer& operator=(const ContextContainer&);

        std::vector<const TA_Base_Bus::IBooleanContext*> m_booleanContexts;
        std::vector<const TA_Base_Bus::IRealNumberContext*> m_realNumberContexts;
        std::vector<const TA_Base_Bus::IStringContext*> m_stringContexts;
    };
}

#endif // M_Eval_ContextContainer_H
