//------------------------------------------------------------------------------
// File    : NewMenu.h 
// Version : 1.18
// Date    : 30. November 2003
// Author  : Bruno Podetti
// Email   : Podetti@gmx.net
// Web     : www.podetti.com/NewMenu 
// Systems : VC6.0/7.0 and VC7.1 (Run under (Window 98/ME), Windows Nt 2000/XP)
//
// You are free to use/modify this code but leave this header intact.
// This class is public domain so you are free to use it any of your 
// applications (Freeware, Shareware, Commercial). 
// All I ask is that you let me know so that if you have a real winner I can
// brag to my buddies that some of my code is in your app. I also wouldn't 
// mind if you sent me a copy of your application since I like to play with
// new stuff.
//------------------------------------------------------------------------------
#ifndef __CNewMenu_H_
#define __CNewMenu_H_

// new constant for having the same color in the toolbar, you can define this
// also in the projekt definition
//#define USE_NEW_DOCK_BAR

#pragma warning(push)
#pragma warning(disable : 4211)

#include <afxtempl.h>
#include <afxpriv.h>

// Flagdefinitions 
#define MFT_TITLE       0x0001
#define MFT_TOP_TITLE   0x0000
#define MFT_SIDE_TITLE  0x0002
#define MFT_GRADIENT    0x0004
#define MFT_SUNKEN      0x0008
#define MFT_LINE        0x0010
#define MFT_ROUND       0x0020
#define MFT_CENTER      0x0040

// Typedefinition for compatibility to MFC 7.0
#ifndef DWORD_PTR
typedef DWORD DWORD_PTR, *PDWORD_PTR;
#endif

#ifndef ULONG_PTR
typedef unsigned long ULONG_PTR, *PULONG_PTR;
#endif

#ifndef LONG_PTR
typedef long LONG_PTR, *PLONG_PTR;
#endif

// Additional flagdefinition for highlighting
#ifndef ODS_HOTLIGHT
#define ODS_HOTLIGHT        0x0040
#endif

#ifndef ODS_INACTIVE
#define ODS_INACTIVE        0x0080
#endif


/////////////////////////////////////////////////////////////////////////////
// Constans for detecting OS-Type
enum Win32Type
{
  Win32s,
  WinNT3,
  Win95,
  Win98,
  WinME,
  WinNT4,
  Win2000,
  WinXP
};

extern const Win32Type g_Shell;
extern BOOL bRemoteSession;

/////////////////////////////////////////////////////////////////////////////
// Support for getting menuinfo without runtime-error

#if(WINVER < 0x0500)

#define MIM_BACKGROUND              0x00000002

typedef struct tagMENUINFO
{
    DWORD   cbSize;
    DWORD   fMask;
    DWORD   dwStyle;
    UINT    cyMax;
    HBRUSH  hbrBack;
    DWORD   dwContextHelpID;
    ULONG_PTR dwMenuData;
}   MENUINFO, FAR *LPMENUINFO;
typedef MENUINFO CONST FAR *LPCMENUINFO;

BOOL GetMenuInfo( HMENU hMenu, LPMENUINFO pInfo);
#endif


/////////////////////////////////////////////////////////////////////////////
// Global helperfunctions
UINT GetSafeTimerID(HWND hWnd, const UINT uiElapse);
BOOL DrawMenubarItem(CWnd* pWnd,CMenu* pMenu, UINT nItemIndex,UINT nState);

WORD NumBitmapColors(LPBITMAPINFOHEADER lpBitmap);
HBITMAP LoadColorBitmap(LPCTSTR lpszResourceName, HMODULE hInst, int* pNumcol=NULL);

void UpdateMenuBarColor(HMENU hMenu=NULL);

/////////////////////////////////////////////////////////////////////////////
// Forwarddeclaration for drawing purpose
class CMenuTheme;
class CMenuDockBar;

/////////////////////////////////////////////////////////////////////////////
// CNewMenuIcons menu icons for drawing

class CNewMenuIcons : public CObject
{
  DECLARE_DYNCREATE(CNewMenuIcons)

public:
  CNewMenuIcons();
  virtual ~CNewMenuIcons();

public:
  BOOL GetIconSize(int* cx, int* cy);
  CSize GetIconSize();

  virtual int FindIndex(UINT nID);
  virtual void OnSysColorChange();

  virtual BOOL LoadToolBar(LPCTSTR lpszResourceName, HMODULE hInst);
  virtual BOOL LoadToolBar(WORD* pToolInfo, COLORREF crTransparent=CLR_NONE);

  virtual BOOL DoMatch(LPCTSTR lpszResourceName, HMODULE hInst);
  virtual BOOL DoMatch(WORD* pToolInfo, COLORREF crTransparent=CLR_NONE);

  virtual BOOL LoadBitmap(int nWidth, int nHeight, LPCTSTR lpszResourceName, HMODULE hInst=NULL);

//  virtual BOOL SetBlendImage();
  virtual int AddGloomIcon(HICON hIcon, int nIndex=-1);
  virtual int AddGrayIcon(HICON hIcon, int nIndex=-1);
  virtual BOOL MakeImages();

