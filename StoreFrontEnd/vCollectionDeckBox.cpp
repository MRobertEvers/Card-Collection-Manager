#include "vCollectionDeckBox.h"
#include "vcCollectionDeckBoxItemList.h"
#include "viCollectionEditor.h"
#include "viCardEditor.h"
#include "GroupItemData.h"
#include "MainWindow.h"
#include "viCardEditor.h"

wxBEGIN_EVENT_TABLE(vCollectionDeckBox, wxPanel)
EVT_BUTTON( viCardEditor::Changes_Submit, vCollectionDeckBox::onCardChanged )
EVT_BUTTON(viCollectionEditor::Changes_Accept, vCollectionDeckBox::onEditorAccept)
EVT_BUTTON( viCardEditor::Image_Changed, vCollectionDeckBox::onNewItemSelectView )
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
vCollectionDeckBox::ShowCollectionEditor()
{
   if( m_viColEditor != 0 ) { return; }

   m_viColEditor = new viCollectionEditor(this, 4, m_wxszColID);
   m_viColEditor->Show();

   //this->GetSizer()->Add(m_viColEditor, wxSizerFlags(1).Top().Shaped());
   //auto iAdditionSize = m_viColEditor->GetSize().GetWidth();
   //this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() + iAdditionSize,
   //                            this->GetParent()->GetSize().GetY() );
}

void 
vCollectionDeckBox::CloseCollectionEditor()
{
   auto iAdditionSize = m_viColEditor->GetSize().GetWidth();
   m_viColEditor->Destroy();
   m_viColEditor = 0;
   //this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() - iAdditionSize,
   //                            this->GetParent()->GetSize().GetY() );
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
vCollectionDeckBox::onCardChanged( wxCommandEvent& awxEvt )
{
   m_vcItemList->RefreshList();

   m_viCardEditor->Freeze();

   auto ptSF = StoreFrontEnd::Instance();
   m_viCardEditor->DisplayNew( m_wxszColID,
                               ptSF->GetMetaTagHash( m_viCardEditor->GetDisplayingCard().ToStdString() ,
                                                     awxEvt.GetString().ToStdString() ) );
   m_viCardEditor->Layout();

   m_viCardEditor->Thaw();
}

void 
vCollectionDeckBox::onNewItemSelectView( wxCommandEvent& awxEvt )
{
   notifyCardEditor( awxEvt.GetString() );
   awxEvt.StopPropagation();
}

void 
vCollectionDeckBox::onNewItemSelected(wxListEvent& awxEvt)
{
   notifyCardEditor( awxEvt.GetString() );
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
      //auto iAdditionSize = m_viCardEditor->GetSize().GetWidth();
      //this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() + iAdditionSize,
      //                            this->GetParent()->GetSize().GetY() );
   }
   else
   {
      m_viCardEditor->DisplayNew(m_wxszColID, aszHash);
   }
   PostSizeEvent();
   this->Thaw();
}