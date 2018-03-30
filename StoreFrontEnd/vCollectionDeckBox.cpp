#include "vCollectionDeckBox.h"
#include "vcCollectionDeckBoxItemList.h"
#include "viCollectionEditor.h"
#include "viCardEditor.h"
#include "GroupItemData.h"
#include "MainWindow.h"
#include "viCardEditor.h"
#include "CollectionInterface.h"

wxBEGIN_EVENT_TABLE(vCollectionDeckBox, wxPanel)
EVT_BUTTON( viCardEditor::Changes_Submit, vCollectionDeckBox::onCardChanged )
EVT_BUTTON(viCollectionEditor::Changes_Accept, vCollectionDeckBox::onEditorAccept)
EVT_BUTTON( viCardEditor::Image_Changed, vCollectionDeckBox::onNewItemSelectView )
EVT_LIST_ITEM_SELECTED(vcCollectionDeckBoxItemList::List, vCollectionDeckBox::onNewItemSelected)
EVT_BUTTON(viCollectionEditor::Changes_Decline, vCollectionDeckBox::onEditorDecline)
wxEND_EVENT_TABLE()

vCollectionDeckBox::vCollectionDeckBox( IMenuEventSource* aptParent,
                                        wxWindowID aiWID,
                                        const wxString& aszColID )
   : ivCollectionView(aptParent, aiWID, aszColID)
{
   PrepareInterface();
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer(1, 3, 0, 0);
   boxSizer->AddGrowableCol(0);
   boxSizer->AddGrowableRow(0);
   // Order is Count, Name*, Mana Cost, Card Type
   this->SetSizer(boxSizer);

   this->GetParent()->SetMinSize( wxSize( 550, 500 ) );
   this->GetParent()->Layout();
   buildItemList();

   if( !m_vcItemList->IsEmpty() )
   {
      notifyCardEditor(m_vcItemList->GetFirst().GetHash() );
   }
}


vCollectionDeckBox::~vCollectionDeckBox()
{

}

void 
vCollectionDeckBox::PrepareInterface()
{
   Query query;
   query.UIDs();
   query.IncludeCount();

   m_ptCollectionInterface->PrepareInterface( query );
}


void 
vCollectionDeckBox::onNewItemSelected(wxListEvent& awxEvt)
{
   notifyCardEditor( awxEvt.GetString() );
   awxEvt.StopPropagation();
}

void
vCollectionDeckBox::onEditorAccept( wxCommandEvent& awxEvt )
{
   m_vcItemList->RefreshList();
   this->ivCollectionView::onEditorAccept( awxEvt );
}

void 
vCollectionDeckBox::onCardChanged( wxCommandEvent& awxEvt )
{
   m_vcItemList->RefreshList();
   this->ivCollectionView::onCardChanged( awxEvt );
}


void 
vCollectionDeckBox::buildItemList()
{
   m_vcItemList = new vcCollectionDeckBoxItemList(this, m_ptCollectionInterface);
   this->GetSizer()->Add(m_vcItemList, wxSizerFlags(1).Expand());
}

