#pragma once
#include <wx/wxprec.h>

class vicStatsViewRow : public wxPanel
{
public:
   vicStatsViewRow( wxWindow* aptParent,
                    wxWindowID aiID );
   ~vicStatsViewRow();

   bool CanAdd();
   void AddModule( wxPanel* aptNewMod );

private:
   unsigned char m_iModules;
};