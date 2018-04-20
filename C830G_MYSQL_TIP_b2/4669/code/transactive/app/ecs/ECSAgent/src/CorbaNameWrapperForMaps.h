/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/CorbaNameWrapperForMaps.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// CorbaNameWrapperForMaps.h: interface for the CorbaNameWrapperForMaps class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CorbaNameWrapperForMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_CorbaNameWrapperForMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include "core/naming/src/NamedObject.h"

// This class is necessary because the CorbaName class does not allow the 
// default constructor.  For us to use the CorbaName in a map we must provide
// a deafult constructor and a copy constructor.  The class wraps the CorbaName
// class to provide the missing constructors and to automatically cleanup the 
// CorbaName pointer member which is allocated on the heap

class CorbaNameWrapperForMaps
{
public:
    CorbaNameWrapperForMaps(const TA_Base_Core::CorbaName& corbaName)
    : m_isDefaultConstructed(false)
    {
        m_corbaName = new TA_Base_Core::CorbaName(corbaName);
    }

    CorbaNameWrapperForMaps(const CorbaNameWrapperForMaps& copy)
    {
        m_corbaName = new TA_Base_Core::CorbaName(copy.getCorbaName());
        m_isDefaultConstructed = copy.m_isDefaultConstructed;
    }

    virtual ~CorbaNameWrapperForMaps()
    {
        if (m_corbaName)
        {
            delete m_corbaName;
            m_corbaName = NULL;
        }
    }

    CorbaNameWrapperForMaps& CorbaNameWrapperForMaps::operator= (const CorbaNameWrapperForMaps& source)
    {
        if (this == &source) return *this;   // Gracefully handle self assignment

        // Put the normal assignment duties here...
        delete m_corbaName;

        m_corbaName = new TA_Base_Core::CorbaName(source.getCorbaName());
        m_isDefaultConstructed = source.m_isDefaultConstructed;

        return *this;
    } 

    CorbaNameWrapperForMaps& CorbaNameWrapperForMaps::operator= (const TA_Base_Core::CorbaName& source)
    {
        // Put the normal assignment duties here...
        delete m_corbaName;

        m_corbaName = new TA_Base_Core::CorbaName(source);
        m_isDefaultConstructed = false;

        return *this;
    } 

    CorbaNameWrapperForMaps()
    : m_isDefaultConstructed(true)
    {
        m_corbaName = new TA_Base_Core::CorbaName("","");
    }

    bool isValid()
    { 
        return !m_isDefaultConstructed;
    }

    TA_Base_Core::CorbaName getCorbaName() const 
    { 
        return *m_corbaName;
    }

    operator TA_Base_Core::CorbaName()
    {
        return *m_corbaName;
    }

private:

    TA_Base_Core::CorbaName* m_corbaName;
    bool m_isDefaultConstructed;
};

#endif // !defined(AFX_CorbaNameWrapperForMaps_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)


