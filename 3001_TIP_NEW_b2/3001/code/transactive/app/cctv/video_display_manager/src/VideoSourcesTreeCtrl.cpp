/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/VideoSourcesTreeCtrl.cpp $
  * @author:  Robert Young
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2014/11/05 11:38:32 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * <description>
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

#include "stdafx.h"
#include "VideoSourcesTreeCtrl.h"
#include "VideoDisplayManagerDlg.h"
#include "SequenceEditorDlg.h"
#include "QuadControllerDlg.h"
#include "DisplayItemManager.h"

#include "BvsStage.h"
#include "Camera.h"
#include "Quad.h"
#include "Sequence.h"
#include "Monitor.h"

#include "boost/tokenizer.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    const std::string READ_ONLY_DISPLAY_TEXT = " (Read Only)";

    VideoSourcesTreeCtrl::VideoSourcesTreeCtrl()
      :  m_isSequenceEditorTree(FALSE),
         m_isDragging(FALSE),
         m_wndTip(),
         m_isQuadControllerTree(FALSE),
         m_nCurItem(NULL),
         m_bCapture(FALSE),
         m_tipPopDelay(500),
         m_tipInitialiseDelay(250),
         m_tipTransferDelay(0),
         m_tipTimeoutDelay(5000),
         m_pDetailsEdit(NULL)
    {
        FUNCTION_ENTRY( "VideoSourcesTreeCtrl" );

        // create the root item
        m_videoSourcesGroups.insert( std::pair<std::string, HTREEITEM>("", TVI_ROOT) );

        FUNCTION_EXIT;
    }


    VideoSourcesTreeCtrl::~VideoSourcesTreeCtrl()
    {
        FUNCTION_ENTRY( "~VideoSourcesTreeCtrl" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(VideoSourcesTreeCtrl, CTreeCtrl)
        // {{AFX_MSG_MAP(VideoSourcesTreeCtrl)
        ON_WM_LBUTTONDOWN()
        ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
        ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
        ON_WM_KILLFOCUS()
        ON_WM_SETFOCUS()
        ON_WM_RBUTTONDOWN()
        ON_WM_CONTEXTMENU()
        ON_WM_MOUSEACTIVATE()
        ON_NOTIFY_REFLECT_EX(TVN_SELCHANGED, OnSelchanged)
        ON_WM_MOUSEMOVE()
        ON_WM_TIMER()
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void VideoSourcesTreeCtrl::insertVideoSources( std::vector<VideoInput*> sources )
    {
        FUNCTION_ENTRY( "insertVideoSources" );

		SetRedraw(FALSE);
        // if thee are no items
        if (sources.size() == 0)
        {
            // insert the empty folder
            HTREEITEM newItem = InsertItem("NONE CONFIGURED", 0, 0, TVI_ROOT, TVI_SORT);
        }

        // insert each item
        for (std::vector<VideoInput*>::iterator iter = sources.begin();
        iter != sources.end(); iter++)
        {
            insertVideoSource( *iter );
        }
		SetRedraw(TRUE);
        FUNCTION_EXIT;
    }


    void VideoSourcesTreeCtrl::insertVideoSource( VideoInput* source )
    {
        FUNCTION_ENTRY( "insertVideoSource" );

        TA_ASSERT(source != NULL, "Trying to insert a NULL video source");

        // tokenise group name
        std::vector<std::string> parts = tokeniseGroupName( source->getGroup() );

        // 
        // find the group - insert groups if necessary
        // 

        HTREEITEM groupItem = NULL;
        HTREEITEM parentGroupItem = NULL;

        // if it is a member of a group
        if ( (parts.size() > 0) && (parts[0] != "") )
        {
            // make sure each group level exists
            std::string parentName = "";
            for (std::vector<std::string>::iterator iter = parts.begin();
            iter != parts.end(); iter++)
            {
                // find the group
                groupItem = getGroupItem(iter->c_str(), parentName.c_str());

                // didnt find the group
                if (groupItem == NULL)
                {
                    // insert it
                    HTREEITEM newItem = InsertItem(iter->c_str(), 0, 0, parentGroupItem, TVI_SORT);
                    m_videoSourcesGroups.insert( std::pair<std::string, HTREEITEM>(*iter, newItem) );
                    SetItemData(newItem, NULL);

                    groupItem = newItem;
                }

                // for next time around the loop - set the parent name to the current item
                parentName = *iter;
                // and set the parent group index to the current group index
                parentGroupItem = groupItem;
            }
        }
        else
        {
            // if there was no group configured (empty group name)
            // insert this at the top level
            groupItem = TVI_ROOT;
        }

        // group item should now hold the group that this item should be inserted after

        // 
        // insert the item
        // 
        std::ostringstream text;
        // TD16691
        std::string sourceDescription;
        std::string isReadyOnly("");
        Sequence* sourceSequence = dynamic_cast<Sequence*>(source);

        if (sourceSequence != NULL)
        {
            sourceDescription = sourceSequence->getSequenceDescription();
            if (sourceSequence->getReadOnly())
            {
                isReadyOnly = READ_ONLY_DISPLAY_TEXT;
            }
        }
        else
        {
            sourceDescription = source->getDescription();
        }

        // text << source->getName() << " - " << source->getDescription();
        text << source->getName() << isReadyOnly << " - " << sourceDescription;
        // TD16691

        // based on the type - choose the image that should be displayed
        if (dynamic_cast<Camera*>(source) != NULL)
        {
            // insert the item
            HTREEITEM newItem = InsertItem(text.str().c_str(), 2, 3, groupItem, TVI_LAST);

            // set the data - each list item has a pointer to its associated object
            SetItemData(newItem, reinterpret_cast<DWORD>(source));
        }
        else if (dynamic_cast<Quad*>(source) != NULL)
        {
            // insert the item
            HTREEITEM newItem = InsertItem(text.str().c_str(), 4, 5, groupItem, TVI_LAST);

            // set the data - each list item has a pointer to its associated object
            SetItemData(newItem, reinterpret_cast<DWORD>(source));
        }
        else if (dynamic_cast<Sequence*>(source) != NULL)
        {
            // insert the item
            HTREEITEM newItem = InsertItem(text.str().c_str(), 6, 7, groupItem, TVI_LAST);

            // set the data - each list item has a pointer to its associated object
            SetItemData(newItem, reinterpret_cast<DWORD>(source));
        }

        FUNCTION_EXIT;
    }


    std::vector<std::string> VideoSourcesTreeCtrl::tokeniseGroupName(const std::string& groupName)
    {
        FUNCTION_ENTRY( "tokeniseGroupName" );
        FUNCTION_EXIT;
        return tokenizeString(groupName, "/");
    }


    std::vector<std::string> VideoSourcesTreeCtrl::tokenizeString( const std::string& theString,
                                                                   const std::string& separatorList )
    {
        FUNCTION_ENTRY( "tokenizeString" );

        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( separatorList.c_str() );
        tokenizer tokens(theString, sep);

        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
            parts.push_back(*tok_iter);
        }

        // if parts is empty, then this should return the entire string
        if (parts.size() == 0)
        {
            parts.push_back(theString);
        }

        FUNCTION_EXIT;
        return parts;
    }


    void VideoSourcesTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnLButtonDown" );

        CWnd* prevFoc = SetFocus();
        HTREEITEM newSelection = NULL;
        UINT pFlags = 0;
        newSelection = HitTest(point,&pFlags);

        if ( (NULL != newSelection) &&
            ( (pFlags == TVHT_ONITEMICON) || (pFlags == TVHT_ONITEMLABEL) ) )
        {
            if ( ( newSelection != GetSelectedItem() ) || (prevFoc != this) )
            {
                m_wndTip.Cancel();
                // Get text and text rectangle for item under mouse
                CString sText; // item text
                CRect rcText;  // item text rect
                OnGetItemInfo(point, rcText, sText);
                if (FALSE == IsRectCompletelyVisible(rcText))
                {
                    m_wndTip.ShowDelayed(m_tipPopDelay);
                }
                CTreeCtrl::SelectItem( newSelection );
            }
        }
        CTreeCtrl::OnLButtonDown(nFlags, point);

        FUNCTION_EXIT;
    }


    void VideoSourcesTreeCtrl::endDragging()
    {
        FUNCTION_ENTRY( "endDragging" );

        m_isDragging = FALSE;

        FUNCTION_EXIT;
    }


    // Rob Y:
    // Had to write this method as the CTreeCtrl method does not work properly
    // with the Font Arial Bold!!
    CImageList* VideoSourcesTreeCtrl::CreateDragImageEx(HTREEITEM hItem)
    {
        FUNCTION_ENTRY( "CreateDragImageEx" );

        CImageList* std = CreateDragImage(hItem);
        IMAGEINFO info;
        CRect stdRect;
        std->GetImageInfo(0,&info);
        stdRect = info.rcImage;
        CRect rect;
        GetItemRect(hItem, rect, TRUE);

        rect.OffsetRect((stdRect.Width() - rect.Width() + 2) - rect.left, -rect.top);

        // Create bitmap
        CClientDC    dc (this);
        CDC         memDC;

        if(!memDC.CreateCompatibleDC(&dc))
        {
            FUNCTION_EXIT;
            return NULL;
        }


        CBitmap bitmap;
        if(!bitmap.CreateCompatibleBitmap(&dc, stdRect.Width(), stdRect.Height()))
        {
            FUNCTION_EXIT;
            return NULL;
        }


        CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
        CFont* pOldFont = memDC.SelectObject(GetFont());

        memDC.FillSolidRect(&stdRect, RGB(140, 140, 140)); // mask
        memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));

        int itemImageIndex, selectedItemImageIndex;
        GetItemImage(GetSelectedItem(), itemImageIndex,
                            selectedItemImageIndex);

        ::DrawIconEx(memDC.GetSafeHdc(),0,0,m_imageList.ExtractIcon(itemImageIndex),
                        16,16,0,NULL,DI_NORMAL);

        memDC.TextOut(rect.left, rect.top, GetItemText(hItem));

        memDC.SelectObject( pOldFont );
        memDC.SelectObject( pOldMemDCBitmap );


        // Create imagelist
        CImageList* pImageList = new CImageList;
        pImageList->Create(stdRect.Width(), stdRect.Height(),
         ILC_COLOR | ILC_MASK, 0, 1);
        pImageList->Add(&bitmap, RGB(140, 140, 140)); // mask

        delete std;

        FUNCTION_EXIT;
        return pImageList;
    }


    HTREEITEM VideoSourcesTreeCtrl::getGroupItem(const std::string& groupName, const std::string& parentName) const
    {
        FUNCTION_ENTRY( "getGroupItem" );

        // find the first matching key
        std::multimap<std::string, HTREEITEM>::const_iterator findIter = m_videoSourcesGroups.find(groupName);

        if ( findIter != m_videoSourcesGroups.end() )
        {
            do
            {
                // check the parent matches
                HTREEITEM parent = GetParentItem(findIter->second);
                if ( (GetItemText(parent) == parentName.c_str()) || (parentName.size() == 0) )
                {
                    // return the HTREEITEM
                    FUNCTION_EXIT;
                    return findIter->second;
                }

                findIter++;
            } while ( findIter != m_videoSourcesGroups.upper_bound(groupName) );
        }

        FUNCTION_EXIT;
        return NULL;
    }


    void VideoSourcesTreeCtrl::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "OnItemexpanded" );

        NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

        UINT action = pNMTreeView->action;
        TVITEM tvItem = pNMTreeView->itemNew;
        tvItem.mask |= TVIF_IMAGE | TVIF_SELECTEDIMAGE;

        GetItem(&tvItem);

        if (action == TVE_EXPAND && tvItem.iImage == 0)
        {
            tvItem.iImage = 1;
            tvItem.iSelectedImage = 1;
            SetItem(&tvItem);
            resetToolTip();
        }
        else if (action == TVE_COLLAPSE && tvItem.iImage == 1)
        {
            tvItem.iImage = 0;
            tvItem.iSelectedImage = 0;
            SetItem(&tvItem);
            resetToolTip();
        }
        *pResult = 0;

        FUNCTION_EXIT;
    }


    void VideoSourcesTreeCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "OnBegindrag" );

        NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
        // Send WM_LBUTTONDOWN message to VideoDisplayManagerDlg.
        if ( FALSE == m_isSequenceEditorTree )
        {
            CRect rect;
            m_wndTip.GetWindowRect(&rect);
            m_wndTip.Cancel();
            GetActiveWindow()->RedrawWindow(&rect);
            if ( FALSE == CTreeCtrl::ItemHasChildren(CTreeCtrl::GetSelectedItem()) )
            {
                CRect itemRect;
                // unfortunately CreateDragImage does not work properly (no text)
                // for the font Arial Bold so had to write our own method to do
                // same thing!
                GetItemRect(CTreeCtrl::GetSelectedItem(),&itemRect,TRUE);
                CImageList* dragImage = this->CreateDragImageEx(CTreeCtrl::GetSelectedItem());
                CPoint adjust;
                adjust.y = pNMTreeView->ptDrag.y - itemRect.top - 1;
                adjust.x = pNMTreeView->ptDrag.x - itemRect.left + 19;
                if ( FALSE == m_isQuadControllerTree )
                {
                    ( (VideoDisplayManagerDlg*)(AfxGetApp()->m_pMainWnd) )->
                        setDragAdjust(adjust);
                    ( (VideoDisplayManagerDlg*)(AfxGetApp()->m_pMainWnd) )->
                        setDragImage(dragImage);
                    m_isDragging = TRUE;
                    ::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_LBUTTONDOWN, NULL, NULL);
                }
                else
                {
                    ( (QuadControllerDlg*)(GetParent()) )->
                        setDragAdjust(adjust);
                    ( (QuadControllerDlg*)(GetParent()) )->
                        setDragImage(dragImage);
                    m_isDragging = TRUE;
                    ::SendMessage(GetParent()->GetSafeHwnd(), WM_LBUTTONDOWN, NULL, NULL);
                }

            }
        }
        *pResult = 0;

        FUNCTION_EXIT;
    }


    BOOL VideoSourcesTreeCtrl::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY( "PreTranslateMessage" );

        switch ( pMsg->message )
        {
        case WM_KEYDOWN:
            {
                m_wndTip.Cancel();
                m_nCurItem = NULL;
                switch ( pMsg->wParam )
                {
                case VK_ESCAPE:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                default:
                    {
                        // do nothing
                    }
                    break;
                }
            }
            break;
        case WM_MOUSEWHEEL:
            {
                if ( TRUE == m_isDragging )
                {
                    // Disable mousewheel during drags as
                    // causes drawing problems
                    FUNCTION_EXIT;
                    return TRUE;
                }
                else
                {
                    // reset  the "tooltip wnd"
                    m_nCurItem = NULL;
                    m_wndTip.Cancel();
                }
            }
            break;
        default:
            {
                // do nothing
            }
            break;
        }

        FUNCTION_EXIT;
        return CTreeCtrl::PreTranslateMessage(pMsg);
    }


    void VideoSourcesTreeCtrl::OnKillFocus(CWnd* pNewWnd)
    {
        FUNCTION_ENTRY( "OnKillFocus" );

        if ( m_isSequenceEditorTree )
        {
            ((SequenceEditorDlg*)GetParent())->enableAddButton(FALSE);
        }

        if ( !ItemHasChildren(GetSelectedItem()) )
        {
            int normal, selected =0;
            this->GetItemImage(GetSelectedItem(), normal, selected);
            this->SetItemImage(GetSelectedItem(), normal, normal);
        }

        if ((NULL != GetSelectedItem()) && (NULL != m_pDetailsEdit))
        {
            this->m_pDetailsEdit->SetWindowText("No item currently selected");
        }

        CTreeCtrl::OnKillFocus(pNewWnd);

        FUNCTION_EXIT;
    }


    void VideoSourcesTreeCtrl::OnSetFocus(CWnd* pOldWnd)
    {
        FUNCTION_ENTRY( "OnSetFocus" );

        if ( TRUE == m_isQuadControllerTree )
        {
            ::SendMessage(GetParent()->GetSafeHwnd(), WM_LBUTTONDOWN, -1, -1);
        }
        else if ( FALSE == m_isSequenceEditorTree )
        {
            ::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_LBUTTONDOWN, -1, -1);

            // update the details edit
            handleSelectionChanged();
        }

        if ( m_isSequenceEditorTree )
        {
            ((SequenceEditorDlg*)GetParent())->enableAddButton(
                !ItemHasChildren(GetSelectedItem()) );
        }

        if ( !ItemHasChildren(GetSelectedItem()) )
        {
            int normal, selected =0;
            this->GetItemImage(GetSelectedItem(), normal, selected);
            this->SetItemImage(GetSelectedItem(), normal, normal+1);
        }

        CTreeCtrl::OnSetFocus(pOldWnd);

        FUNCTION_EXIT;
    }


    BOOL VideoSourcesTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "OnSelchanged" );

        NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

        if ( m_isSequenceEditorTree )
        {
            ((SequenceEditorDlg*)GetParent())->enableAddButton(
                !ItemHasChildren(GetSelectedItem()) );
        }
        else if (this->m_isQuadControllerTree == FALSE)
        {
            // update the details edit
            handleSelectionChanged();
        }
        *pResult = 0;

        FUNCTION_EXIT;
        return FALSE;
    }


    void VideoSourcesTreeCtrl::handleSelectionChanged()
    {
        FUNCTION_ENTRY( "handleSelectionChanged" );

        HTREEITEM selectedItem = GetSelectedItem();

        // if there is a selected item
        // and the details edit has been set
        if ( (NULL != selectedItem) &&
             (NULL != m_pDetailsEdit) )
        {
            // get the item data for the selected item
            DWORD itemData = GetItemData(selectedItem);

            // if the data has been set to a pointer
            if (itemData != NULL)
            {
                // get the item details
                VideoInput* input = reinterpret_cast<VideoInput*>(itemData);

                // get the details from the object
                this->m_pDetailsEdit->SetWindowText(input->getDetails().c_str());
            }
            else // NULL item data - this must be a group
            {
                CString selItemText = GetItemText(GetSelectedItem());
                selItemText += " Group";
                this->m_pDetailsEdit->SetWindowText(selItemText);
            }
        }

        FUNCTION_EXIT;
    }


    void VideoSourcesTreeCtrl::videoInputUpdated(VideoInput* input)
    {
        FUNCTION_ENTRY( "videoInputUpdated" );

        // update the description of the item that changed
        HTREEITEM changedItem = findItemFromSubstr(input->getName().c_str());
        if (changedItem != NULL)
        {
            std::ostringstream text;
            // TD16691
            std::string inputDescription;
            std::string isReadOnlyStr("");
            Sequence* inputSequence = dynamic_cast<Sequence*>(input);

            if (inputSequence != NULL)
            {
                inputDescription = inputSequence->getSequenceDescription();
                if (inputSequence->getReadOnly())
                {
                    isReadOnlyStr = READ_ONLY_DISPLAY_TEXT;
                }
            }
            else
            {
                inputDescription = input->getDescription();
            }

            text << input->getName() << isReadOnlyStr << " - " << inputDescription;
            SetItemText(changedItem, text.str().c_str());

            // TD16691
            // only update the details if this window has focus
            if ( (GetFocus() == this) && (changedItem == GetSelectedItem()) )
            {
                handleSelectionChanged();
            }
        }

        FUNCTION_EXIT;
    }


    VideoInput* VideoSourcesTreeCtrl::getSelectedInput()
    {
        FUNCTION_ENTRY( "getSelectedInput" );

        HTREEITEM selectedItem = GetSelectedItem();

        // if there is a selected item
        if (NULL != selectedItem)
        {
            // get the item data for the selected item
            DWORD itemData = GetItemData(selectedItem);

            // if the data has been set to a pointer
            if (itemData != NULL)
            {
                // get the item details
                VideoInput* input = reinterpret_cast<VideoInput*>(itemData);

                // if this can be cast to an input - return that input
                // otherwise it will be NULL
                FUNCTION_EXIT;
                return input;
            }
        }

        FUNCTION_EXIT;
        return NULL;
    }


    void VideoSourcesTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnRButtonDown" );

        SetFocus();
        HTREEITEM newSelection = NULL;
        UINT pFlags = 0;
        newSelection = HitTest(point,&pFlags);

        if ( (NULL != newSelection) &&
            ( (pFlags == TVHT_ONITEMICON) || (pFlags == TVHT_ONITEMLABEL) ) &&
            ( newSelection != GetSelectedItem() ) )
        {
                m_wndTip.Cancel();
                // Get text and text rectangle for item under mouse
                CString sText; // item text
                CRect rcText;  // item text rect
                OnGetItemInfo(point, rcText, sText);
                if (FALSE == IsRectCompletelyVisible(rcText))
                {
                    m_wndTip.ShowDelayed(m_tipPopDelay);
                }
                CTreeCtrl::SelectItem( newSelection );
        }

        // CTreeCtrl::OnRButtonDown(nFlags, point);

        FUNCTION_EXIT;
    }


    void VideoSourcesTreeCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        FUNCTION_ENTRY( "OnContextMenu" );

        if ( ( FALSE == m_isQuadControllerTree ) && (FALSE == m_isSequenceEditorTree) )
        {
            ScreenToClient(&point);

            UINT pFlags = 0;
            HitTest(point,&pFlags);

            if ( (pFlags == TVHT_ONITEMICON) || (pFlags == TVHT_ONITEMLABEL) )
            {
                int index, selIndex;
                GetItemImage(GetSelectedItem(), index, selIndex);
                // only for Quads and Sequences
                if ( index > 3 )
                {
                    ClientToScreen(&point);
                    // Invoke the context menu functionality on the main dialog.
                    LPARAM lParam = 0;
                    lParam = ( ( point.x ) + (point.y << 16 ) );
                    m_wndTip.Cancel();
                    ::SendMessage( AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_CONTEXTMENU, MK_RBUTTON, lParam );

                }
            }
        }

        FUNCTION_EXIT;
    }


    int VideoSourcesTreeCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
    {
        FUNCTION_ENTRY( "OnMouseActivate" );

        SetFocus();

        FUNCTION_EXIT;
        return CTreeCtrl::OnMouseActivate(pDesktopWnd, nHitTest, message);
    }


    void VideoSourcesTreeCtrl::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        // TODO: Add your specialized code here and/or call the base class
        m_imageList.Create(16, 16, ILC_MASK, 8, 0);
        m_imageList.Add(AfxGetApp()->LoadIcon(IDI_CLOSEDFOLDER));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDI_OPENFOLDER));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDI_CAMERA));
        CClientDC    dc (this);
        CDC         memDC;
        memDC.CreateCompatibleDC(&dc);
        CBitmap cameraBitmap;
        cameraBitmap.CreateCompatibleBitmap(&dc, 16,16);
        CBitmap* pOldMemDCBitmap = memDC.SelectObject( &cameraBitmap );
        memDC.FillSolidRect(0,0,16,16, RGB(140, 140, 140));
        m_imageList.Draw(&memDC, 2, CPoint(0,0), ILD_BLEND50);
        memDC.SelectObject( pOldMemDCBitmap );
        m_imageList.Add(&cameraBitmap,RGB(140, 140, 140));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDI_QUAD));
        CBitmap quadBitmap;
        quadBitmap.CreateCompatibleBitmap(&dc, 16,16);
        pOldMemDCBitmap = memDC.SelectObject( &quadBitmap );
        memDC.FillSolidRect(0,0,16,16, RGB(140, 140, 140));
        m_imageList.Draw(&memDC, 4, CPoint(0,0), ILD_BLEND50);
        memDC.SelectObject( pOldMemDCBitmap );
        m_imageList.Add(&quadBitmap,RGB(140, 140, 140));
        m_imageList.Add(AfxGetApp()->LoadIcon(IDI_SEQUENCE));
        CBitmap sequenceBitmap;
        sequenceBitmap.CreateCompatibleBitmap(&dc, 16,16);
        pOldMemDCBitmap = memDC.SelectObject( &sequenceBitmap );
        memDC.FillSolidRect(0,0,16,16, RGB(140, 140, 140));
        m_imageList.Draw(&memDC, 6, CPoint(0,0), ILD_BLEND50);
        memDC.SelectObject( pOldMemDCBitmap );
        m_imageList.Add(&sequenceBitmap,RGB(140, 140, 140));
        SetImageList(&m_imageList, TVSIL_NORMAL);

        // make sure normal tooltips are never set (we are using our own as
        // CTreeCtrl's supplied tooltip functionality has fault
        CToolTipCtrl* tip = GetToolTips();
        if (NULL != tip)
        {
            tip->Activate(FALSE);
        }
        // now create custom wnd to use as tooltip
        if (!m_wndTip)
        {
            CFont* thisfont = this->GetFont();
            LOGFONT font;
            thisfont->GetLogFont(&font);
            m_wndTip.Create(CPoint(0,0), this, PTS_TRANSPARENT);
        }

        CTreeCtrl::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    BOOL VideoSourcesTreeCtrl::IsRectCompletelyVisible(const CRect& rc)
    {
        FUNCTION_ENTRY( "IsRectCompletelyVisible" );

        CRect rcClient;
        GetClientRect(&rcClient);

        FUNCTION_EXIT;
        return ( (rcClient.right > rc.right) && (rcClient.left < rc.left) );
    }


    HTREEITEM VideoSourcesTreeCtrl::OnGetItemInfo(CPoint p, CRect& rc, CString& s)
    {
        FUNCTION_ENTRY( "OnGetItemInfo" );

        UINT pFlags;
        HTREEITEM nItem = HitTest(p, &pFlags);
        s.Empty();
        if ( (NULL != nItem) &&
            ( (pFlags == TVHT_ONITEMICON) || (pFlags == TVHT_ONITEMLABEL) ) )
        {
            s = GetItemText(nItem);
            GetItemRect(nItem, &rc, TRUE);
            CFont *pFont = GetFont();
            CClientDC dc(this);
            CFont* pOldFont = dc.SelectObject(pFont);
            dc.DrawText(s,&rc,DT_CALCRECT);
            dc.SelectObject(pOldFont);

            FUNCTION_EXIT;
            return nItem;
        }

        FUNCTION_EXIT;
        return NULL;
    }


    void VideoSourcesTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnMouseMove" );

        KillTimer(1);
        SetTimer(1,m_tipTimeoutDelay,NULL);
        // Get text and text rectangle for item under mouse
        CString sText; // item text
        CRect rcText;  // item text rect
        HTREEITEM nItem = OnGetItemInfo(point, rcText, sText);

        if ( (nItem==NULL) || (nItem!=m_nCurItem) )
        {
           BOOL tipVisible = m_wndTip.IsWindowVisible();
           m_wndTip.Cancel(); // new item, or no item: cancel popup text
           if (nItem!=NULL && !IsRectCompletelyVisible(rcText))
           {
               // new item, and not wholly visible: prepare popup tip
               CRect rc = rcText;
               ClientToScreen(&rc);   // text rect in screen coords
               //sText.Insert(86, "\r\n");    //TD17254    In order to wrap up the text in one screen
               m_wndTip.SetWindowText(sText);   // set tip text to that of item
               // move tip window over tree text
               m_wndTip.SetWindowPos(NULL, rc.left, rc.top - 16, rc.Width()+16,
                   rc.Height(), SWP_NOZORDER|SWP_NOACTIVATE);    //TD17254    In order to wrap up the text in one screen
               if ( FALSE == tipVisible )
               {
                   m_wndTip.ShowDelayed(m_tipInitialiseDelay); // show popup text delayed
                    KillTimer(1);
                    SetTimer(1,(m_tipTimeoutDelay + m_tipInitialiseDelay),NULL);
               }
               else
               {
                   m_wndTip.ShowDelayed(m_tipTransferDelay); // show popup text delayed
                    KillTimer(1);
                    SetTimer(1,(m_tipTimeoutDelay + m_tipTransferDelay),NULL);
               }
               if ( !m_bCapture)
               {
                    SetCapture();
                    m_bCapture = TRUE;
               }
           }
        }
        else
        {
            if ( m_bCapture )
            {
                SetCapture();
            }
        }

        m_nCurItem = nItem;
        if ( (m_bCapture) && (nItem==NULL) )
        {
           ReleaseCapture();
           m_bCapture=FALSE;
        }

        CTreeCtrl::OnMouseMove(nFlags, point);

        FUNCTION_EXIT;
    }


    void VideoSourcesTreeCtrl::OnTimer(UINT nIDEvent)
    {
        FUNCTION_ENTRY( "OnTimer" );

        if (1 == nIDEvent )
        {
            m_wndTip.Cancel();
        }

        CTreeCtrl::OnTimer(nIDEvent);

        FUNCTION_EXIT;
    }


    void VideoSourcesTreeCtrl::expandToGroups()
    {
        FUNCTION_ENTRY( "expandToGroups" );

        std::multimap<std::string, HTREEITEM>::iterator theIterator;
        for ( theIterator = m_videoSourcesGroups.begin();
        theIterator != m_videoSourcesGroups.end(); theIterator++ )
        {
            if ( ItemHasChildren(GetChildItem(( theIterator->second )) ) )
            {
                Expand(( theIterator->second ), TVE_EXPAND);
            }
        }

        // select the first group
        if ( m_videoSourcesGroups.size() > 1 )
        {
            SelectItem( GetNextItem(TVI_ROOT, TVGN_CHILD) );
        }

        FUNCTION_EXIT;
    }


    HTREEITEM VideoSourcesTreeCtrl::findItemFromSubstr(const CString& substr, HTREEITEM treeItem /* = NULL */)
    {
        FUNCTION_ENTRY( "findItemFromSubstr" );

        HTREEITEM nextItem;
        HTREEITEM resultItem = NULL;

        if (treeItem == NULL)
        {
            treeItem = GetRootItem();
        }

        if ( -1 != GetItemText(treeItem).Find( substr ) )
        {
            FUNCTION_EXIT;
            return treeItem;
        }

        nextItem = GetNextItem(treeItem, TVGN_CHILD);
        if (nextItem != NULL)
        {
            resultItem = findItemFromSubstr(substr, nextItem);
        }

        if (resultItem != NULL)
        {
            FUNCTION_EXIT;
            return resultItem;
        };

        nextItem = GetNextItem(treeItem, TVGN_NEXT);
        if (nextItem != NULL)
        {
            resultItem = findItemFromSubstr(substr, nextItem);
        }

        FUNCTION_EXIT;
        return resultItem;
    }


    void VideoSourcesTreeCtrl::resetToolTip()
    {
        FUNCTION_ENTRY( "resetToolTip" );

        m_wndTip.Cancel();
        m_nCurItem = NULL;
        m_bCapture = FALSE;
        ReleaseCapture();

        FUNCTION_EXIT;
    }


} // TA_IRS_App