  void SetResourceName(LPCTSTR lpszResourceName);

  int AddRef();
  int Release();

#if defined(_DEBUG) || defined(_AFXDLL)
  // Diagnostic Support
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:
  LPCTSTR m_lpszResourceName;
  HMODULE m_hInst;
  int m_nColors;
  COLORREF m_crTransparent;

  CImageList m_IconsList;

  CArray<UINT,UINT&> m_IDs;
  DWORD m_dwRefCount;
};

/////////////////////////////////////////////////////////////////////////////
// CNewMenuBitmaps menu icons for drawing
class CNewMenuBitmaps : public CNewMenuIcons
{
  DECLARE_DYNCREATE(CNewMenuBitmaps)

public:
  CNewMenuBitmaps();
  virtual ~CNewMenuBitmaps();

public:
  int Add(UINT nID, COLORREF crTransparent=CLR_NONE);
  int Add(HICON hIcon, UINT nID=0);
  int Add(CBitmap* pBitmap, COLORREF crTransparent=CLR_NONE);

  virtual void OnSysColorChange();

  CArray<COLORREF,COLORREF&> m_TranspColors;
};

/////////////////////////////////////////////////////////////////////////////
// CNewMenuItemData menu item data for drawing

class CNewMenuItemData : public CObject
{
  DECLARE_DYNCREATE(CNewMenuItemData)

public:
  CNewMenuItemData();
  virtual ~CNewMenuItemData();

public:
  LPCTSTR GetString();
  void SetString(LPCTSTR szMenuText);

#if defined(_DEBUG) || defined(_AFXDLL)
  // Diagnostic Support
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

public:
  CString m_szMenuText;

  UINT m_nTitleFlags;
  UINT m_nFlags;
  UINT m_nID;
  UINT m_nSyncFlag;

  CNewMenuIcons* m_pMenuIcon;
  int m_nMenuIconOffset;

  void* m_pData;
};

/////////////////////////////////////////////////////////////////////////////
// CNewMenu the new menu class

class CNewMenu : public CMenu
{
  friend class CNewMenuHook;
  friend class CNewMenuIcons;

  DECLARE_DYNCREATE(CNewMenu)

public:
  // how the menu's are drawn, either original or XP style
  typedef enum 
  { 
    STYLE_ORIGINAL,
    STYLE_ORIGINAL_NOBORDER,

    STYLE_XP,
    STYLE_XP_NOBORDER,

    STYLE_SPECIAL,
    STYLE_SPECIAL_NOBORDER,

    STYLE_ICY,
    STYLE_ICY_NOBORDER,

    STYLE_XP_2003,
    STYLE_XP_2003_NOBORDER,

    STYLE_UNDEFINED = -1

  } EDrawStyle;

  // how seperators are handled when removing a menu (Tongzhe Cui)
  typedef enum {NONE, HEAD, TAIL, BOTH} ESeperator;

public:
  CNewMenu(HMENU hParent=0); 
  virtual ~CNewMenu();

  // Functions for loading and applying bitmaps to menus (see example application)
  virtual BOOL LoadMenu(HMENU hMenu);
  virtual BOOL LoadMenu(LPCTSTR lpszResourceName);
  virtual BOOL LoadMenu(int nResource);

  BOOL LoadToolBar(WORD* pIconInfo, COLORREF crTransparent=CLR_NONE);
  BOOL LoadToolBar(LPCTSTR lpszResourceName, HMODULE hInst = NULL);
  BOOL LoadToolBar(UINT nToolBar, HMODULE hInst = NULL);
  BOOL LoadToolBars(const UINT *arID,int n, HMODULE hInst = NULL);

  BOOL LoadFromToolBar(UINT nID,UINT nToolBar,int& xoffset);
  BOOL AddBitmapToImageList(CImageList *list,UINT nResourceID);

  static HBITMAP LoadSysColorBitmap(int nResourceId);
  // custom check mark bitmaps
  void LoadCheckmarkBitmap(int unselect, int select); 

  // functions for appending a menu option, use the AppendMenu call
  BOOL AppendMenu(UINT nFlags,UINT nIDNewItem=0,LPCTSTR lpszNewItem=NULL,int nIconNormal=-1);
  BOOL AppendMenu(UINT nFlags,UINT nIDNewItem,LPCTSTR lpszNewItem,CImageList *il,int xoffset);
  BOOL AppendMenu(UINT nFlags,UINT nIDNewItem,LPCTSTR lpszNewItem,CBitmap *bmp);

  BOOL AppendODMenu(LPCTSTR lpstrText, UINT nFlags = MF_OWNERDRAW, UINT nID = 0, int nIconNormal = -1);  
  BOOL AppendODMenu(LPCTSTR lpstrText, UINT nFlags, UINT nID, CBitmap* pbmp);
  BOOL AppendODMenu(LPCTSTR lpstrText, UINT nFlags, UINT nID, CImageList *il, int xoffset);
  BOOL AppendODMenu(LPCTSTR lpstrText, UINT nFlags, UINT nID, CNewMenuIcons* pIcons, int nIndex);

