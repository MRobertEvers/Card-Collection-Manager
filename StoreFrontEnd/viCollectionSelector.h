#pragma once
#include <wx/grid.h>

class vicCollectionPeeker;

class viCollectionSelector : public wxFrame
{
public:
   viCollectionSelector( wxPanel* aptParent, wxWindowID aiWID );
   ~viCollectionSelector();

private:
   wxDECLARE_EVENT_TABLE();

   vicCollectionPeeker * m_Peeker;

   void onItemSelection( wxGridEvent& awxEvt );
   void onResize( wxSizeEvent& awxEvt );
   void onSelectRange( wxGridRangeSelectEvent& awxEvt );
   void onSelectColumn( wxGridEvent& awxEvt );

};

