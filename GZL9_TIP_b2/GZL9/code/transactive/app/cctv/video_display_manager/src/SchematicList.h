#pragma once

#include <string>
#include <vector>
#include <map>
#include <utility>


namespace TA_IRS_App
{
  // SchematicList

  class SchematicList : public CListBox
  {
      DECLARE_DYNAMIC(SchematicList)

    public:
      SchematicList();
      virtual ~SchematicList();
      void changeLocation(unsigned long locationKey);
      unsigned long getCurrentSelLevel();
      unsigned long getFirstItemLevel();
    protected:
      DECLARE_MESSAGE_MAP()
    private:
      typedef std::pair<unsigned long, std::string> ListPair;
      typedef std::vector<ListPair> ListPairs;
      typedef std::map<unsigned long, ListPairs> LocationToListPairsMap;
      LocationToListPairsMap m_locationToListPairsMap;
  };
}

