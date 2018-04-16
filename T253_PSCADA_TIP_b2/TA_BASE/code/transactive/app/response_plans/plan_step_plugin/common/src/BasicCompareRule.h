#ifndef COMPARERULE_INCLUDE
#define COMPARERULE_INCLUDE

#include "CompareMethod.h"
#include "ICompareRule.h"

template<class BasicType>
class BasicCompareRule : public ICompareRule
{
public:
    BasicCompareRule( const BasicType& refSource, const CompareMethod nCompareType, const BasicType& refTarget );
    virtual ~BasicCompareRule();

    virtual bool result();

private:
    const CompareMethod m_eCompareType;
    const BasicType& m_refSource;
    const BasicType& m_refTarget;
};

template<class BasicType>
bool BasicCompareRule<BasicType>::result()
{
    bool bResult = false;

    switch ( m_eCompareType )
    {
    case eRuleEqualTo:
        bResult = (m_refSource == m_refTarget);
        break;
    case eRuleNotEqualTo:
        bResult = (m_refSource != m_refTarget);
        break;
    case eRuleNotLessThan:
        bResult = (m_refSource >= m_refTarget);
        break;
    case eRuleNotMoreThan:
        bResult = (m_refSource <= m_refTarget);
        break;
    case eRuleLessThan:
        bResult = (m_refSource < m_refTarget);
        break;
    case eRuleMoreThan:
        bResult = (m_refSource > m_refTarget);
        break;
    default:
        break;
    }

    return bResult;
}

template<class BasicType>
BasicCompareRule<BasicType>::BasicCompareRule( const BasicType& refSource, const CompareMethod nCompareType, const BasicType& refTarget ) :
m_eCompareType( nCompareType ),
m_refSource( refSource ),
m_refTarget( refTarget )
{
}

template<class BasicType>
BasicCompareRule<BasicType>::~BasicCompareRule( )
{
}

#endif
