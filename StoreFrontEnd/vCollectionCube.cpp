#include "vCollectionCube.h"
#include "vcCollectionCubeDisplay.h"
#include "vcCollectionCubeGroup.h"
#include "viCollectionEditor.h"
#include "viCardEditor.h"
#include "GroupItemData.h"
#include "MainWindow.h"
#include "viCardEditor.h"
#include "vcCollectionCubeDisplay.h"

wxBEGIN_EVENT_TABLE( vCollectionCube, wxPanel )
EVT_BUTTON( viCardEditor::Changes_Submit, vCollectionCube::onCardChanged )
EVT_BUTTON( viCollectionEditor::Changes_Accept, vCollectionCube::onEditorAccept )
EVT_LIST_ITEM_SELECTED( vcCollectionCubeDisplay::Group_List, vCollectionCube::onNewItemSelected )
EVT_BUTTON( viCollectionEditor::Changes_Decline, vCollectionCube::onEditorDecline )
wxEND_EVENT_TABLE()

vCollectionCube::vCollectionCube( MainFrame* aptParent,
                                  wxWindowID aiWID,
                                  const wxString& aszColID )
   : wxPanel( aptParent, aiWID ), m_viColEditor( 0 )
{
   m_wxszColID = aszColID;
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 1, 3, 0, 0 );

   boxSizer->AddGrowableCol( 0 );
   boxSizer->AddGrowableRow( 0 );
   // Order is Count, Name*, Mana Cost, Card Type
   this->SetSizer( boxSizer );

   this->GetParent()->SetSize( 550, 500 );
   this->GetParent()->Layout();
   buildGroupPanel();

   if( !m_vcItemList->IsEmpty() )
   {
      notifyCardEditor( m_vcItemList->GetFirst().GetHash() );
   }
}


vCollectionCube::~vCollectionCube()
{

}

void
vCollectionCube::ShowCollectionEditor()
{
   if( m_viColEditor != 0 ) { return; }

   m_viColEditor = new viCollectionEditor( this, 4, m_wxszColID );
   this->GetSizer()->Add( m_viColEditor, wxSizerFlags( 1 ).Top().Shaped() );
   auto iAdditionSize = m_viColEditor->GetSize().GetWidth();
   this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() + iAdditionSize,
                               this->GetParent()->GetSize().GetY() );
}

void
vCollectionCube::CloseCollectionEditor()
{
   auto iAdditionSize = m_viColEditor->GetSize().GetWidth();
   m_viColEditor->Destroy();
   m_viColEditor = 0;
   this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() - iAdditionSize,
                               this->GetParent()->GetSize().GetY() );
}

void
vCollectionCube::onEditorAccept( wxCommandEvent& awxEvt )
{
   m_vcItemList->RefreshList();
   CloseCollectionEditor();
}

void
vCollectionCube::onEditorDecline( wxCommandEvent& awxEvt )
{
   CloseCollectionEditor();
}

void
vCollectionCube::onCardChanged( wxCommandEvent& awxEvt )
{
   m_vcItemList->RefreshList();
   m_viCardEditor->DisplayNew( m_wxszColID, m_vcItemList->GetFirst().GetHash() );
   m_viCardEditor->Layout();
   m_viCardEditor->Thaw();
}

void
vCollectionCube::onNewItemSelected( wxListEvent& awxEvt )
{
   notifyCardEditor( awxEvt.GetString() );
   awxEvt.StopPropagation();
}

void
vCollectionCube::onDeckEditor( wxCommandEvent& awxEvt )
{
   ShowCollectionEditor();
}

void
vCollectionCube::buildGroupPanel()
{
   m_vcItemList = new vcCollectionCubeDisplay( this, 0, m_wxszColID );
   this->GetSizer()->Add( m_vcItemList, wxSizerFlags( 1 ).Expand() );
}

void
vCollectionCube::notifyCardEditor( const wxString& aszHash )
{
   this->Freeze();
   if( m_viCardEditor == nullptr )
   {
      m_viCardEditor = new viCardEditor( this, 5, m_wxszColID, aszHash );
      //m_viCardEditor->SetMinSize(wxSize(350, 500));
      this->GetSizer()->Add( m_viCardEditor, wxSizerFlags( 1 ).Expand() );
      auto iAdditionSize = m_viCardEditor->GetSize().GetWidth();
      this->GetParent()->SetSize( this->GetParent()->GetSize().GetX() + iAdditionSize,
                                  this->GetParent()->GetSize().GetY() );
   }
   else
   {
      m_viCardEditor->DisplayNew( m_wxszColID, aszHash );
   }
   PostSizeEvent();
   this->Thaw();
}