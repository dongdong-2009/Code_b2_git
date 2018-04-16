#include "StdAfx.h"
#include "ControlStationCorbaImpl.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"

ControlStationCorbaImpl::ControlStationCorbaImpl()
    : m_monitorNumber(0),
      m_monitorWidth(0),
      m_monitorHeight(0)
{
    activate();
}

TA_Base_Core::IControlStationCorbaDef::RECT ControlStationCorbaImpl::getRect(unsigned long targetScreen, unsigned long targetArea, long val)
{
    if (!m_monitorNumber || !m_monitorWidth || !m_monitorHeight)
    {
        calculateMonitor(m_monitorNumber, m_monitorWidth, m_monitorHeight);
    }

    size_t nth = 1;

    if (TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC == targetScreen)
    {
        nth = val < 1 ? 1 : val;
        nth = m_monitorNumber < val ? m_monitorNumber : val;
    }

    TA_Base_Core::IControlStationCorbaDef::RECT rect;
    rect.left         = m_monitorWidth * (nth - 1);
    rect.top          = 0;
    rect.right        = m_monitorWidth * nth;
    rect.bottom       = m_monitorHeight;
    return rect;
}

void ControlStationCorbaImpl::calculateMonitor(unsigned long& monitorNumber, unsigned long& monitorWidth, unsigned long& monitorHeight)
{
    struct Resolution
    {
        size_t width;
        size_t height;
    };

    Resolution resolutions[] =
    {
        {1024, 576},  {1024, 600},  {960, 640},   {1024, 640},  {960, 720},   {1136, 640},  {1024, 768},  {1024, 800},
        {1152, 720},  {1152, 768},  {1280, 720},  {1120, 832},  {1280, 768},  {1152, 864},  {1334, 750},  {1280, 800},
        {1152, 900},  {1024, 1024}, {1366, 768},  {1280, 854},  {1600, 768},  {1280, 960},  {1080, 1200}, {1440, 900},
        {1280, 1024}, {1440, 960},  {1600, 900},  {1400, 1050}, {1440, 1024}, {1440, 1080}, {1600, 1024}, {1680, 1050},
        {1776, 1000}, {1600, 1200}, {1600, 1280}, {1920, 1080}, {1920, 1200}, {1920, 1280}, {2048, 1152}, {1792, 1344},
        {1856, 1392}, {2880, 900},  {1800, 1440}, {2048, 1280}, {1920, 1400}, {2538, 1080}, {2560, 1080}, {1920, 1440},
        {2160, 1440}, {2048, 1536}, {2304, 1440}, {2560, 1440}, {2304, 1728}, {2560, 1600}, {2560, 1700}, {2560, 1800},
        {2560, 1920}, {3440, 1440}, {2736, 1824}, {2880, 1800}, {2560, 2048}, {2732, 2048}, {2800, 2100}, {3200, 1800},
        {3000, 2000}, {3200, 2048}, {3200, 2400}, {3840, 2160}, {3840, 2400}, {4096, 2304}, {5120, 2160}, {4096, 3072},
        {4500, 3000}, {5120, 2880}, {5120, 3200}, {5120, 4096}, {6400, 4096}, {6400, 4800}, {7680, 4320}, {7680, 4800},
        {8192, 4608}, {8192, 8192}
    };

    size_t n = sizeof(resolutions) / sizeof(Resolution);
    size_t totalWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    monitorHeight = GetSystemMetrics(SM_CYSCREEN);
    monitorNumber = GetSystemMetrics(SM_CMONITORS);
    monitorWidth = totalWidth / monitorNumber;

    if (1 == monitorNumber && 2 < totalWidth / monitorHeight)
    {
        for (size_t i = 0; i < n; ++i)
        {
            Resolution& r = resolutions[i];

            if (monitorHeight == r.height)
            {
                if (0 == totalWidth % r.width)
                {
                    monitorWidth = r.width;
                    monitorNumber = totalWidth / monitorWidth;
                    return;
                }
            }
        }

        if (TA_Base_Core::RunParams::getInstance().isSet("MonitorWidth") && TA_Base_Core::RunParams::getInstance().isSet("MonitorNumber"))
        {
            monitorNumber = TA_Base_Core::getRunParamValue("MonitorNumber", 1);
            monitorWidth = TA_Base_Core::getRunParamValue("MonitorWidth", 1920);
        }
        else
        {
            monitorNumber = totalWidth / (monitorHeight * 16 / 9);
            monitorWidth = totalWidth / monitorNumber;
        }
    }
}

void ControlStationCorbaImpl::activate()
{
    try
    {
        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "ControlStation");
    }
    catch (const CORBA::SystemException&)
    {
    }

    activateServantWithName("ControlStation");
}

void ControlStationCorbaImpl::deactivate()
{
    deactivateServant();
}