  // for appending a popup menu (see example application)
  CNewMenu* AppendODPopupMenu(LPCTSTR lpstrText);

  // functions for inserting a menu option, use the InsertMenu call (see above define)
  BOOL InsertMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem=0,LPCTSTR lpszNewItem=NULL,int nIconNormal=-1);
  BOOL InsertMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem,LPCTSTR lpszNewItem,CImageList *il,int xoffset);
  BOOL InsertMenu(UINT nPosition,UINT nFlags,UINT nIDNewItem,LPCTSTR lpszNewItem,CBitmap *bmp);

  BOOL InsertODMenu(UINT nPosition,LPCTSTR lpstrText, UINT nFlags = MF_OWNERDRAW,UINT nID = 0,int nIconNormal = -1);  
  BOOL InsertODMenu(UINT nPosition,LPCTSTR lpstrText, UINT nFlags, UINT nID, CBitmap* pBmp);
  BOOL InsertODMenu(UINT nPosition,LPCTSTR lpstrText, UINT nFlags, UINT nID, CImageList *il,int xoffset);
  BOOL InsertODMenu(UINT nPosition,LPCTSTR lpstrText, UINT nFlags, UINT nID, CNewMenuIcons* pIcons, int nIndex);

  // Same as ModifyMenu but replacement for CNewMenu
  BOOL ModifyODMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem = 0,LPCTSTR lpszNewItem = NULL);
	BOOL ModifyODMenu(UINT nPosition, UINT nFlags, UINT nIDNewItem, const CBitmap* pBmp);

  // functions for modifying a menu option, use the ModifyODMenu call (see above define)
  BOOL ModifyODMenu(LPCTSTR lpstrText, UINT nID=0, int nIconNormal=-1);
  BOOL ModifyODMenu(LPCTSTR lpstrText, UINT nID, CImageList *il, int xoffset);
  BOOL ModifyODMenu(LPCTSTR lpstrText, UINT nID, CNewMenuIcons* pIcons, int nIndex);

  BOOL ModifyODMenu(LPCTSTR lpstrText,UINT nID,CBitmap *bmp);
  BOOL ModifyODMenu(LPCTSTR lpstrText,LPCTSTR OptionText,int nIconNormal);
  // use this method for adding a solid/hatched colored square beside a menu option
  // courtesy of Warren Stevens
  BOOL ModifyODMenu(LPCTSTR lpstrText,UINT nID,COLORREF fill,COLORREF border,int hatchstyle=-1);

  // for deleting and removing menu options
  BOOL  DeleteMenu(UINT uiId,UINT nFlags);
  BOOL  RemoveMenu(UINT uiId,UINT nFlags);
  int RemoveMenu(LPCTSTR pText, ESeperator sPos=CNewMenu::NONE);

  // function for retrieving and setting a menu options text (use this function
  // because it is ownerdrawn)
  BOOL GetMenuText(UINT id,CString &string,UINT nFlags = MF_BYPOSITION);
  BOOL SetMenuText(UINT id,CString string, UINT nFlags = MF_BYPOSITION);

  // Getting a submenu from it's name or position
  CMenu* GetSubMenu (LPCTSTR lpszSubMenuName) const;
  CMenu* GetSubMenu (int nPos) const;
  int GetMenuPosition(LPCTSTR pText);

  // Destoying
  virtual BOOL DestroyMenu();

  // Drawing: 
  // Draw an item
  virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
  // Measure an item  
  virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
  // Draw title of the menu
  virtual void DrawTitle(LPDRAWITEMSTRUCT lpDIS,BOOL bIsMenuBar);
  // Erase the Background of the menu
  virtual BOOL EraseBkgnd(HWND hWnd,HDC hDC);

  static COLORREF GetMenuBarColor2003();
  static COLORREF GetMenuBarColorXP();
  static COLORREF GetMenuBarColor(HMENU hMenu=NULL);
  static COLORREF GetMenuColor(HMENU hMenu=NULL);

  static void SetMenuTitleFont(CFont* pFont);
  static void SetMenuTitleFont(LOGFONT* pLogFont);
  static LOGFONT GetMenuTitleFont();

  // Menutitle function
  BOOL SetMenuTitle(LPCTSTR pTitle,UINT nTitleFlags=MFT_TOP_TITLE);
  BOOL RemoveMenuTitle();

  // Function to set how menu is drawn, either original or XP style
  static UINT GetMenuDrawMode();
  static UINT SetMenuDrawMode(UINT mode);

  // Function to set how disabled items are drawn 
  //(mode=FALSE means they are not drawn selected)
  static BOOL SetSelectDisableMode(BOOL mode);
  static BOOL GetSelectDisableMode();

  // Function to set how icons were drawn in normal mode 
  //(enable=TRUE means they are drawn blended)
  static BOOL SetXpBlendig(BOOL bEnable=TRUE);
  static BOOL GetXpBlendig();

  // Function to set how default menu border were drawn
  //(enable=TRUE means that all menu in the application has the same border)
  static BOOL SetNewMenuBorderAllMenu(BOOL bEnable=TRUE);
  static BOOL GetNewMenuBorderAllMenu();


  static void OnSysColorChange();

  // Static functions used for handling menu's in the mainframe
  static LRESULT FindKeyboardShortcut(UINT nChar,UINT nFlags,CMenu *pMenu);
  static BOOL OnMeasureItem(const MSG* pMsg);
  static void OnInitMenuPopup(HWND hWnd, CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);

  // Helperfunction to find the menu to the item
  static CMenu* FindPopupMenuFromID(CMenu* pMenu, UINT nID);
  static CMenu* FindPopupMenuFromID(HMENU hMenu, UINT nID);

  static CMenu* FindPopupMenuFromIDData(CMenu* pMenu, UINT nID, ULONG_PTR pData);
  static CMenu* FindPopupMenuFromIDData(HMENU hMenu, UINT nID, ULONG_PTR pData);

  virtual void OnInitMenuPopup();
  virtual BOOL OnUnInitPopupMenu();

  // Customizing:
  // Set icon size
  void SetIconSize(int width, int height);

  // set the color in the bitmaps that is the background transparent color
  COLORREF SetBitmapBackground(COLORREF newColor);
  COLORREF GetBitmapBackground(); 

  // Return the last itemrect from a menubaritem.
  CRect GetLastActiveMenuRect();
  HMENU GetParent();
  BOOL IsPopup();
  BOOL SetPopup(BOOL bIsPopup=TRUE);

  BOOL SetItemData(UINT uiId, DWORD_PTR dwItemData, BOOL fByPos = FALSE);
  BOOL SetItemDataPtr(UINT uiId, void* pItemData, BOOL fByPos = FALSE);

  DWORD_PTR GetItemData(UINT uiId, BOOL fByPos = FALSE) const;
  void* GetItemDataPtr(UINT uiId, BOOL fByPos = FALSE) const;

  BOOL SetMenuData(DWORD_PTR dwMenuData);
  BOOL SetMenuDataPtr(void* pMenuData);

  DWORD_PTR GetMenuData() const;
  void* GetMenuDataPtr() const;

