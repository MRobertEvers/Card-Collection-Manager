#pragma once
#include <wx/wxprec.h>
#include <map>
#include <memory>
#include <wx/listctrl.h>

class CardInterface;
class CollectionInterface;

class vcCollectionDeckBoxItemList : public wxPanel
{
public:
   enum
   {
      List = 0x0
   };

   vcCollectionDeckBoxItemList( wxWindow* aptParent, 
                                std::shared_ptr<CollectionInterface> aptInt );
   ~vcCollectionDeckBoxItemList();

   void RefreshList();
   CardInterface GetItemByListIndex(int Ind);
   CardInterface GetFirst();
   int GetFirstInt();
   bool IsEmpty();
private:
   // This is so we can populate event data before sending it up.
   wxDECLARE_EVENT_TABLE();

   std::vector<CardInterface*> m_vecDataItemsDisplayOrder;
   wxListCtrl* m_wxListControl;
   std::shared_ptr<CollectionInterface> m_ptColInt;

   int m_iSelection;

   void onItemSelection(wxListEvent& awxEvt);
   void onItemDeselection(wxListEvent& awxEvt);
   std::map<wxString, std::vector<CardInterface*>> 
      performGrouping(std::vector<CardInterface>& avecItems);

   void displayGrouping(const std::map<wxString,std::vector<CardInterface*>> &amapGrouping);
   void addListItem( CardInterface& aData);
};

