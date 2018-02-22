#include "vcCollectionsMenuList.h"
#include "vCollectionsOverview.h"
#include "StoreFrontEnd.h"
#include <wx/button.h>
#include <wx/listctrl.h>

wxBEGIN_EVENT_TABLE(vcCollectionsMenuList, wxPanel)
EVT_LIST_ITEM_SELECTED(List, vcCollectionsMenuList::onItemSelection)
EVT_LIST_ITEM_DESELECTED(List, vcCollectionsMenuList::onItemDeselection)
EVT_BUTTON( vCollectionsOverview::View_Collection, vcCollectionsMenuList::OnViewCollection )
EVT_BUTTON( vCollectionsOverview::Add_Collection, vcCollectionsMenuList::OnAddCollection )
wxEND_EVENT_TABLE()

vcCollectionsMenuList::vcCollectionsMenuList(wxWindow* aptParent)
   : wxPanel(aptParent)
{
   m_iSelection = -1;
   m_wxListControl = new wxListView( this, List,
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
   FocusLatest();
}

void
vcCollectionsMenuList::OnViewCollection( wxCommandEvent& awxEvt )
{
   awxEvt.SetString( GetSelectionText() );
   if( !awxEvt.GetString().IsEmpty() )
   {
      awxEvt.Skip();
   }
}

void 
vcCollectionsMenuList::OnAddCollection( wxCommandEvent& awxEvt )
{
   wxString szNewCollection =
      wxGetTextFromUser( "Enter the name of your new collection.",
                         "New Collection", 
                         "MyDeck" );

   auto ptSF = StoreFrontEnd::Instance();
   auto szID = ptSF->CreateNewCollection( szNewCollection.ToStdString(), "" );
   if( szID != "" )// TODO: Does this return NF?
   {
      AddCollectionOption( szNewCollection.ToStdString() );
   }
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
vcCollectionsMenuList::FocusLatest()
{
   auto selectedItem = m_wxListControl->GetFirstSelected();
   while( selectedItem != -1 )
   {
      m_wxListControl->Select( selectedItem, false );
      selectedItem = m_wxListControl->GetNextSelected(selectedItem);
   }
   m_wxListControl->Select(m_wxListControl->GetItemCount()-1);
   m_wxViewButton->SetFocus();
}

void 
vcCollectionsMenuList::buildButtons()
{
   wxButton* loadButt = new wxButton( this, 
                                      vCollectionsOverview::Load_Collection,
                                      "Load Collection" );
   m_wxViewButton = new wxButton( this,
                                  vCollectionsOverview::View_Collection,
                                  "View Collection" );
   wxButton* addButt = new wxButton( this, 
                                     vCollectionsOverview::Add_Collection,
                                     "Add Collection" );

   this->GetSizer()->Add(loadButt, wxSizerFlags().Expand());
   this->GetSizer()->Add( m_wxViewButton, wxSizerFlags().Expand());
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