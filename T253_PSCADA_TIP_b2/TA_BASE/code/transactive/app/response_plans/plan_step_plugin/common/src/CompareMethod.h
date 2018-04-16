#ifndef COMPAREMETHOD_INCLUDE
#define COMPAREMETHOD_INCLUDE

enum CompareMethod
{
    eRuleEqualTo = 0,  // opeartor == 
    eRuleNotEqualTo,   // operator !=
    eRuleNotLessThan,  // operator >=
    eRuleNotMoreThan,  // operator <=
    eRuleLessThan,     // operator <
    eRuleMoreThan,     // operator >
    eRuleIn,           // find in container
    eRuleNotIn         // find not in container
};

#endif