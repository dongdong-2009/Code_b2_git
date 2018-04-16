#ifndef PLANSTEPEDITORHELPER_INCLUDE_H
#define PLANSTEPEDITORHELPER_INCLUDE_H

#include <vector>
#include <map>

#include "ace/RW_Mutex.h"

struct ListItem;

namespace TA_Base_App
{

class IPlanNode;

class PlanStepEditorHelper
{
public:
    static const unsigned int INVALID_ID;
    PlanStepEditorHelper(IPlanNode& parent);
    ~PlanStepEditorHelper();
    std::vector<ListItem> getSteps();
    ListItem getItemById(unsigned int id);

private:
    void initHelper();

private:
    typedef std::map<unsigned int, ListItem> StepItemMap;
    StepItemMap m_itemMap;
    IPlanNode& m_planNode;
    ACE_RW_Mutex m_mtxItemMap;
    bool m_bInitilized;
};

};

#endif