// Miscellaneous Protected Member functions
protected:
  CNewMenuIcons* GetMenuIcon(int &nIndex, UINT nID, CImageList *pil, int xoffset);
  CNewMenuIcons* GetMenuIcon(int &nIndex, int nID);
  CNewMenuIcons* GetMenuIcon(int &nIndex, CBitmap* pBmp);

  CNewMenuIcons* GetToolbarIcons(UINT nToolBar, HMODULE hInst=NULL);

  DWORD SetMenuIcons(CNewMenuIcons* pMenuIcons);
  BOOL Replace(UINT nID, UINT nNewID);

  static BOOL IsNewShell();
  BOOL IsMenuBar(HMENU hMenu=NULL);

  void SetLastMenuRect(HDC hDC, LPRECT pRect);

  CNewMenuItemData* FindMenuItem(UINT nID);
  CNewMenu* FindMenuOption(int nId, int& nLoc);
  CNewMenu* FindMenuOption(LPCTSTR lpstrText, int& nLoc);

  CNewMenu* FindAnotherMenuOption(int nId, int& nLoc, CArray<CNewMenu*,CNewMenu*>&newSubs, CArray<int,int&>&newLocs);

  CNewMenuItemData* NewODMenu(UINT pos, UINT nFlags, UINT nID, LPCTSTR string);

  void SynchronizeMenu();
  void InitializeMenuList(int value);
  void DeleteMenuList();
  
  CNewMenuItemData* FindMenuList(UINT nID);
  CNewMenuItemData* CheckMenuItemData(ULONG_PTR nItemData) const;

  void DrawSpecial_OldStyle(CDC* pDC, LPCRECT pRect, UINT nID, DWORD dwStyle);
  void DrawSpecial_WinXP(CDC* pDC, LPCRECT pRect, UINT nID, DWORD dwStyle);

  void DrawSpecialCharStyle(CDC* pDC, LPCRECT pRect, TCHAR Sign, DWORD dwStyle);
  void DrawSpecialChar(CDC* pDC, LPCRECT pRect, TCHAR Sign, BOOL bBold);

  void DrawMenuTitle(LPDRAWITEMSTRUCT lpDIS, BOOL bIsMenuBar);

  // Measure an item
  void MeasureItem_OldStyle( LPMEASUREITEMSTRUCT lpMIS, BOOL bIsMenuBar); 
  void DrawItem_OldStyle (LPDRAWITEMSTRUCT lpDIS, BOOL bIsMenubar);

  void MeasureItem_Icy( LPMEASUREITEMSTRUCT lpMIS, BOOL bIsMenuBar); 
  void DrawItem_Icy (LPDRAWITEMSTRUCT lpDIS, BOOL bIsMenubar);
  BOOL Draw3DCheckmark(CDC* dc, CRect rc, HBITMAP hbmpChecked, HBITMAP hbmpUnchecked, DWORD dwState);

  void MeasureItem_WinXP( LPMEASUREITEMSTRUCT lpMIS, BOOL bIsMenuBar); 
  void DrawItem_WinXP (LPDRAWITEMSTRUCT lpDIS, BOOL bIsMenuBar);

  void DrawItem_XP_2003 (LPDRAWITEMSTRUCT lpDIS, BOOL bIsMenuBar);

  void DrawItem_SpecialStyle (LPDRAWITEMSTRUCT lpDIS, BOOL bIsMenubar);

