/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/AppLauncher.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This singleton contains the methods used to launch applications through the Control
  * Station and load displays into GraphWorX.
  *
  */

#include "bus/generic_gui/src/AppLauncher.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{
    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::RunParams;
    using TA_Base_Core::IControlStationCorbaDef;
    using namespace TA_Base_Bus::TA_GenericGui;

    AppLauncher* AppLauncher::s_instance = NULL;
    TA_Base_Core::ReEntrantThreadLockable AppLauncher::m_singletonLock;
    const unsigned int AppLauncher::HORIZONTAL_ALIGNMENTS = ALIGN_LEFT | ALIGN_VERTICAL | ALIGN_RIGHT;
    const unsigned int AppLauncher::VERTICAL_ALIGNMENTS = ALIGN_TOP | ALIGN_HORIZONTAL | ALIGN_BOTTOM;
    const unsigned int AppLauncher::DEFAULT_HORIZONTAL_ALIGNMENT = ALIGN_LEFT;
    const unsigned int AppLauncher::DEFAULT_VERTICAL_ALIGNMENT = ALIGN_TOP;

    AppLauncher::AppLauncher() : m_controlStation(NULL)
    {
    }


    AppLauncher::~AppLauncher()
    {
    }


    AppLauncher& AppLauncher::getInstance()
    {
        // protect the instance check / creation
        TA_Base_Core::ThreadGuard guard(m_singletonLock);

        if( s_instance == NULL )
        {
            s_instance = new AppLauncher();
        }
        return *s_instance;

    }


    RECT AppLauncher::getRect(  const EScreen targetScreen, 
                                const EArea targetArea, 
                                const int val /*= -1*/)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getRect");
        RECT retval;

        if (CORBA::is_nil(m_controlStation))
        {
            findControlStation();
        }
        try
        {
            TA_Base_Core::IControlStationCorbaDef::RECT tmp = m_controlStation->getRect(targetScreen, targetArea, val);

            retval.left = tmp.left;
            retval.right = tmp.right;
            retval.top = tmp.top;
            retval.bottom = tmp.bottom;
        }
        catch ( const IControlStationCorbaDef::ApplicationException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "IControlStationCorbaDef::ApplicationException", "Application could not be launched. Converting into a TransActive ApplicationException");
            std::ostringstream errorMsg;
            errorMsg << "Could not get rect with targetScreen = " << targetScreen << ", targetArea = " << targetArea << ", val = " << val;
            TA_THROW( TA_Base_Core::ApplicationException(errorMsg.str().c_str()) );
        }
		catch (const CORBA::Exception& ex)
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
            "Caught a CORBA exception, throwing an ApplicationException in its place");

            std::ostringstream errorMsg;
            errorMsg << "Could not get rect with targetScreen = " << targetScreen << ", targetArea = " << targetArea << ", val = " << val;
            TA_THROW( TA_Base_Core::ApplicationException(errorMsg.str().c_str()) );
		}

        return retval;
    }
        

    void AppLauncher::launchApplication(const unsigned long appType, 
                           const std::string& extraCommandLine, 
                           const unsigned long posFlag /*= 0*/,
                           const unsigned long alignFlag /*= 0*/,
                           const RECT* objectDim /*= NULL*/,
                           const RECT* boundaryDim /*= NULL*/)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "launchApplication");

        if (CORBA::is_nil(m_controlStation))
        {
            findControlStation();
        }

        try
        {
            TA_Base_Core::IControlStationCorbaDef::RECT objectRect;
            if (objectDim != NULL)
            {
                objectRect.left = objectDim->left;
                objectRect.right = objectDim->right;
                objectRect.top = objectDim->top;
                objectRect.bottom = objectDim->bottom;
            }
            else
            {
                objectRect.left = DEFAULT_RECT.left;
                objectRect.right = DEFAULT_RECT.right;
                objectRect.top = DEFAULT_RECT.top;
                objectRect.bottom = DEFAULT_RECT.bottom;
            }

            TA_Base_Core::IControlStationCorbaDef::RECT boundaryRect;
            if (boundaryDim != NULL)
            {
                boundaryRect.left = boundaryDim->left;
                boundaryRect.right = boundaryDim->right;
                boundaryRect.top = boundaryDim->top;
                boundaryRect.bottom = boundaryDim->bottom;
            }
            else
            {
                boundaryRect.left = DEFAULT_RECT.left;
                boundaryRect.right = DEFAULT_RECT.right;
                boundaryRect.top = DEFAULT_RECT.top;
                boundaryRect.bottom = DEFAULT_RECT.bottom;
            }

            m_controlStation->launchAnApplication(appType, extraCommandLine.c_str(), posFlag, alignFlag, objectRect, boundaryRect);
        }
        catch ( const IControlStationCorbaDef::ApplicationException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "IControlStationCorbaDef::ApplicationException", "Application could not be launched. Converting into a TransActive ApplicationException");
            std::ostringstream errorMsg;
            errorMsg << "Could not launch the application with appType " << appType;
            TA_THROW( TA_Base_Core::ApplicationException(errorMsg.str().c_str()) );
        }
		catch (const CORBA::Exception& ex)
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
            "Caught a CORBA exception, throwing an ApplicationException in its place");

            std::ostringstream errorMsg;
            errorMsg << "Could not launch the application with appType " << appType;
            TA_THROW( TA_Base_Core::ApplicationException(errorMsg.str().c_str()) );
		}

        LOG( SourceInfo, DebugUtil::FunctionExit, "launchApplication");
    }


    void AppLauncher::launchGraphWorxDisplay(const std::string& display, const std::string& asset /*=""*/, const long leftPosition /*=0*/)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "launchGraphWorxDisplay");

        TA_ASSERT( !display.empty(), "Display to launch was an empty string" );

        if (CORBA::is_nil(m_controlStation))
        {
            findControlStation();
        }

        try
        {
            m_controlStation->launchDisplay(display.c_str(), asset.c_str(), leftPosition);
        }
        catch ( IControlStationCorbaDef::ApplicationException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "IControlStationCorbaDef::ApplicationException", "Display could not be loaded. Converting into a TransActive ApplicationException");
            std::ostringstream errorMsg;
            errorMsg << "Could not load the display " << display << "for application in position " << leftPosition << " with asset " << asset.c_str();
            TA_THROW( TA_Base_Core::ApplicationException(errorMsg.str().c_str()) );
        }
		catch (const CORBA::Exception& ex)
		{
            // We are not expecting to catch anything other than ApplicationExceptions
            // If any unexpected exceptions are raised (eg CORBA::Exception), we re-throw
            // them as ApplicationExceptions.
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
            "Caught a CORBA exception, throwing an ApplicationException in its place" );
            std::ostringstream errorMsg;
            errorMsg << "Could not load the display " << display << "for application in position " << leftPosition;
            TA_THROW( TA_Base_Core::ApplicationException(errorMsg.str().c_str()) );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "launchGraphWorxDisplay");
    }


    void AppLauncher::findControlStation()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "findControlStation");
        try
        {
            // This should have been set by ManagedProcess.
            std::string ior = RunParams::getInstance().get("ProcessManagerIor");
            if ( ior.empty() )
            {
                TA_THROW( TA_Base_Core::ApplicationException("Could not resolve the Control Station to launch as the ProcessManagerIor in RunParams was empty") );
            }

            // Change (eg) corbaloc::localhost:6666/ProcessManager
            // to          corbaloc::localhost:6666/ControlStation
            // This will give us the part of the Control Station that allows
            // us to launch applications.
            std::string::size_type position = ior.find("ProcessManager");
            ior = ior.substr(0,position);
            ior += "ControlStation";

            CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject(ior);

            if (!CORBA::is_nil(obj))
            {
                m_controlStation = IControlStationCorbaDef::_narrow(obj);

                if (CORBA::is_nil(m_controlStation))
                {
                    TA_THROW( TA_Base_Core::ApplicationException("Could not narrow the retrieved object to the Control Station. Could not launch the application") );
                }
            }
            else
            {
                TA_THROW( TA_Base_Core::ApplicationException("The retrieved CORBA object is nil.  Could not launch the application") );
            }
        }
		catch (const CORBA::Exception& ex)
		{
            // We are not expecting to catch anything other than ApplicationExceptions
            // If any unexpected exceptions are raised (eg CORBA::Exception), we re-throw
            // them as ApplicationExceptions.
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(),
            "Caught a CORBA exception, throwing an ApplicationException in its place" );
            TA_THROW( TA_Base_Core::ApplicationException("Could not locate Control Station while trying to launch.") );
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "findControlStation");
    }


    bool AppLauncher::calculateNewCoordinates(RECT& appDim,
                                               const RECT& objectDim,
                                               const RECT& boundaryDim,
                                               const unsigned long posFlag,
                                               const unsigned long alignFlag)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateNewCoordinates");

        if (posFlag == POS_NONE)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "No positioning flag defined, so not moving it. Returning.");
            return false;
        }

        bool isResizable = ((posFlag & POS_RESIZABLE) != 0);
        bool isBounded = ((posFlag & POS_BOUNDED) != 0);
        bool isNoOverlap = ((posFlag & POS_NOOVERLAP) != 0);
        bool isRelative = ((posFlag & POS_RELATIVE) != 0);
        bool isAlignFit = ((alignFlag & TA_Base_Bus::TA_GenericGui::ALIGN_FIT) != 0);
        unsigned long vAlign, hAlign;

        // if the application rect is invalid and it's not requested to be fitted within the boundary
        // then we can't reposition it.
        if (isInvalidRect(appDim) && !(isBounded && isAlignFit))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Application dimension is invalid so cannot be reposited.  Returning");
            return false;
        }

        // if it is relative or overlap but the caller has not specified the object dimension, then don't reposition it
        if ((isRelative || isNoOverlap) && isInvalidRect(objectDim))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Change positioning flag to not relative/nooverlap as the object rect is not defined or is invalid");

            isRelative = false;
            isNoOverlap = false;
            // if it is not overlap and not relative (as the rect wasn't defined) and it's also not 
            // bounded, then we can't do anything
            if (!isBounded)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "No positioning as flags and rects are not defined properly.  Returning");
                return false;
            }
        }

        // if it's bounded but caller has not specified the boundary dimension, then don't reposition it
        if (isBounded && isInvalidRect(boundaryDim))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Change positioning flag to not bounded as the boundary rect is not defined or is invalid");
            isBounded = false;
        }

        // Check to see if we have at least one of POS_BOUNDED, POS_NOOVERLAP or POS_RELATIVE defined
        // Doesn't work if we only have POS_RESIZE as that is meaningless without the others.
        if (!(isBounded || isNoOverlap || isRelative))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "No positioning as flags or rects are not defined properly.  Returning");
            return false;
        }

        // See if we got a combination of ALIGN_LEFT, ALIGN_VERTICAL and ALIGN_RIGHT, use the default
        hAlign = HORIZONTAL_ALIGNMENTS & alignFlag; //separate out the flags
        if (!((hAlign == ALIGN_LEFT) || (hAlign == ALIGN_VERTICAL) || (hAlign == ALIGN_RIGHT)))
        {
            hAlign = DEFAULT_HORIZONTAL_ALIGNMENT;
        }

        // See if we got a combination of ALIGN_TOP, ALIGN_HORIZONTAL and ALIGN_BOTTOM, use the default
        vAlign = VERTICAL_ALIGNMENTS & alignFlag; //separate out the flags
        if (!((vAlign == ALIGN_TOP) || (vAlign == ALIGN_HORIZONTAL) || (vAlign == ALIGN_BOTTOM)))
        {
            vAlign = DEFAULT_VERTICAL_ALIGNMENT;
        }

        // if we're doing no overlap, but caller wants to centre the app at where it shouldn't go
        // then just use the default alignment
        if (isNoOverlap && (vAlign == ALIGN_HORIZONTAL) && (hAlign == ALIGN_VERTICAL))
        {
            hAlign = DEFAULT_HORIZONTAL_ALIGNMENT;
            vAlign = DEFAULT_VERTICAL_ALIGNMENT;
        }

        // if it is supposed to be bounded (without being relative/nooverlap to any object) or if it is 
        // to fit within the boundary, then calculate it here.  If it is flagged to be fitted, it will
        // ignore the relative/nooverlap flags.
        if (isBounded && (!(isNoOverlap || isRelative) || isAlignFit))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Calculating application coordinates bounded within the boundary");
            if (alignFlag & TA_Base_Bus::TA_GenericGui::ALIGN_FIT)
            {
                appDim = boundaryDim;
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                    "Application is fitted within the boundary");
                return true;
            }

            // it's bounded but not fitted, so need to calculate the new app dimension to fit within the boundary
            calculateBoundedRect(appDim, boundaryDim, hAlign, vAlign, isResizable);
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Application is bounded within the boundary");
            return true;
        }

        // otherwise it is (isNoOverlap || isRelative)
        if (isBounded)
        {
            // bounded and no overlap
            if (isNoOverlap)
            {
                calculateNoOverlapBoundedRect(appDim, objectDim, boundaryDim, hAlign, vAlign, isResizable);
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                    "Application is relative but does not overlap the object and bounded within the boundary");
                return true;
            }

            // bounded and relative
            calculateRelativeBoundedRect(appDim, objectDim, boundaryDim, hAlign, vAlign, isResizable);
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Application is relative to the object and bounded within the boundary");
            return true;
        }

        // no overlap only
        if (isNoOverlap)
        {
            calculateNoOverlapRect(appDim, objectDim, vAlign, hAlign);
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
                "Application is relative but does not overlap the object and is not bounded");
            return true;
        }

        // relative only
        calculateRelativeRect(appDim, objectDim, hAlign, vAlign);
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "Application is relative to the object the object and is not bounded");
        return true;
    }


    void AppLauncher::calculateRelativeRect(RECT& appDim,
                                            const RECT& objectDim,
                                            const unsigned long hAlign, 
                                            const unsigned long vAlign)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateRelativeRect");
        calculateRelativeCoordinates(appDim, objectDim, vAlign);
        calculateRelativeCoordinates(appDim, objectDim, hAlign);
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateRelativeRect");
    }


    void AppLauncher::calculateNoOverlapRect(   RECT& appDim,
                                                const RECT& objectDim,
                                                const unsigned long vAlign,
                                                const unsigned long hAlign)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateNoOverlapRect");
        long appWidth = appDim.right - appDim.left;
        long appHeight = appDim.bottom - appDim.top;

        if (hAlign == ALIGN_VERTICAL)
        {
            appDim.left = objectDim.left + (objectDim.right - objectDim.left - appWidth)/2;
            appDim.right = appDim.left + appWidth;

            switch(vAlign)
            {
                case ALIGN_BOTTOM:
                    appDim.bottom = objectDim.top;
                    appDim.top = appDim.bottom - appHeight;
                    break;
                case ALIGN_TOP:
                case ALIGN_HORIZONTAL:
                default:
                    appDim.top = objectDim.bottom;
                    appDim.bottom = appDim.top + appHeight;
                    break;
            }
            LOG( SourceInfo, DebugUtil::FunctionExit, "calculateNoOverlapRect");
            return;
        }

        if (hAlign == ALIGN_LEFT)
        {
            appDim.left = objectDim.right;
            appDim.right = appDim.left + appWidth;
        }
        else // hAlign == ALIGN_RIGHT)
        {
            appDim.right = objectDim.left;
            appDim.left = appDim.right - appWidth;
        }

        switch (vAlign)
        {
        case ALIGN_HORIZONTAL:
            appDim.top = objectDim.top + (objectDim.bottom - objectDim.top - appHeight)/2;
            appDim.bottom = appDim.top + appHeight;
            break;
        case ALIGN_BOTTOM:
            appDim.bottom = objectDim.bottom;
            appDim.top = appDim.bottom - appHeight;
            break;
        case ALIGN_TOP:
        default:
            appDim.top = objectDim.top;
            appDim.bottom = appDim.top + appHeight;
            break;
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateNoOverlapRect");
    }


    void AppLauncher::calculateBoundedRect( RECT& appDim,
                                            const RECT& boundaryDim,
                                            const unsigned long hAlign,
                                            const unsigned long vAlign,
                                            const bool isResizable)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateBoundedRect");
        calculateBoundedCoordinates(appDim, boundaryDim, hAlign, isResizable);
        calculateBoundedCoordinates(appDim, boundaryDim, vAlign, isResizable);
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateBoundedRect");
    }


    void AppLauncher::calculateRelativeBoundedRect( RECT& appDim,
                                                    const RECT& objectDim,
                                                    const RECT& boundaryDim,
                                                    const unsigned long hAlign, 
                                                    const unsigned long vAlign,
                                                    const bool isResizable)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateRelativeBoundedRect");
        calculateRelativeRect(appDim, objectDim, hAlign, vAlign);
        boundAppRect(appDim, boundaryDim, isResizable);
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateRelativeBoundedRect");
    }


    void AppLauncher::calculateNoOverlapBoundedRect(RECT& appDim,
                                                   const RECT& objectDim,
                                                   const RECT& boundaryDim,
                                                   const unsigned long hAlign,
                                                   const unsigned long vAlign,
                                                   const bool isResizable)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateNoOverlapBoundedRect");

        // if it is resizable, then just calculate the new applicaiton coordinates
        // as though it is not bounded by the boundary, then cut the parts that falls
        // out of the boundary
        if (isResizable)
        {
            calculateNoOverlapRect(appDim, objectDim, vAlign, hAlign);
            boundAppRect(appDim, boundaryDim, isResizable);
            return;
        }

        // it's not resizable:
        // we want to position the application window within the boundary as best as
        // possible.  If it is not resizable and it falls outside the boundary, then we 
        // need to find another position to fit the app.  If the app couldn't fit into the
        // boundary without intersecting the object (i.e. after trying out all four positions)
        // then the application will have to overlap as much of the object as possible to keep
        // the application within the boundary.

        long objectWidth = objectDim.right - objectDim.left;
        long objectHeight = objectDim.bottom - objectDim.top;
        RECT tmpAppDim = appDim;
        bool fitAcross;

        // Attempt 1: calculate the new coordinates - We try to align it horizontally first
        fitAcross = calculateNoOverlapBoundedCoordinates(appDim, objectDim, boundaryDim, hAlign, true);
        if (fitAcross)
        {
            calculateNoOverlapBoundedCoordinates(appDim, objectDim, boundaryDim, vAlign, false);
            return;
        }

        // Attempt 2: It doesn't fit within the boundary, see if we could fit it in the next position clockwise
        appDim = tmpAppDim; // reset to original value
        fitAcross = calculateNoOverlapBoundedCoordinates(appDim, objectDim, boundaryDim, vAlign, true);
        if (fitAcross)
        {
            calculateNoOverlapBoundedCoordinates(appDim, objectDim, boundaryDim, hAlign, false);
            return;
        }

        // Attempt 3: It doesn't fit within the boundary, see if we could fit it in the next position clockwise
        appDim = tmpAppDim; // reset to original value
        fitAcross = calculateNoOverlapBoundedCoordinates(appDim, objectDim, boundaryDim, 
            ((hAlign == ALIGN_LEFT) ? ALIGN_RIGHT : ALIGN_LEFT), true);
        if (fitAcross)
        {
            calculateNoOverlapBoundedCoordinates(appDim, objectDim, boundaryDim, vAlign, false);
            return;
        }

        // Attempt 4: it doesn't fit within the boundary, see if we could fit it in the next position clockwise
        appDim = tmpAppDim; // reset to original value
        fitAcross = calculateNoOverlapBoundedCoordinates(appDim, objectDim, boundaryDim, 
            ((vAlign == ALIGN_TOP) ? ALIGN_BOTTOM : ALIGN_TOP), true);
        if (fitAcross)
        {
            calculateNoOverlapBoundedCoordinates(appDim, objectDim, boundaryDim, hAlign, false);
            return;
        }

        // Attempt 5: It still couldn't fit into the boundary without intersecting the object, so we have no other
        // choice but to overlap the object.  So, just act as though we're positioning it as close to the border as possible
        appDim = tmpAppDim; // reset to original value
        calculateRelativeBoundedRect(appDim, objectDim, boundaryDim, hAlign, vAlign, false);

        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateNoOverlapBoundedRect");
    }


    void AppLauncher::calculateRelativeCoordinates( RECT& appDim,
                                                    const RECT& objectDim,
                                                    const unsigned long align)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateRelativeCoordinates");

        bool isHAlign = ((align & HORIZONTAL_ALIGNMENTS) != 0);
        long& appDim1 = (isHAlign) ? appDim.left : appDim.top;
        long& appDim2 = (isHAlign) ? appDim.right : appDim.bottom;
        long objectDim1 = (isHAlign) ? objectDim.left : objectDim.top;
        long objectDim2 = (isHAlign) ? objectDim.right : objectDim.bottom;
        long appLength = appDim2 - appDim1;
        long offset = (objectDim2 - objectDim1)/2;

        switch (align)
        {
        case ALIGN_LEFT:
        case ALIGN_TOP:
        default:
            appDim1 = objectDim1 + offset;
            appDim2 = appDim1 + appLength;
            break;
        case ALIGN_VERTICAL:
        case ALIGN_HORIZONTAL:
            appDim1 = objectDim1 + offset - (appLength/2);
            appDim2 = appDim1 + appLength;
            break;
        case ALIGN_RIGHT:
        case ALIGN_BOTTOM:
            appDim2 = objectDim2 - offset;
            appDim1 = appDim2 - appLength;
            break;
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateRelativeCoordinates");
    }


    void AppLauncher::calculateBoundedCoordinates( RECT& appDim,
                                                    const RECT& boundaryDim,
                                                    const unsigned long align,
                                                    const bool isResizable)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateBoundedCoordinates");

        bool isHAlign = ((align & HORIZONTAL_ALIGNMENTS) != 0);
        long& appDim1 = (isHAlign) ? appDim.left : appDim.top;
        long& appDim2 = (isHAlign) ? appDim.right : appDim.bottom;
        long boundaryDim1 = (isHAlign) ? boundaryDim.left : boundaryDim.top;
        long boundaryDim2 = (isHAlign) ? boundaryDim.right : boundaryDim.bottom;
        long appLength = appDim2 - appDim1;
        long boundaryLength = boundaryDim2 - boundaryDim1;

        if (isResizable && (appLength > boundaryLength))
        {
            appLength = boundaryLength;
        }

        switch (align)
        {
        case ALIGN_VERTICAL:
        case ALIGN_HORIZONTAL:
            appDim1 = boundaryDim1 + ( (boundaryLength - appLength)/2 );
            appDim2 = appDim1 + appLength;
            break;
        case ALIGN_RIGHT:
        case ALIGN_BOTTOM:
            appDim2 = boundaryDim2;
            appDim1 = appDim2 - appLength;
            break;
        case ALIGN_LEFT:
        case ALIGN_TOP:
        default:
            appDim1 = boundaryDim1;
            appDim2 = appDim1 + appLength;
            break;
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateBoundedCoordinates");
    }


    bool AppLauncher::calculateNoOverlapBoundedCoordinates(RECT& appDim,
                                                           const RECT& objectDim,
                                                           const RECT& boundaryDim,
                                                           unsigned long align,
                                                           const bool needOffset)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "calculateNoOverlapBoundedCoordinates");

        bool isHAlign = ((align & HORIZONTAL_ALIGNMENTS) != 0);
        long objectDim1 = (isHAlign) ? objectDim.left : objectDim.top;
        long objectDim2 = (isHAlign) ? objectDim.right : objectDim.bottom;
        long boundaryDim1 = (isHAlign) ? boundaryDim.left : boundaryDim.top;
        long boundaryDim2 = (isHAlign) ? boundaryDim.right : boundaryDim.bottom;
        long& appDim1 = (isHAlign) ? appDim.left : appDim.top;
        long& appDim2 = (isHAlign) ? appDim.right : appDim.bottom;
        long offset = needOffset ? objectDim2 - objectDim1 : 0;
        bool isShiftable = !needOffset;  // If we have offset horizontally across, it won't be shiftable across.  
                                         // It has to be shiftable vertically instead.  
        // calculate the new left and right coordinates
        long appLength = appDim2 - appDim1;
        long boundaryLength = boundaryDim2 - boundaryDim1;

        // if the app is not resizable but it is longer than the boundary, then just have its left/top
        // positioned on the boundary's left/top.
        if ((appLength > boundaryLength) && isShiftable)
        {
            appDim1 = boundaryDim1;
            appDim2 = appDim1 + appLength;
            LOG( SourceInfo, DebugUtil::FunctionExit, "calculateNoOverlapBoundedCoordinates");
            return true;
        }

        // if we need to offset the application to avoid overlapping the object, then the alignment
        // can't possibly be horizontally or vertically centred.
        if (needOffset)
        {
            if (align == ALIGN_VERTICAL)
            {
                align = DEFAULT_HORIZONTAL_ALIGNMENT;
            }
            else if (align == ALIGN_HORIZONTAL)
            {
                align = DEFAULT_VERTICAL_ALIGNMENT;
            }
        }
            
        switch (align)
        {
        case ALIGN_LEFT:
        case ALIGN_TOP:
        default:
            appDim1 = objectDim1 + offset;
            appDim2 = appDim1 + appLength;

            // if the app falls outside the boundary, then we need to reposition it to fit within the boundary
            if (!((appDim1 >= boundaryDim1) && (appDim2 <= boundaryDim2)))
            {
                // if is is no overlap and we don't want to resize or reposition the application window
                // to fit within the boundary, then don't set the coordinates, just return.
                if (!isShiftable)
                {
                    return false;
                }

                // Push the app to the left until it its right edge falls within the boundary
                appDim2 = boundaryDim2;
                appDim1 = appDim2 - appLength;
            }                
            break;
        case ALIGN_VERTICAL:
        case ALIGN_HORIZONTAL:
            appDim1 = objectDim1 + offset - (appLength/2);
            appDim2 = appDim1 + appLength;
            if (appDim1 < boundaryDim1)    // if the top/left of the app falls outside the bound:
            {
                if (!isShiftable)
                {
                    return false;
                }

                // otherwise, make sure the top/left side is within the boundary
                appDim1 = boundaryDim1;
                appDim2 = appDim1 + appLength;
            }
            break;
        case ALIGN_RIGHT:
        case ALIGN_BOTTOM:
            appDim2 = objectDim2 - offset;
            appDim1 = appDim2 - appLength;

            if (!((appDim1 >= boundaryDim1) && (appDim2 <= boundaryDim2)))
            {
                // if is is no overlap and we don't want to resize or reposition the application window
                // to fit within the boundary, then don't set the coordinates, just return.
                if (!isShiftable)
                {
                    return false;
                }

                // We want the left side of the app to be completely in the boundary
                appDim1 = boundaryDim1;
                appDim2 = appDim1 + appLength;
            }                
            break;
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "calculateNoOverlapBoundedCoordinates");

        return true;
    }


    bool AppLauncher::isInvalidRect(const RECT& rect)
    {
        return ((rect.right < rect.left) ||
                 (rect.bottom < rect.top) ||        // see if it is an invalid rect
                 ((rect.left == DEFAULT_RECT.left) &&
                  (rect.right == DEFAULT_RECT.right) &&
                  (rect.top == DEFAULT_RECT.top) &&
                  (rect.bottom == DEFAULT_RECT.bottom)));

    }


    void AppLauncher::boundAppRect( RECT& appDim,
                                    const RECT& boundaryDim,
                                    const bool isResizable)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "boundAppRect");
        int appWidth = appDim.right - appDim.left;
        int appHeight = appDim.bottom - appDim.top;

        // if the resultant application falls completely outside the boundary we need to push 
        // the application back into the boundary
        if (appDim.top > boundaryDim.bottom)        // app is below bounded area
        {
            appDim.bottom = boundaryDim.bottom;
            appDim.top = appDim.bottom - appHeight;
        }
        else if (appDim.bottom < boundaryDim.top)   // app is above bounded area
        {
            appDim.top = boundaryDim.top;
            appDim.bottom = appDim.top + appHeight;
        }

        if (appDim.left > boundaryDim.right)        // app is on the right of bounded area
        {
            appDim.right = boundaryDim.right;
            appDim.left = appDim.right - appWidth;
        }
        else if (appDim.right < boundaryDim.left)   // app is on the left of bounded area
        {
            appDim.left = boundaryDim.left;
            appDim.right = appDim.left + appWidth;
        }

        // if it is resizable, then any part that falls outside the boundary will be removed
        // otherwise it's going to be pushed into the boundary
        if (isResizable)
        {
            if (appDim.bottom > boundaryDim.bottom)
            {
                appDim.bottom = boundaryDim.bottom;
            }

            if (appDim.right > boundaryDim.right)
            {
                appDim.right = boundaryDim.right;
            }

            if (appDim.top < boundaryDim.top)
            {
                appDim.top = boundaryDim.top;
            }

            if (appDim.left < boundaryDim.left)
            {
                appDim.left = boundaryDim.left;
            }
        }
        else
        {
            int appHeight = appDim.bottom - appDim.top;
            if (appHeight > (boundaryDim.bottom - boundaryDim.top))
            {
                appDim.top = boundaryDim.top;
                appDim.bottom = appDim.top + appHeight;
            }
            else
            {
                if (appDim.top < boundaryDim.top)
                {
                    appDim.top = boundaryDim.top;
                    appDim.bottom = appDim.top + appHeight;
                }
                if (appDim.bottom > boundaryDim.bottom)
                {
                    appDim.bottom = boundaryDim.bottom;
                    appDim.top = appDim.bottom - appHeight;
                }
            }

            int appWidth = appDim.right - appDim.left;
            if (appWidth > (boundaryDim.right - boundaryDim.left))
            {
                appDim.left = boundaryDim.left;
                appDim.right = appDim.left + appWidth;
            }
            else
            {
                if (appDim.left < boundaryDim.left)
                {
                    appDim.left = boundaryDim.left;
                    appDim.right = appDim.left + appWidth;
                }
                if (appDim.right > boundaryDim.right)
                {
                    appDim.right = boundaryDim.right;
                    appDim.left = appDim.right - appWidth;
                }
            }
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "boundAppRect");
    }

} // End namespace TA_Base_Bus
