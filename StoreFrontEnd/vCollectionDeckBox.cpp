#include "vCollectionDeckBox.h"
#include "vcCollectionDeckBoxItemList.h"
#include "viCollectionEditor.h"
#include "viCardEditor.h"
#include "vcdCDBIListItemData.h"
#include "MainWindow.h"

wxBEGIN_EVENT_TABLE(vCollectionDeckBox, wxPanel)
EVT_BUTTON(viCollectionEditor::Changes_Accept, vCollectionDeckBox::onEditorAccept)
EVT_LIST_ITEM_SELECTED(vcCollectionDeckBoxItemList::List, vCollectionDeckBox::onNewItemSelected)
EVT_BUTTON(viCollectionEditor::Changes_Decline, vCollectionDeckBox::onEditorDecline)
wxEND_EVENT_TABLE()

vCollectionDeckBox::vCollectionDeckBox( MainFrame* aptParent, 
                                        wxWindowID aiWID,
                                        const wxString& aszColID )
   : wxPanel(aptParent, aiWID), m_viColEditor(0)
{
   m_wxszColID = aszColID;
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer(1, 3, 0, 0);
   boxSizer->AddGrowableCol(0);
   boxSizer->AddGrowableRow(0);
   // Order is Count, Name*, Mana Cost, Card Type
   this->SetSizer(boxSizer);

   this->GetParent()->SetSize(550, 500);
   buildItemList();
   this->GetParent()->Layout();

   if( !m_vcItemList->IsEmpty() )
   {
      notifyCardEditor(m_vcItemList->GetFirst().GetHash());
   }
   
}


vCollectionDeckBox::~vCollectionDeckBox()
{

}

void 
vCollectionDeckBox::ShowCollectionEditor()
{
   if( m_viColEditor != 0 ) { return; }

   m_viColEditor = new viCollectionEditor(this, 4, m_wxszColID);
   this->GetSizer()->Add(m_viColEditor, wxSizerFlags(1).Top().Shaped());
   auto iAdditionSize = m_viColEditor->GetSize().GetWidth();
   this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() + iAdditionSize,
                               this->GetParent()->GetSize().GetY() );
}

void 
vCollectionDeckBox::CloseCollectionEditor()
{
   auto iAdditionSize = m_viColEditor->GetSize().GetWidth();
   m_viColEditor->Destroy();
   m_viColEditor = 0;
   this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() - iAdditionSize,
                               this->GetParent()->GetSize().GetY() );
}

void 
vCollectionDeckBox::onEditorAccept(wxCommandEvent& awxEvt)
{
   m_vcItemList->RefreshList();
   CloseCollectionEditor();
}

void 
vCollectionDeckBox::onEditorDecline(wxCommandEvent& awxEvt)
{
   CloseCollectionEditor();
}

void 
vCollectionDeckBox::onNewItemSelected(wxListEvent& awxEvt)
{
   auto iIndex = awxEvt.GetIndex();
   auto listItem = m_vcItemList->GetItemByListIndex(iIndex);
   notifyCardEditor(listItem.GetHash());
   awxEvt.StopPropagation();
}

void
vCollectionDeckBox::onDeckEditor(wxCommandEvent& awxEvt)
{
   ShowCollectionEditor();
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
      //m_viCardEditor->SetMinSize(wxSize(350, 500));
      this->GetSizer()->Add(m_viCardEditor, wxSizerFlags(1).Expand());
      auto iAdditionSize = m_viCardEditor->GetSize().GetWidth();
      this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() + iAdditionSize,
                                  this->GetParent()->GetSize().GetY() );
   }
   else
   {
      m_viCardEditor->DisplayNew(m_wxszColID, aszHash);
   }
   PostSizeEvent();
   this->Thaw();
}