//  BOOL ImageListDuplicate(CImageList* il,int xoffset,CImageList* newlist);
  void ColorBitmap(CDC* pDC, CBitmap& bmp, CSize size, COLORREF fill, COLORREF border, int hatchstyle=-1);
  
// Member Variables
public:
  static DWORD m_dwLastActiveItem;

protected: 
  // Stores list of menu items
  CTypedPtrArray<CPtrArray, CNewMenuItemData*> m_MenuItemList;   
  // When loading an owner-drawn menu using a Resource, CNewMenu must keep track of
  // the popup menu's that it creates. Warning, this list *MUST* be destroyed
  // last item first :)
  // Stores list of sub-menus
  CTypedPtrArray<CPtrArray, HMENU>  m_SubMenus;

  static BOOL m_bEnableXpBlending;
  static BOOL m_bNewMenuBorderAllMenu;
  static BOOL m_bSelectDisable;
  static CMenuTheme* m_pActMenuDrawing;
  static LOGFONT m_MenuTitleFont;
  static CTypedPtrList<CPtrList, CNewMenuIcons*>* m_pSharedMenuIcons;

  int m_iconX;
  int m_iconY;

  HWND m_hTempOwner;

  COLORREF m_bitmapBackground;

  CImageList* m_checkmaps;
  BOOL m_checkmapsshare;

  int m_selectcheck;
  int m_unselectcheck;

  BOOL m_bDynIcons;

  HMENU m_hParentMenu;

  BOOL m_bIsPopupMenu;

  CRect m_LastActiveMenuRect;

  DWORD m_dwOpenMenu;

  void* m_pData;
};

/////////////////////////////////////////////////////////////////////////////
// CMenuFrame<> template for easy using of the new menu

template<class baseClass>
class CMenuFrame : public baseClass
{
  typedef CMenuFrame<baseClass> MyNewFrame;
public:
  CNewMenu m_DefaultNewMenu;
  CNewMenu m_SystemNewMenu;

public:

  CMenuFrame()
  {
    m_bInMenuLoop = FALSE;
    m_TimerID = NULL;
    m_menubarItemIndex = UINT(-1);
  }

  // control bar docking
  void EnableDocking(DWORD dwDockStyle);

private:
  static const AFX_MSGMAP_ENTRY _messageEntries[];

protected: 
  static const AFX_MSGMAP messageMap;

  BOOL m_bInMenuLoop;
  UINT m_TimerID;
  UINT m_menubarItemIndex;


#if _MFC_VER < 0x0700 
  static const AFX_MSGMAP* PASCAL _GetBaseMessageMap()
  { 
    return &baseClass::messageMap; 
  };
#else
  static const AFX_MSGMAP* PASCAL GetThisMessageMap()
  {
    return &CMenuFrame<baseClass>::messageMap; 
  }
#endif

  virtual const AFX_MSGMAP* GetMessageMap() const 
  {
    return &CMenuFrame<baseClass>::messageMap; 
  }

