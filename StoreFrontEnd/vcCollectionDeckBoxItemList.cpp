#include "vcCollectionDeckBoxItemList.h"
#include "StoreFront.h"
#include "vcdCDBIListItemData.h"

wxBEGIN_EVENT_TABLE(vcCollectionDeckBoxItemList, wxPanel)
EVT_LIST_ITEM_SELECTED(List, vcCollectionDeckBoxItemList::onItemSelection)
EVT_LIST_ITEM_DESELECTED(List, vcCollectionDeckBoxItemList::onItemDeselection)
wxEND_EVENT_TABLE()

vcCollectionDeckBoxItemList::vcCollectionDeckBoxItemList( wxWindow* aptParent,
                                                          const wxString& aszColID )
   : wxPanel(aptParent)
{
   m_wxszColID = aszColID;

   m_iSelection = -1;
   m_wxListControl = new wxListCtrl( this, List,
                                     wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
   m_wxListControl->InsertColumn(0, "#");
   m_wxListControl->InsertColumn(1, "Name");
   m_wxListControl->InsertColumn(2, "Mana Cost");
   m_wxListControl->InsertColumn(3, "Card Type");
   m_wxListControl->InsertColumn(4, "Set");

   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   boxSizer->Add(m_wxListControl, 1, wxEXPAND);

   RefreshList();
}


vcCollectionDeckBoxItemList::~vcCollectionDeckBoxItemList()
{

}

void 
vcCollectionDeckBoxItemList::RefreshList()
{
   auto ptSF = StoreFrontEnd::Instance();
   auto lstCol = ptSF->GetCollectionList( m_wxszColID.ToStdString(),
                                          0 /*Default HashType is 7*/ );
   vector<vcdCDBIListItemData> vecData;
   for( auto& szItem : lstCol )
   {
      vcdCDBIListItemData data(szItem, vcdCDBIListItemData::LONG_NAME);
      vecData.push_back(data);
   }

   m_wxListControl->Freeze();
   m_wxListControl->DeleteAllItems();
   for( auto& item : vecData )
   {
      addListItem(item);
   }
   m_wxListControl->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
   m_wxListControl->SetColumnWidth(1, wxLIST_AUTOSIZE);
   m_wxListControl->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
   m_wxListControl->SetColumnWidth(3, wxLIST_AUTOSIZE);
   m_wxListControl->SetColumnWidth(4, wxLIST_AUTOSIZE);
   m_wxListControl->Thaw();
}

void
vcCollectionDeckBoxItemList::onItemSelection(wxListEvent& awxEvt)
{
   m_iSelection = awxEvt.GetIndex();
   awxEvt.StopPropagation();
}

void
vcCollectionDeckBoxItemList::onItemDeselection(wxListEvent& awxEvt)
{
   m_iSelection = -1;
   awxEvt.StopPropagation();
}

void 
vcCollectionDeckBoxItemList::addListItem(vcdCDBIListItemData& aData)
{
   wxString buf = std::to_string(aData.GetNumber());
   // buf.Printf(wxT("This is item %d"), i);
   int i = m_wxListControl->GetItemCount();
   long tmp = m_wxListControl->InsertItem(i, buf, 0);
   m_wxListControl->SetItemData(tmp, i);

   buf = aData.GetName();
   m_wxListControl->SetItem(tmp, 1, buf);

   buf = aData.GetManaCost();
   m_wxListControl->SetItem(tmp, 2, buf);

   buf = aData.GetCardType();
   m_wxListControl->SetItem(tmp, 3, buf);

   buf = aData.GetSet();
   m_wxListControl->SetItem(tmp, 4, buf);
}