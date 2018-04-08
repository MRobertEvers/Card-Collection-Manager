#include "vStatsViewer.h"
#include "vicStatsViewRow.h"
#include "./Modules/vimCMCModule.h"
#include "./Modules/vimTypeBreakDown.h"

BEGIN_EVENT_TABLE( vStatsViewer, wxFrame )
EVT_MOUSE_EVENTS( vStatsViewer::OnMouseEvents )
EVT_MOUSE_CAPTURE_LOST( vStatsViewer::LoseFocus ) 
END_EVENT_TABLE()

vStatsViewer::vStatsViewer( wxWindow* aptParent,
                            wxWindowID aiID,
                            std::shared_ptr<CollectionInterface> aptInterface )
   : wxFrame(NULL, aiID, "Stats", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP ), m_ptInterface(aptInterface)
{
   m_mgr.SetManagedWindow( this );
   // Want 3 by n.
   //wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   //this->SetSizer( sizer );

   //vicStatsViewRow* row = new vicStatsViewRow( this, 1 );
   //sizer->Add( row, wxSizerFlags( 1 ).Expand() );

   vimCMCModule* mod = new vimCMCModule( this, 1, m_ptInterface );
   //row->AddModule( mod );
   //mod->SetFocus();
   m_mgr.AddPane( mod,
                  wxAuiPaneInfo().Center().BestSize( 400, 400 ).CloseButton( false ).Caption( wxT( "chart" ) ) );

   //vimTypeBreakDown* mod2 = new vimTypeBreakDown( this, 1, m_ptInterface );
   //row->AddModule( mod2 );

   //this->SetSize( wxSize( 720, 360 ) );
   m_mgr.Update();
}


vStatsViewer::~vStatsViewer()
{
   m_mgr.UnInit();
}

void 
vStatsViewer::OnMouseEvents( wxMouseEvent &ev )
{

}

void 
vStatsViewer::LoseFocus( wxMouseCaptureLostEvent& awxevt )
{

}