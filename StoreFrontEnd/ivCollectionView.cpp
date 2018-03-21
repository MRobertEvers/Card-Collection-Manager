#include "ivCollectionView.h"
#include "viCollectionEditor.h"
#include "viCardEditor.h"
#include "GroupItemData.h"
#include "MainWindow.h"
#include "viCardEditor.h"
#include "CollectionInterface.h"
#include "vStatsViewer.h"
#include "viHistoryViewer.h"


ivCollectionView::ivCollectionView( MainFrame* aptParent, wxWindowID aiWID, const wxString& aszColID )
   : wxPanel( aptParent, aiWID ), m_wxszColID(aszColID), m_viColEditor(0), m_infoBar(0)
{
   m_ptCollectionInterface = std::make_shared<CollectionInterface>( m_wxszColID.ToStdString() );
}


ivCollectionView::~ivCollectionView()
{
}

void
ivCollectionView::ShowCollectionEditor()
{
   if( m_viColEditor != 0 ) { return; }

   m_viColEditor = new viCollectionEditor( this, 4, m_wxszColID );
   m_viColEditor->Show();
}

void
ivCollectionView::CloseCollectionEditor()
{
   m_viColEditor->Destroy();
   m_viColEditor = 0;
}

void 
ivCollectionView::ShowStats()
{
   auto view = new vStatsViewer( this, 1, m_ptCollectionInterface );
   view->Show();
}

void 
ivCollectionView::ShowHistory()
{
   auto view = new viHistoryViewer( this, 1, m_ptCollectionInterface );
   view->Show();
}

void 
ivCollectionView::displayInfoBar()
{
   if( m_infoBar == 0 )
   {
      auto sizer = this->GetSizer();
      m_infoBar = new wxStatusBar( this );
      sizer->Add( m_infoBar, wxSizerFlags().Expand() );
      refreshInfoBar();
   }
}


void 
ivCollectionView::refreshInfoBar()
{
   wxString szCount = "Size: " + std::to_string( m_ptCollectionInterface->GetItemCount() );
   m_infoBar->SetStatusText( szCount );
}

void
ivCollectionView::onEditorAccept( wxCommandEvent& awxEvt )
{
   CloseCollectionEditor();
}

void
ivCollectionView::onEditorDecline( wxCommandEvent& awxEvt )
{
   CloseCollectionEditor();
}

void
ivCollectionView::onCardChanged( wxCommandEvent& awxEvt )
{
   auto ptSF = StoreFrontEnd::Server();
   wxString szHash = ptSF->GetMetaTagHash( m_viCardEditor->GetDisplayingCard().ToStdString(),
                                           awxEvt.GetString().ToStdString() );
   notifyCardEditor( szHash );
}

void
ivCollectionView::onNewItemSelectView( wxCommandEvent& awxEvt )
{
   notifyCardEditor( awxEvt.GetString() );
   awxEvt.StopPropagation();
}


void
ivCollectionView::onDeckEditor( wxCommandEvent& awxEvt )
{
   ShowCollectionEditor();
}

void
ivCollectionView::notifyCardEditor( const wxString& aszHash )
{
   if( m_viCardEditor == nullptr )
   {
      m_viCardEditor = new viCardEditor( this, 5, m_wxszColID, aszHash );
      this->GetSizer()->Add( m_viCardEditor, wxSizerFlags( 1 ).Expand() );
   }
   else
   {
      m_viCardEditor->DisplayNew( m_wxszColID, aszHash );
   }
}