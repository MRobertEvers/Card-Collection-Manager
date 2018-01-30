#include "vCollectionDeckBox.h"
#include "vcCollectionDeckBoxItemList.h"
#include "viCollectionEditor.h"
#include "viCardEditor.h"
#include "vcdCDBIListItemData.h"

wxBEGIN_EVENT_TABLE(vCollectionDeckBox, wxPanel)
EVT_BUTTON(viCollectionEditor::Changes_Accept, vCollectionDeckBox::onEditorAccept)
EVT_LIST_ITEM_SELECTED(vcCollectionDeckBoxItemList::List, vCollectionDeckBox::onNewItemSelected)
// EVT_BUTTON(viCollectionEditor::Changes_Decline, viCollectionEditor::onDecline)
wxEND_EVENT_TABLE()

vCollectionDeckBox::vCollectionDeckBox( wxWindow* aptParent, 
                                        wxWindowID aiWID,
                                        const wxString& aszColID )
   : wxPanel(aptParent, aiWID)
{
   m_wxszColID = aszColID;
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer(1, 3, 0, 0);
   boxSizer->AddGrowableCol(0);
   boxSizer->AddGrowableRow(0);
   // Order is Count, Name*, Mana Cost, Card Type
   this->SetSizer(boxSizer);

   buildItemList();

   ShowCollectionEditor();
}


vCollectionDeckBox::~vCollectionDeckBox()
{

}

void 
vCollectionDeckBox::ShowCollectionEditor()
{
   //auto viColEd = new viCollectionEditor(this, 4, m_wxszColID);
   //this->GetSizer()->Add(viColEd, wxSizerFlags(1).Center().Shaped());
   if( !m_vcItemList->IsEmpty() )
   {
      notifyCardEditor(m_vcItemList->GetItem(0).GetHash());
   }
}

void 
vCollectionDeckBox::onEditorAccept(wxCommandEvent& awxEvt)
{
   m_vcItemList->RefreshList();
}

void 
vCollectionDeckBox::onNewItemSelected(wxListEvent& awxEvt)
{
   auto itemData = m_vcItemList->GetItem(awxEvt.GetIndex());
   notifyCardEditor(itemData.GetHash());
}

void 
vCollectionDeckBox::buildItemList()
{
   m_vcItemList = new vcCollectionDeckBoxItemList(this, m_wxszColID);
   this->GetSizer()->Add(m_vcItemList, wxSizerFlags(1).Expand());
}

void 
vCollectionDeckBox::notifyCardEditor(const wxString& aszHash)
{
   this->Freeze();
   if( m_viCardEditor == nullptr )
   {
      m_viCardEditor = new viCardEditor(this, 5, m_wxszColID, aszHash);
      this->GetSizer()->Add(m_viCardEditor, wxSizerFlags(1).Shaped().FixedMinSize());
   }
   else
   {
      m_viCardEditor->DisplayNew(m_wxszColID, aszHash);
   }
   this->Thaw();
   PostSizeEvent();
}