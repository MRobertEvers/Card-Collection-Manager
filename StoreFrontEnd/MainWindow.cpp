#include "MainWindow.h"
#include "VMainWindow.h"
#include "CMainWindow.h"


MainFrame::MainFrame(const wxString& title)
{
   m_View = new VMainWindow( title );
   m_Controller = new CMainWindow( m_View );
   m_View->SetController( m_Controller );

   m_View->Show();
}

MainFrame::~MainFrame()
{

}
