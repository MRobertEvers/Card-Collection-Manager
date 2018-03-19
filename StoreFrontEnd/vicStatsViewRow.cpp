#include "vicStatsViewRow.h"

vicStatsViewRow::vicStatsViewRow( wxWindow* aptParent,
                                  wxWindowID aiID )
   : wxPanel(aptParent, aiID), m_iModules(0)
{
   wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
   this->SetSizer( sizer );
}

vicStatsViewRow::~vicStatsViewRow()
{

}

bool 
vicStatsViewRow::CanAdd()
{
   return m_iModules < 3;
}

void 
vicStatsViewRow::AddModule( wxPanel* aptNewMod )
{
   this->GetSizer()->Add( aptNewMod, wxSizerFlags( 1 ) );
   m_iModules++;
}