  static const AFX_MSGMAP_ENTRY* GetMessageEntries()
  {
    static const AFX_MSGMAP_ENTRY Entries[] =
    {
      ON_WM_MEASUREITEM()
      ON_WM_MENUCHAR()
      ON_WM_INITMENUPOPUP()
      ON_WM_ENTERMENULOOP()
      ON_WM_EXITMENULOOP() 
      ON_WM_TIMER()
      ON_WM_CREATE()
      ON_WM_NCHITTEST()
      ON_WM_DESTROY()
      ON_WM_SYSCOLORCHANGE()
#ifdef USE_NEW_DOCK_BAR
      ON_WM_NCPAINT()
      ON_WM_PAINT()
      ON_WM_ACTIVATEAPP()
      ON_WM_ACTIVATE()
#endif //USE_NEW_DOCK_BAR
      {0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 }
    }; 
    return Entries;
  }

  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct)
  {
    if (baseClass::OnCreate(lpCreateStruct) == -1)
      return -1;

    if(m_SystemNewMenu.m_hMenu)
    {
      ::DestroyMenu(m_SystemNewMenu.Detach());
    }
    m_SystemNewMenu.Attach(::GetSystemMenu(m_hWnd,FALSE));
    return 0;
  }

  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
  {
    baseClass::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
    CNewMenu::OnInitMenuPopup(m_hWnd,pPopupMenu, nIndex, bSysMenu);
  }

  afx_msg void OnSysColorChange() 
  {
    baseClass::OnSysColorChange();
    CNewMenu::OnSysColorChange();
    UpdateMenuBarColor(m_DefaultNewMenu);
  }

  afx_msg void OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
  {
    m_bInMenuLoop = TRUE;
    if(m_TimerID!=NULL)
    {
      KillTimer(m_TimerID);
      m_TimerID = NULL;
    }
    if (m_menubarItemIndex!=UINT(-1))
    {
      DrawMenubarItem(this,GetMenu(),m_menubarItemIndex,NULL);
      m_menubarItemIndex=UINT(-1);
    }
    baseClass::OnEnterMenuLoop(bIsTrackPopupMenu);
  }

  afx_msg void OnExitMenuLoop(BOOL bIsTrackPopupMenu)
  {
    m_bInMenuLoop = FALSE;
    baseClass::OnExitMenuLoop(bIsTrackPopupMenu);
  }

  afx_msg void OnTimer(UINT nIDEvent)
  {
    baseClass::OnTimer(nIDEvent);
    if(m_TimerID==nIDEvent)
    {   
      CPoint pt;
      GetCursorPos(&pt);
      SendMessage(WM_NCHITTEST, 0, MAKELONG(pt.x, pt.y));
    }
  }

  afx_msg void OnDestroy()
  {
    if(m_TimerID!=NULL)
    {
      KillTimer(m_TimerID);
      m_TimerID = NULL;
    }
    baseClass::OnDestroy();
  }

  afx_msg UINT OnNcHitTest(CPoint point)
  {
    UINT nHitCode = baseClass::OnNcHitTest(point);
    // Test Win95/98/me and Win NT 4.0
    if(g_Shell<Win2000 || bRemoteSession)
    {
      UINT nStatus;
      UINT nHotlightStatus;

      if( IsChild(GetFocus()) )
      { 
        nStatus = 0; 
        nHotlightStatus = ODS_HOTLIGHT;
      }
      else if(g_Shell==Win95 || g_Shell==WinNT4)
      { // Win95 or winNt 4.0 do not have an inactive menubar
        nStatus = 0; 
        nHotlightStatus = 0;
      }
      else
      { 
        nStatus = ODS_INACTIVE;
        nHotlightStatus = ODS_HOTLIGHT|ODS_INACTIVE;
      }

      CNewMenu* pNewMenu = DYNAMIC_DOWNCAST(CNewMenu,GetMenu());
      if (!m_bInMenuLoop && nHitCode == HTMENU)
      {
        // I support only CNewMenu ownerdrawings menu!!
        if(pNewMenu)
        {
          UINT nItemIndex = MenuItemFromPoint(m_hWnd, pNewMenu->m_hMenu, point);

          if ( nItemIndex!=(UINT)-1 )
          {
            if(m_menubarItemIndex!=nItemIndex)
            { 
              // Clear the old Item
              DrawMenubarItem(this,pNewMenu,m_menubarItemIndex,nStatus);

              // Draw the hotlight item.
              if(DrawMenubarItem(this,pNewMenu,nItemIndex,nHotlightStatus))
              {
                // Set a new Timer
                if(m_TimerID==NULL)
                {
                  m_TimerID=GetSafeTimerID(m_hWnd,100);
                }
                m_menubarItemIndex = nItemIndex;
              }
              else
              {
                m_menubarItemIndex = UINT(-1);
              }
            }
            else
            {
              // Draw the hotlight item again.
              if(CNewMenu::m_dwLastActiveItem==NULL && 
                 DrawMenubarItem(this,pNewMenu,nItemIndex,nHotlightStatus))
              {
                // Set a new Timer
                if(m_TimerID==NULL)
                {
                  m_TimerID=GetSafeTimerID(m_hWnd,100);
                }
                m_menubarItemIndex = nItemIndex;
              }
            }
            return nHitCode;
          }
        }
      }

      if (m_menubarItemIndex!=UINT(-1))
      {
        DrawMenubarItem(this,pNewMenu,m_menubarItemIndex,nStatus);
        m_menubarItemIndex=UINT(-1);
      }
      if(m_TimerID!=NULL)
      {
        KillTimer(m_TimerID);
        m_TimerID=NULL;
      }
    }
    return nHitCode;
  }

  afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMIS) 
  {
    if(!CNewMenu::OnMeasureItem(GetCurrentMessage()))
    {
      baseClass::OnMeasureItem(nIDCtl, lpMIS);
    }
  } 

  afx_msg LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
  {
    LRESULT lresult;
    if( DYNAMIC_DOWNCAST(CNewMenu,pMenu) )
      lresult=CNewMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
    else
      lresult=baseClass::OnMenuChar(nChar, nFlags, pMenu);
    
    return lresult;
  }

