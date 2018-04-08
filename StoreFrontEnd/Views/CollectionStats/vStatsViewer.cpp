#include "vStatsViewer.h"
#include "vicStatsViewRow.h"
#include "./Modules/vimCMCModule.h"
#include "./Modules/vimTypeBreakDown.h"


vStatsViewer::vStatsViewer( wxWindow* aptParent,
                            wxWindowID aiID,
                            std::shared_ptr<CollectionInterface> aptInterface )
   : wxFrame(aptParent, aiID, "Stats", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP ), m_ptInterface(aptInterface)
{
   // Want 3 by n.
   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   vicStatsViewRow* row = new vicStatsViewRow( this, 1 );
   sizer->Add( row, wxSizerFlags( 1 ).Expand() );

   vimCMCModule* mod = new vimCMCModule( this, 1, m_ptInterface );
   row->AddModule( mod );

   vimTypeBreakDown* mod2 = new vimTypeBreakDown( this, 1, m_ptInterface );
   row->AddModule( mod2 );

   this->SetSize( wxSize( 720, 360 ) );
}


vStatsViewer::~vStatsViewer()
{
}