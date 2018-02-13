#include "MainWindow.h"
#include "StorefrontConfig.h"
#include "cCollectionsOverview.h"
#include "cCollectionDeckBox.h"
#include "vCollectionsOverview.h"
#include "StoreFrontEnd.h"
#include <wx/defs.h> 

// Events can be tied at run-time. This is compile time.
wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
EVT_BUTTON(vCollectionsOverview::View_Collection, MainFrame::OnViewCollection)
EVT_MENU(Menu_Quit, MainFrame::OnQuit)
EVT_MENU(Menu_About, MainFrame::OnAbout)
EVT_MENU(Menu_Main, MainFrame::OnViewCollectionOverview)
EVT_MENU(Menu_Import, MainFrame::OnImportSource)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title)
   : wxFrame(NULL, sfMAIN_WINDOW, title), m_bEvtHandlerView(false), m_bEvtHandlerViewFlag(false)
{
   StoreFrontEnd::Instance();
   wxInitAllImageHandlers();

   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   buildMenus();

   viewCollectionsOverview();
}

MainFrame::~MainFrame()
{

}


void 
MainFrame::RegisterMenu(wxMenu* afileMenu, wxString aszTitle)
{
   m_wxMenuBar->Append(afileMenu, aszTitle);
}

void 
MainFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
   // true is to force the frame to close
   Close(true);
}

void 
MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
   //wxMessageBox(wxString::Format
   //(
   //   "Welcome to %s!\n"
   //   "\n"
   //   "This is the minimal wxWidgets sample\n"
   //   "running under %s.",
   //   wxVERSION_STRING,
   //   wxGetOsDescription()
   //),
   //   "About wxWidgets minimal sample",
   //   wxOK | wxICON_INFORMATION,
   //   this);

}

void 
MainFrame::OnViewCollection(wxCommandEvent& awxEvt)
{
   viewCollection(awxEvt.GetString());
}

void 
MainFrame::OnViewCollectionOverview(wxCommandEvent& awxEvt)
{
   viewCollectionsOverview();
}

void 
MainFrame::OnImportSource(wxCommandEvent& awxEvt)
{
   StoreFrontEnd::Instance()->ImportCollectionSource();
}

void 
MainFrame::OnGenericMenuEvent(wxCommandEvent& awxEvt)
{
   if( m_bEvtHandlerView )
   {
      m_CurrentPanel->MenuEvent(awxEvt);
   }
}

void 
MainFrame::RegisterSendMenuEvents()
{
   m_bEvtHandlerViewFlag = true;
}

void 
MainFrame::ReleaseMenuEventHandler()
{
   if( m_bEvtHandlerView )
   {
      ((IMenuEventHandler*)m_CurrentPanel)->ReleaseEventHandler();
   }
   m_bEvtHandlerView = m_bEvtHandlerViewFlag;
   m_bEvtHandlerViewFlag = false;
}

void 
MainFrame::BindMenuEventHandler()
{
   if( m_bEvtHandlerView )
   {
      m_CurrentPanel->BindEventHandler();
   }
}

void 
MainFrame::buildMenus()
{
   // set the frame icon
   SetIcon(wxICON(sample));

   // create a menu bar
   wxMenu *fileMenu = new wxMenu;
   fileMenu->Append(Menu_Main, "&Overview", "Return to collection overview.");
   fileMenu->Append(Menu_Import, "&Import Source", "Import collection source");
   fileMenu->Append(Menu_Quit, "E&xit\tAlt-X", "Quit this program");

   // the "About" item should be in the help menu
   wxMenu *helpMenu = new wxMenu;
   helpMenu->Append(Menu_About, "&About\tF1", "Show about dialog");

   // now append the freshly created menu to the menu bar...
   m_wxMenuBar = new wxMenuBar();
   m_wxMenuBar->Append(fileMenu, "&File");
   m_wxMenuBar->Append(helpMenu, "&Help");
   
   // ... and attach this menu bar to the frame
   SetMenuBar(m_wxMenuBar);

   // create a status bar just for fun (by default with 1 pane only)
   CreateStatusBar(2);
   SetStatusText("StoreFrontPro!");
}

void 
MainFrame::viewCollectionsOverview()
{
   setView(new cCollectionsOverview(this));
}

void 
MainFrame::viewCollection(const wxString& aszColName)
{
   auto ptSF = StoreFrontEnd::Instance();
   auto szName = ptSF->GetCollectionID(aszColName.ToStdString());
   setView( new cCollectionDeckBox(this, wxString(szName)) );
}

void 
MainFrame::setView(IMenuEventHandler* awxNewPanel)
{
   if( m_CurrentPanel != NULL )
   {
      ReleaseMenuEventHandler();

      for( unsigned int i = 0; i < m_vecViewMenus.size(); i++ )
      {
         auto ptMenu = m_wxMenuBar->Remove(i+2);
         delete ptMenu;
      }
      m_vecViewMenus.clear();

      this->GetSizer()->Detach(m_CurrentPanel->GetView());
      m_CurrentPanel->GetView()->Destroy();
   }

   m_CurrentPanel = awxNewPanel;
   this->GetSizer()->Add(m_CurrentPanel->GetView(), wxSizerFlags(1).Expand());

   // Looks at current panel.
   BindMenuEventHandler();

   // Causes the children to calculate sizes.
   PostSizeEvent();
}

void
MainFrame::appendMenuOptions(unsigned int aiMenuID, const wxString& aszName)
{

}