#ifdef USE_NEW_DOCK_BAR
  afx_msg void OnNcPaint()
  {
    baseClass::OnNcPaint();
    DrawSmallBorder();
  }
  
  afx_msg void OnPaint()
  {
    baseClass::OnPaint();
    DrawSmallBorder();
  }

#if _MFC_VER < 0x0700 
 	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask)
  {
    baseClass::OnActivateApp(bActive, hTask);
    DrawSmallBorder();
  }
#else
  afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID)
  {
    baseClass::OnActivateApp(bActive, dwThreadID);
    DrawSmallBorder();
  }
#endif

  afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized )
  {
    baseClass::OnActivate(nState,pWndOther, bMinimized );
    DrawSmallBorder();
  }

  void DrawSmallBorder()
  {
    // To eliminate small border between menu and client rect
    MENUINFO menuInfo = {0};
    menuInfo.cbSize = sizeof(menuInfo);
    menuInfo.fMask = MIM_BACKGROUND;
    if(::GetMenuInfo(::GetMenu(m_hWnd),&menuInfo) && menuInfo.hbrBack)
    {
      CDC* pDC = GetWindowDC(); 
      CRect clientRect;
      GetClientRect(clientRect);
      ClientToScreen(clientRect);
      CRect windowRect;
      GetWindowRect(windowRect);
      CRect rect(clientRect.left-windowRect.left,clientRect.top-windowRect.top-1,clientRect.right-windowRect.left,clientRect.top-windowRect.top);

      CBrush *pBrush = CBrush::FromHandle(menuInfo.hbrBack);
      // need for win95/98/me
      VERIFY(pBrush->UnrealizeObject());
      CPoint oldOrg = pDC->SetBrushOrg(0,0);
      pDC->FillRect(rect,pBrush);
      pDC->SetBrushOrg(oldOrg);
      // must be called
      ReleaseDC(pDC);
    }
  }
#endif
};

#ifdef _AFXDLL
  #if _MFC_VER < 0x0700 
    template<class baseClass>
    const AFX_MSGMAP CMenuFrame<baseClass>::messageMap = { &CMenuFrame<baseClass>::_GetBaseMessageMap, GetMessageEntries()};
  #else
    template<class baseClass>
    const AFX_MSGMAP CMenuFrame<baseClass>::messageMap = { &baseClass::GetThisMessageMap, GetMessageEntries()};
  #endif
#else
  template<class baseClass>
  const AFX_MSGMAP CMenuFrame<baseClass>::messageMap = { &baseClass::messageMap, GetMessageEntries()};
#endif

/////////////////////////////////////////////////////////////////////////////
// CMenuMiniDockFrameWnd for docking toolbars with new menu

class CMenuMiniDockFrameWnd: public CMenuFrame<CMiniDockFrameWnd> 
{
  DECLARE_DYNCREATE(CMenuMiniDockFrameWnd) 
};

// control bar docking
template<class baseClass>
void CMenuFrame<baseClass>::EnableDocking(DWORD dwDockStyle)
{
  baseClass::EnableDocking(dwDockStyle);
  // Owerite registering for floating frame
  m_pFloatingFrameClass = RUNTIME_CLASS(CMenuMiniDockFrameWnd);
}


/////////////////////////////////////////////////////////////////////////////
// CMenuDialog for dialog implementation

class CMenuDialog : public CMenuFrame<CDialog>
{
  DECLARE_DYNAMIC(CMenuDialog);

public:
  CMenuDialog();
  CMenuDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
  CMenuDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);

  // Overridables (special message map entries)
  virtual BOOL OnInitDialog();

protected:
  // Generated message map functions
  //{{AFX_MSG(CMenuDialog)
  afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
  //}}AFX_MSG

  DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CMenuMiniFrameWnd for menu to documents

class CMenuMiniFrameWnd : public CMenuFrame<CMiniFrameWnd>
{
  DECLARE_DYNCREATE(CMenuMiniFrameWnd)
};

/////////////////////////////////////////////////////////////////////////////
// CMenuMDIChildWnd for menu to documents

class CMenuMDIChildWnd : public CMenuFrame<CMDIChildWnd>
{
  DECLARE_DYNCREATE(CMenuMDIChildWnd)
};

/////////////////////////////////////////////////////////////////////////////
// CMenuFrameWnd for menu to documents

class CMenuFrameWnd : public CMenuFrame<CFrameWnd>
{
  DECLARE_DYNCREATE(CMenuFrameWnd)

public:
#if _MFC_VER < 0x0700 
    // dynamic creation - load frame and associated resources
  virtual BOOL LoadFrame(UINT nIDResource,
        DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
        CWnd* pParentWnd = NULL,
        CCreateContext* pContext = NULL);
#endif

