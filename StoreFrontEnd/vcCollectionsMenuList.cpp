#include "vcCollectionsMenuList.h"
#include "vCollectionsOverview.h"
#include <wx/button.h>

wxBEGIN_EVENT_TABLE(vcCollectionsMenuList, wxPanel)
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
   m_wxListControl->InsertItem( m_wxListControl->GetItemCount(),
                                aszCollectionName );
}

wxString 
vcCollectionsMenuList::GetSelectionText()
{
   if( m_iSelection >= 0 )
   {
      return m_wxListControl->GetItemText( m_iSelection );
   }
   else
   {
      return "";
   }
}

void 
vcCollectionsMenuList::buildButtons()
{
   wxButton* loadButt = new wxButton( this, 
                                      vCollectionsOverview::Load_Collection,
                                      "Load Collection" );
   wxButton* viewButt = new wxButton( this, 
                                      vCollectionsOverview::View_Collection,
                                      "View Collection" );
   wxButton* addButt = new wxButton( this, 
                                     vCollectionsOverview::Add_Collection,
                                     "Add Collection" );

   this->GetSizer()->Add(loadButt, wxSizerFlags().Expand());
   this->GetSizer()->Add(viewButt, wxSizerFlags().Expand());
   this->GetSizer()->Add(addButt, wxSizerFlags().Expand());
}

void 
vcCollectionsMenuList::onItemSelection(wxListEvent& awxEvt)
{
   m_iSelection = awxEvt.GetIndex();
   awxEvt.StopPropagation();
}

void 
vcCollectionsMenuList::onItemDeselection(wxListEvent& awxEvt)
{
   m_iSelection = -1;
   awxEvt.StopPropagation();
}