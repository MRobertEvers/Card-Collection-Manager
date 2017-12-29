#include "vcCollectionsMenuList.h"
#include "vCollectionsOverview.h"
#include <wx/button.h>

wxBEGIN_EVENT_TABLE(vcCollectionsMenuList, wxPanel)
EVT_BUTTON(vCollectionsOverview::Load_Collection, vcCollectionsMenuList::onViewCollection)
EVT_LIST_ITEM_SELECTED(List, vcCollectionsMenuList::onItemSelection)
EVT_LIST_ITEM_DESELECTED(List, vcCollectionsMenuList::onItemDeselection)
wxEND_EVENT_TABLE()

vcCollectionsMenuList::vcCollectionsMenuList(wxWindow* aptParent)
   : wxPanel(aptParent)
{
   m_iSelection = -1;
   m_wxListControl = new wxListCtrl( this, List,
                                     wxDefaultPosition, wxDefaultSize, wxLC_LIST );

   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   boxSizer->Add(m_wxListControl, 1, wxEXPAND);

   buildButtons();
}


vcCollectionsMenuList::~vcCollectionsMenuList() 
{
   m_wxListControl->Destroy();
}

void 
vcCollectionsMenuList::AddCollectionOption(std::string aszCollectionName)
{
   m_wxListControl->InsertItem( 0, aszCollectionName );
}

void 
vcCollectionsMenuList::buildButtons()
{
   wxButton* loadButt = new wxButton( this, 
                                      vCollectionsOverview::Load_Collection,
                                      "Load Collection");
   wxButton* viewButt = new wxButton( this, 
                                      vCollectionsOverview::View_Collection,
                                      "View Collection");
   wxButton* addButt = new wxButton( this, 
                                     vCollectionsOverview::Add_Collection,
                                     "Add Collection");

   this->GetSizer()->Add(loadButt, wxSizerFlags().Expand());
   this->GetSizer()->Add(viewButt, wxSizerFlags().Expand());
   this->GetSizer()->Add(addButt, wxSizerFlags().Expand());
}

void 
vcCollectionsMenuList::onViewCollection(wxCommandEvent& awxEvt)
{
   awxEvt.SetInt(m_iSelection);

   // This allows for the continued propagation of the event.
   awxEvt.Skip(true);
}

void 
vcCollectionsMenuList::onItemSelection(wxListEvent& awxEvt)
{
   m_iSelection = awxEvt.GetIndex();
}

void 
vcCollectionsMenuList::onItemDeselection(wxListEvent& awxEvt)
{
   m_iSelection = -1;
}