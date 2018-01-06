#include "vCollectionDeckBox.h"
#include "vcCollectionDeckBoxItemList.h"
#include "viCollectionEditor.h"

wxBEGIN_EVENT_TABLE(vCollectionDeckBox, wxPanel)
EVT_BUTTON(viCollectionEditor::Changes_Accept, vCollectionDeckBox::onEditorAccept)
// EVT_BUTTON(viCollectionEditor::Changes_Decline, viCollectionEditor::onDecline)
wxEND_EVENT_TABLE()

vCollectionDeckBox::vCollectionDeckBox( wxWindow* aptParent, 
                                        wxWindowID aiWID,
                                        const wxString& aszColID )
   : wxPanel(aptParent, aiWID)
{
   m_wxszColID = aszColID;

   wxFlexGridSizer* boxSizer = new wxFlexGridSizer(0, 2, 0, 0);
   boxSizer->AddGrowableCol(0);
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
   auto viColEd = new viCollectionEditor(this, 4, m_wxszColID);
   this->GetSizer()->Add(viColEd, wxSizerFlags(1).Center().Shaped());

}

void 
vCollectionDeckBox::onEditorAccept(wxCommandEvent& awxEvt)
{
   m_vcItemList->RefreshList();
}

void 
vCollectionDeckBox::buildItemList()
{
   m_vcItemList = new vcCollectionDeckBoxItemList(this, m_wxszColID);
   this->GetSizer()->Add(m_vcItemList, 1, wxEXPAND);
}