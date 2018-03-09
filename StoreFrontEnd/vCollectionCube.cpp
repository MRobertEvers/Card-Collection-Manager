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
EVT_BUTTON( viCardEditor::Image_Changed, vCollectionCube::onNewItemSelectView )
EVT_GRID_CELL_LEFT_CLICK( vCollectionCube::onNewItemSelected )
EVT_BUTTON( viCollectionEditor::Changes_Accept, vCollectionCube::onEditorAccept )
EVT_BUTTON( viCollectionEditor::Changes_Decline, vCollectionCube::onEditorDecline )
wxEND_EVENT_TABLE()

vCollectionCube::vCollectionCube( MainFrame* aptParent,
                                  wxWindowID aiWID,
                                  const wxString& aszColID )
   : ivCollectionView( aptParent, aiWID, aszColID )
{
   wxFlexGridSizer* boxSizer = new wxFlexGridSizer( 1, 3, 0, 0 );

   boxSizer->AddGrowableCol( 0 );
   boxSizer->AddGrowableRow( 0 );
   // Order is Count, Name*, Mana Cost, Card Type
   this->SetSizer( boxSizer );

   this->GetParent()->SetMinSize( wxSize( 550, 500 ));
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
vCollectionCube::onNewItemSelected( wxGridEvent& awxEvt )
{
   notifyCardEditor( awxEvt.GetString() );
   awxEvt.Skip();
}

void
vCollectionCube::onEditorAccept( wxCommandEvent& awxEvt )
{
   m_vcItemList->RefreshList();
   this->ivCollectionView::onEditorAccept( awxEvt );
}

void
vCollectionCube::onCardChanged( wxCommandEvent& awxEvt )
{
   m_vcItemList->RefreshList();
   this->ivCollectionView::onCardChanged( awxEvt );
}

void
vCollectionCube::buildGroupPanel()
{
   m_vcItemList = new vcCollectionCubeDisplay( this, 0, m_wxszColID );
   this->GetSizer()->Add( m_vcItemList, wxSizerFlags( 1 ).Expand() );
}

