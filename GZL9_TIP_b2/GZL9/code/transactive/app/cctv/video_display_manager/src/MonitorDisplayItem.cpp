#pragma once

#include "StdAfx.h"
#include "Monitor.h"
#include "Camera.h"
#include "Sequence.h"
#include "Label.h"
#include "MonitorDisplayItem.h"


namespace TA_IRS_App
{

MonitorDisplayItem::MonitorDisplayItem() :
        m_monitorWnd(NULL),
        m_lable(NULL),
        m_monitor(NULL),
        m_index(-1),
        m_parentWnd(NULL),
        m_localMonitor(false),
        m_input(NULL)
{

}

MonitorDisplayItem::~MonitorDisplayItem()
{

}

void MonitorDisplayItem::setMonitorwnd(CWnd* monitorWnd)
{
    m_monitorWnd = monitorWnd;
}

void MonitorDisplayItem::setLable(CLabel* lable)
{
    m_lable = lable;
}

void MonitorDisplayItem::SetMonitor(Monitor * monitor)
{
    m_monitor = monitor;
}

Monitor* MonitorDisplayItem::getMonitor()
{
    return m_monitor;
}

CWnd* MonitorDisplayItem::getMonitorWnd()
{
    return m_monitorWnd;
}

CLabel* MonitorDisplayItem::getLable()
{
    return  m_lable;
}

bool MonitorDisplayItem::isInRect(CPoint & pt)
{
    CRect wndRect;;
    m_monitorWnd->GetWindowRect(&wndRect);
    m_parentWnd->ScreenToClient(&wndRect);
    return  wndRect.PtInRect(pt) == TRUE;
}

int MonitorDisplayItem::getIndex()
{
    return m_index;
}

void MonitorDisplayItem::setIndex(int index)
{
    m_index = index;
}

void MonitorDisplayItem::setLocalMonitor(bool isLocal)
{
    m_localMonitor = isLocal;
}

bool MonitorDisplayItem::isLocalMonitor()
{
    return m_localMonitor;
}

void MonitorDisplayItem::setParentWnd(CWnd * parentWnd)
{
    m_parentWnd = parentWnd;
}

bool MonitorDisplayItem::inputIsCamera()
{
    return dynamic_cast<Camera*>(m_monitor->getInput()) != NULL;
}

bool MonitorDisplayItem::inputIsSequence()
{
    return dynamic_cast<Sequence*>(m_monitor->getInput()) != NULL;
}

Camera * MonitorDisplayItem::getCamera()
{
    return dynamic_cast<Camera*>(m_monitor->getInput());
}

Sequence * MonitorDisplayItem::getSequence()
{
    return dynamic_cast<Sequence*>(m_monitor->getInput());
}

std::string MonitorDisplayItem::getInputName()
{
    VideoInput* input = m_monitor->getInput();
    return input == NULL ? "" : input->getName();
}

}
