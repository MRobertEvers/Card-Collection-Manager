#include "vStatsViewer.h"
#include "vicStatsViewRow.h"
#include "vimCMCModule.h"

vStatsViewer::vStatsViewer( wxWindow* aptParent,
                            wxWindowID aiID,
                            std::shared_ptr<CollectionInterface> aptInterface )
   : wxFrame(aptParent, aiID, "Stats"), m_ptInterface(aptInterface)
{
   // Want 3 by n.
   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   vicStatsViewRow* row = new vicStatsViewRow( this, 1 );
   sizer->Add( row, wxSizerFlags( 1 ) );

   vimCMCModule* mod = new vimCMCModule( this, 1, m_ptInterface );
   row->AddModule( mod );
}


vStatsViewer::~vStatsViewer()
{
}
