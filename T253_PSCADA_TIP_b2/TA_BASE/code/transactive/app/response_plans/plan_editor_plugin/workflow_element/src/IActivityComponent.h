#ifndef IACTIVITYCOMPONENT_INCLUDE
#define IACTIVITYCOMPONENT_INCLUDE

#include <string>
#include <map>
#include <vector>

class WorkflowActivity;
typedef WorkflowActivity* (*CreateActivitycPtr)();

class IActivityComponent
{
public:
    virtual bool readonly() = 0;
    virtual int getStepType() = 0;
    virtual int getAccessType() = 0;
    virtual CreateActivitycPtr getActivityCreationFuncPtr() = 0;
	virtual std::vector<std::pair<std::string, std::vector<int>>> getShortcuts() = 0;
    virtual std::string getName() = 0;
    virtual std::string getResource() = 0;
    virtual std::string getCatagroyName() = 0;
};

#endif