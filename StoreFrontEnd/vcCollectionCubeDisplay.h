#pragma once
#include <wx/wxprec.h>
#include "wx/listctrl.h"
#include "GroupItemData.h"
#include "Group.h"
#include <map>
#include <memory>
#include <vector>

class CollectionInterface;
class CardInterface;
class vcCollectionCubeGroup;

class CubeDisplayColumnSorter : public  Group::SortingOperator
{
public:
   bool operator()( const wxString& agrpLeft, const wxString& agrpRight ) const;
};

class CubeDisplayItemSorter : public  Group::SortingOperator
{
public:
   bool operator()( const wxString& agrpLeft, const wxString& agrpRight ) const;
};


class vcCollectionCubeDisplay : public wxScrolledWindow
{
public:
   enum
   {
      Group_List = 0x09
   };
   vcCollectionCubeDisplay( wxPanel* aptParent, wxWindowID aiWID, 
                            std::shared_ptr<CollectionInterface> aptInt );
   ~vcCollectionCubeDisplay();

   void RefreshList();
   CardInterface GetItemByListIndex( int Ind );
   CardInterface GetFirst();
   int GetFirstInt();
   bool IsEmpty();

private:
   wxDECLARE_EVENT_TABLE();

   std::map<wxString, CardInterface> m_mapItemData;
   std::map<int, vcCollectionCubeGroup*> m_mapColGroups;
   std::shared_ptr<CollectionInterface> m_ptColInt;

   void clearDisplay();
   Group defaultGroup();
   wxColour getGroupColor( const wxString& aszGroup );
   wxColour getGroupFontColor( const wxString& aszGroup );

   std::map<wxString, 
            std::vector<CardInterface*>,
      Group::Sorting> performGrouping( std::vector<CardInterface>& avecItems,
                                                       const Group& aGrp );
};