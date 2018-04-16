#ifndef IWORKFLOWOWNER_INCLUDE
#define IWORKFLOWOWNER_INCLUDE

class IActivityParameter;

class IWorkflowOwner
{
public:
    virtual void activityCreated( const int nType, const unsigned int unId ) = 0;
    virtual void activityDeleted( const unsigned int unId ) = 0;
    virtual void activityUpdated( const unsigned int unId ) = 0;
    virtual void activityCopied( const unsigned int unFrom, const unsigned int unTo ) = 0;
    virtual void flowCreated( const unsigned int unId ) = 0;
    virtual void flowDeleted( const unsigned int unId ) = 0;
    virtual void linkFlowFrom( const unsigned int unFlow, const unsigned int unActivity ) = 0;
    virtual void linkFlowTo( const unsigned int unFlow, const unsigned int unActivity ) = 0;

    virtual IActivityParameter& getActivityParameter( const unsigned int unId ) = 0;
};

#endif