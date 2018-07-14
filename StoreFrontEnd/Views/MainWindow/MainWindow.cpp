#include "MainWindow.h"
#include "VMainWindow.h"
#include "CMainWindow.h"


MainFrame::MainFrame(const wxString& title)
{
   m_pView = new VMainWindow( title );
   m_pController = new CMainWindow( m_pView );
   m_pView->SetController( m_pController );

   m_pView->Show();
}

MainFrame::~MainFrame()
{

}
