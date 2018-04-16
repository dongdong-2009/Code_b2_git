/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/InterlockingRuleAlias.h $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

///////////////////////////////////////////////////////////
//  InterlockingRuleAlias.h
//  Implementation of the Class InterlockingAlias
//  Created on:      28-Jan-2005 10:33:30 AM
///////////////////////////////////////////////////////////

#if !defined(InterlockingAlias_0142C933_E5CC_47f0_B38C_AD0E3D6E51E8__INCLUDED_)
#define InterlockingAlias_0142C933_E5CC_47f0_B38C_AD0E3D6E51E8__INCLUDED_

#include <string>
#include <vector>
#include "boost/shared_ptr.hpp"

namespace TA_Base_Core
{
    class IInterlockAlias;
}
namespace TA_Base_Bus
{
    class InterlockingRuleAlias;
    typedef std::vector<boost::shared_ptr<TA_Base_Bus::InterlockingRuleAlias> > InterlockingRuleAliasVector;

    /**
     * Every variable used in an expresion will have an alias. At first the alias name
     * will be the same as the variable name. In future the user will be able to
     * change the alias name.
     * 
     * The variable type will also be part of the alias. The type will allow us to be
     * able to determine how to handle the variable at run time.
     */
    class InterlockingRuleAlias
    {

    public:
        InterlockingRuleAlias();
        InterlockingRuleAlias( TA_Base_Core::IInterlockAlias& ialias);
	    virtual ~InterlockingRuleAlias();

    public:

        /**
	     * getVariableName
         * 
	     * 
	     */
	    std::string getVariableName();

        /**
	     * setVariableName
         * 
         * @param aliasName
	     * 
	     */
	    void setVariableName( const std::string& variableName );

	    /**
	     * getVariableType
         * 
	     * 
	     */
        std::string getVariableType();

        /**
	     * setVariableType
         * 
         * @param variableType
	     * 
	     */
	    void setVariableType( const std::string& variableType );

        /**
	     * getAliasName
         *
	     * 
	     */
	    std::string getAliasName();

        /**
	     * setAliasName
         * 
         * @param aliasName
	     * 
	     */
	    void setAliasName( const std::string& aliasName );

    private:
	    std::string m_aliasName;
	    std::string m_variableName;
	    /**
	     * This is used to retrieve context information
	     */
	    std::string m_variableType;

    };
} //namespace
#endif // !defined(InterlockingAlias_0142C933_E5CC_47f0_B38C_AD0E3D6E51E8__INCLUDED_)

