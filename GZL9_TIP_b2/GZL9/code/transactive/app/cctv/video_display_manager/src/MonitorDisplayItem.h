#pragma once
#include <string>

class CWnd;
class CLabel;

namespace TA_IRS_App
{
class Monitor;
class Camera;
class Sequence;
class VideoInput;

class MonitorDisplayItem
{

public:
    MonitorDisplayItem();
    ~MonitorDisplayItem();

    void setMonitorwnd(CWnd* monitorWnd);
    void setLable(CLabel* lable);
    void SetMonitor(Monitor * monitor);
    Monitor * getMonitor();
    CWnd* getMonitorWnd();
    CLabel* getLable();
    bool isInRect(CPoint & pt);
    int getIndex();
    void setIndex(int index);
    void setLocalMonitor(bool isLocal);
    bool isLocalMonitor();
    void setParentWnd(CWnd * parentWnd);
    bool inputIsCamera();
    bool inputIsSequence();
    Camera* getCamera();
    Sequence* getSequence();
    std::string getInputName();
private:
    CWnd* m_monitorWnd;
    CLabel* m_lable;
    Monitor* m_monitor;
    int m_index;
    bool m_localMonitor;
    CWnd* m_parentWnd;
    VideoInput * m_input;
};
}
