#pragma once
#include <wx/wxprec.h>
#include <wx/grid.h>

class wxInfiniteGrid : public wxGrid
{
public:
   wxInfiniteGrid( wxWindow *parent,
                   wxWindowID id = wxID_ANY,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxWANTS_CHARS,
                   const wxString& name = wxGridNameStr );
   virtual ~wxInfiniteGrid();

protected:
   wxDECLARE_EVENT_TABLE();

   void onScroll( wxScrollWinEvent& awxEvt );
};

