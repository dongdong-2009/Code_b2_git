// SchematicList.cpp : implementation file
//

#include "stdafx.h"
#include "app/cctv/video_display_manager/src/SchematicList.h"
#include "app/cctv/video_display_manager/src/Configuration.h"
#include "core/data_access_interface/src/ISchematicDisplayMaps.h"
#include "core/data_access_interface/src/SchematicDisplayMapAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include <algorithm>
#include <boost/foreach.hpp>


// SchematicList
namespace TA_IRS_App
{

  IMPLEMENT_DYNAMIC(SchematicList, CListBox)

  SchematicList::SchematicList()
  {
  }

  SchematicList::~SchematicList()
  {
  }


  BEGIN_MESSAGE_MAP(SchematicList, CListBox)
  END_MESSAGE_MAP()

  static bool ISchematicDisplayMapsSortFunc(TA_Base_Core::ISchematicDisplayMaps* i, TA_Base_Core::ISchematicDisplayMaps* j)
  {
    return i->getSubTypeID() < j->getSubTypeID();
  }

  void SchematicList::changeLocation(unsigned long locationKey)
  {
    // Clear the list first
    while(GetCount() > 0)
    {
      DeleteString(0);
    }
    SetHorizontalExtent(0);
    if(m_locationToListPairsMap.find(locationKey) == m_locationToListPairsMap.end())
    {
      std::vector<TA_Base_Core::ISchematicDisplayMaps*> schematicDisplayMaps;
      try
      {
        TA_Base_Core::SchematicDisplayMapAccessFactory::getInstance().getSchematicDisplayAndLabel(locationKey,
			CCTVConfiguration::getinstance().getCCTVSubsystemKey(), schematicDisplayMaps);
      }
      catch(const TA_Base_Core::DatabaseException& e)
      {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", e.what());
        return;
      }
      catch(const TA_Base_Core::DataException& e)
      {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what());
        return;
      }
      std::sort(schematicDisplayMaps.begin(), schematicDisplayMaps.end(), ISchematicDisplayMapsSortFunc);
      m_locationToListPairsMap.insert(std::make_pair(locationKey, ListPairs()));
      BOOST_FOREACH(TA_Base_Core::ISchematicDisplayMaps * i, schematicDisplayMaps)
      {
        m_locationToListPairsMap[locationKey].push_back(ListPair(i->getSubTypeID(), i->getSubTypeLabel()));
        delete i;
      }
    }
    // Fill up the list
    ListPairs& listData = m_locationToListPairsMap[locationKey];
    int index = -1;
    for(ListPairs::iterator it = listData.begin(); it != listData.end(); ++it)
    {
      InsertString(++index, it->second.c_str());
      CSize   sz;
      int     dx = this->GetHorizontalExtent();
      CDC*    pDC = this->GetDC();
      sz = pDC->GetTextExtent(it->second.c_str());
      if(sz.cx > dx)
      {
        dx = sz.cx;
        SetHorizontalExtent(dx);
      }
      SetItemData(index, it->first);
    }
  }

  unsigned long SchematicList::getCurrentSelLevel()
  {
    int curSel = GetCurSel();
    if(LB_ERR == curSel)
    {
      return 0;
    }
    return GetItemData(curSel);
  }

  unsigned long SchematicList::getFirstItemLevel()
  {
    if(GetCount() == 0)
    {
      return 0;
    }
    return GetItemData(0);
  }

  // SchematicList message handlers

}


