#ifndef IDIAGRAMVIEWER_INCLUDE
#define IDIAGRAMVIEWER_INCLUDE

#include <vector>

class IDiagramViewer
{
public:
    virtual void diagramUpdated() = 0;
    virtual void elementAdded( const unsigned int unId ) = 0;
    virtual void elementDeleted( const unsigned int unId ) = 0;
    virtual void elementUpdated( const unsigned int unId ) = 0;
    virtual void elementSelected( const unsigned int unId, const std::vector<unsigned int>& vecDeselectedIds ) = 0;
    virtual void elementsSelected( const std::vector<unsigned int>& vecSelectedIds, const std::vector<unsigned int>& vecDeselectedIds ) = 0;
};

#endif