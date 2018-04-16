#ifndef ICOMPARERULE_INCLUDE
#define ICOMPARERULE_INCLUDE

#include <memory>

class ICompareRule
{
public:
    virtual bool result() = 0;
};

static std::auto_ptr<ICompareRule> NullCompareRule( NULL );

#endif