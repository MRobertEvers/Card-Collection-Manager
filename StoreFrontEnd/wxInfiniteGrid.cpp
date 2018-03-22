#include "wxInfiniteGrid.h"

wxBEGIN_EVENT_TABLE( wxInfiniteGrid, wxGrid )
EVT_SCROLLWIN( wxInfiniteGrid::onScroll )
wxEND_EVENT_TABLE()

wxInfiniteGrid::wxInfiniteGrid( wxWindow *parent,
                                wxWindowID id,
                                const wxPoint& pos,
                                const wxSize& size,
                                long style,
                                const wxString& name )
   : wxGrid(parent, id, pos, size, style, name)
{

}


wxInfiniteGrid::~wxInfiniteGrid()
{
}


void
wxInfiniteGrid::onScroll( wxScrollWinEvent& awxEvt )
{
   awxEvt.ResumePropagation( 999 );
   awxEvt.Skip();
}
