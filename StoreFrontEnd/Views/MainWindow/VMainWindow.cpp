#include "VMainWindow.h"
#include "MainWindow.h"
#include "CMainWindow.h"
#include "../StoreFrontEnd/StorefrontConfig.h"
#include "../CollectionsOverview/vCollectionsOverview.h"
#include "../ViewTemplates/icCollectionView.h"
#include "../ViewTemplates/IMenuEventHandler.h"

wxBEGIN_EVENT_TABLE(VMainWindow, IMenuEventSource )
EVT_BUTTON(VCollectionsOverview::View_Collection, VMainWindow::OnViewCollection)
EVT_MENU(MainFrame::Menu_Quit, VMainWindow::OnQuit)
EVT_MENU( MainFrame::Menu_About, VMainWindow::OnAbout)
EVT_MENU( MainFrame::Menu_Main, VMainWindow::OnViewCollectionOverview)
EVT_MENU( MainFrame::Menu_Import, VMainWindow::OnImportSource)
EVT_MENU( MainFrame::Menu_View_As, VMainWindow::OnViewCollectionAs)
wxEND_EVENT_TABLE()

VMainWindow::VMainWindow( const wxString& title )
   : IMenuEventSource( sfMAIN_WINDOW, title )
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( boxSizer );

   uiBuildDefaultMenus();
}


VMainWindow::~VMainWindow()
{

}

void 
VMainWindow::SetController( CMainWindow* aptController )
{
   m_Controller = aptController;
}

void 
VMainWindow::OnQuit( wxCommandEvent& awxEvt )
{
   Close( true );
}

void 
VMainWindow::OnAbout( wxCommandEvent& awxEvt )
{

}

void 
VMainWindow::OnViewCollection( wxCommandEvent& awxEvt )
{
   m_Controller->ShowCollection( awxEvt.GetString() );
}

void 
VMainWindow::OnViewCollectionOverview( wxCommandEvent& awxEvt )
{
   m_Controller->ShowCollectionsOverview();
}

void 
VMainWindow::OnImportSource( wxCommandEvent& awxEvt )
{
   m_Controller->ImportSource();
}

void 
VMainWindow::OnViewCollectionAs( wxCommandEvent& awxEvt )
{
   wxString szCollection = awxEvt.GetString();
   int iType = awxEvt.GetInt();
   if( iType == MainFrame::Menu_View_As_Deck )
   {
      m_Controller->ShowCollection( szCollection, CMainWindow::Deck_View );
   }
   else if( iType == MainFrame::Menu_View_As_Cube )
   {
      m_Controller->ShowCollection( szCollection, CMainWindow::Cube_View );
   }
}

void 
VMainWindow::SetView( wxPanel* aptEVTHandler )
{
   if( m_CurrentPanel != nullptr )
   {
      this->GetSizer()->Detach( m_CurrentPanel );
      m_CurrentPanel->Destroy();
   }

   m_CurrentPanel = aptEVTHandler;
   this->GetSizer()->Add( aptEVTHandler, wxSizerFlags( 1 ).Expand() );

   // Causes the children to calculate sizes.
   Layout();
}

void 
VMainWindow::uiBuildDefaultMenus()
{
   // set the frame icon
   SetIcon( wxICON( sample ) );

   // create a menu bar
   wxMenu *fileMenu = new wxMenu;
   fileMenu->Append( MainFrame::Menu_Main, "&Overview", "Return to collection overview." );
   fileMenu->Append( MainFrame::Menu_Import, "&Import Source", "Import collection source" );
   fileMenu->Append( MainFrame::Menu_Quit, "E&xit\tAlt-X", "Quit this program" );

   // the "About" item should be in the help menu
   wxMenu *helpMenu = new wxMenu;
   helpMenu->Append( MainFrame::Menu_About, "&About\tF1", "Show about dialog" );

   // now append the freshly created menu to the menu bar...
   m_wxMenuBar = new wxMenuBar();
   m_wxMenuBar->Append( fileMenu, "&File" );
   m_wxMenuBar->Append( helpMenu, "&Help" );

   // ... and attach this menu bar to the frame
   SetMenuBar( m_wxMenuBar );

   // create a status bar just for fun (by default with 1 pane only)
   CreateStatusBar( 2 );
   SetStatusText( "StoreFrontPro!" );

   uiProtectMenus( 2 );
}

void 
VMainWindow::uiSetView()
{

}