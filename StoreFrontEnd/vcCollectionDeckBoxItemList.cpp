#include "vcCollectionDeckBoxItemList.h"
#include "StoreFront.h"

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
                                     wxDefaultPosition, wxDefaultSize, wxLC_LIST );

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
   for( auto& item : lstCol )
   {
      m_wxListControl->InsertItem( m_wxListControl->GetItemCount(),
                                   item );
   }
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