  // under MFC 7.0 the next function is virtual so we don't neet to owerwrite
  // loadframe
  BOOL Create(LPCTSTR lpszClassName,
        LPCTSTR lpszWindowName,
        DWORD dwStyle = WS_OVERLAPPEDWINDOW,
        const RECT& rect = rectDefault,
        CWnd* pParentWnd = NULL,        // != NULL for popups
        LPCTSTR lpszMenuName = NULL,
        DWORD dwExStyle = 0,
        CCreateContext* pContext = NULL);

#ifdef USE_NEW_DOCK_BAR
  // control bar docking
  void EnableDocking(DWORD dwDockStyle);
#endif
};

/////////////////////////////////////////////////////////////////////////////
// CMenuMDIFrameWnd for menu to documents

class CMenuMDIFrameWnd : public CMenuFrame<CMDIFrameWnd>
{
  DECLARE_DYNCREATE(CMenuMDIFrameWnd);
  HMENU m_hShowMenu;

public: 
  CMenuMDIFrameWnd():m_hShowMenu(NULL){};

  BOOL ShowMenu(BOOL bShow);

#if _MFC_VER < 0x0700 
    // dynamic creation - load frame and associated resources
  virtual BOOL LoadFrame(UINT nIDResource,
        DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,
        CWnd* pParentWnd = NULL,
        CCreateContext* pContext = NULL);
#endif
    // under MFC 7.0 the next function is virtual so we don't neet to owerwrite
    // loadframe
    BOOL Create(LPCTSTR lpszClassName,
        LPCTSTR lpszWindowName,
        DWORD dwStyle = WS_OVERLAPPEDWINDOW,
        const RECT& rect = rectDefault,
        CWnd* pParentWnd = NULL,        // != NULL for popups
        LPCTSTR lpszMenuName = NULL,
        DWORD dwExStyle = 0,
        CCreateContext* pContext = NULL);

#ifdef USE_NEW_DOCK_BAR
  // control bar docking
  void EnableDocking(DWORD dwDockStyle);
#endif
};


/////////////////////////////////////////////////////////////////////////////
// CMenuMultiDocTemplate for menu to documents

class CMenuMultiDocTemplate: public CMultiDocTemplate
{
  DECLARE_DYNAMIC(CMenuMultiDocTemplate)

public:
  CNewMenu m_NewMenuShared;

// Constructors
public:
  CMenuMultiDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
                       CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass);

  ~CMenuMultiDocTemplate();
};

/////////////////////////////////////////////////////////////////////////////
// CMenuMemDC helperclass for drawing off screen 

class CMenuMemDC: public CDC
{
  CRect m_rect;
  CBitmap* m_pOldBitmap;
  CBitmap  m_memBitmap;
  BOOL m_bCancel;

  HDC m_hDcOriginal;

public:
  CMenuMemDC(LPCRECT pRect, HDC hdc):m_rect(pRect),m_bCancel(FALSE),m_hDcOriginal(hdc)
  {
    CDC *pOrgDC = CDC::FromHandle(m_hDcOriginal);
    CreateCompatibleDC(pOrgDC);

    m_memBitmap.CreateCompatibleBitmap (pOrgDC,m_rect.Width (),m_rect.Height());
    m_pOldBitmap = SelectObject (&m_memBitmap);
    SetWindowOrg(m_rect.left, m_rect.top);
  }

  // Abborting to copy image from memory dc to client
  void DoCancel(){m_bCancel=TRUE;}

  ~CMenuMemDC()
  {
    if(!m_bCancel)
    {
      CDC *pOrgDC = CDC::FromHandle(m_hDcOriginal);
      pOrgDC->BitBlt (m_rect.left,m_rect.top,m_rect.Width (),m_rect.Height (),this,m_rect.left,m_rect.top,SRCCOPY);
    }
    SelectObject (m_pOldBitmap);
  }
};

/////////////////////////////////////////////////////////////////////////////
// CNewMenuHelper for enabling / disabling menu border, replacing system menu.

#define  NEW_MENU_DIALOG_SUBCLASS     1
#define  NEW_MENU_DIALOG_SYSTEM_MENU  2
#define  NEW_MENU_DEFAULT_BORDER      4
#define  NEW_MENU_CHANGE_MENU_STYLE   8

class CNewMenuHelper
{
private:
  DWORD m_dwOldFlags;
  int m_OldMenuDrawStyle;

public: 
  CNewMenuHelper(DWORD dwFlags);
  CNewMenuHelper(CNewMenu::EDrawStyle setTempStyle);

  ~CNewMenuHelper();
};

/////////////////////////////////////////////////////////////////////////////
// CMenuDockBar support for office2003 colors.
class CMenuDockBar : public CDockBar
{
	DECLARE_DYNCREATE(CMenuDockBar)
public:
  CMenuDockBar(BOOL bFloating = FALSE);   // TRUE if attached to CMiniDockFrameWnd
  virtual ~CMenuDockBar();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:

  void EraseNonClient();

  afx_msg BOOL OnEraseBkgnd(CDC* pDC);
  afx_msg void OnNcPaint();
};

#pragma warning(pop)

#endif // __CNewMenu_H_
