#ifndef IPARAMETERWRAPPER_INCLUDE
#define IPARAMETERWRAPPER_INCLUDE

#include <string>

class IStringEditor;

class IParameterWrapper
{
public:
    virtual std::string uniqueName() = 0;
    virtual std::string displayName() = 0;
    virtual std::string displayValue() = 0;
    virtual int type() = 0;
    virtual bool show() = 0;
    virtual bool valid() = 0;
};

#endif