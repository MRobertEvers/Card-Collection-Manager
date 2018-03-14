#pragma once
#include <wx/wxprec.h>
#include <wx/grid.h>
#include <map>
#include <string>

class vicCollectionPeeker;

class viCollectionSelector : public wxFrame
{
public:
   enum
   {
      Load_Button = 31
   };

   viCollectionSelector( wxPanel* aptParent, wxWindowID aiWID );
   ~viCollectionSelector();

private:
   wxDECLARE_EVENT_TABLE();

   vicCollectionPeeker * m_Peeker;
   wxGrid* m_Grid;

   std::map<wxString, std::vector<std::pair<std::string, std::string>>> m_mapPeekedCollections;
   std::map<wxString, wxString> m_mapFiles;
   wxString m_szPeeking;

   void setPeeking( const wxString& aszPeek );

   void onItemSelection( wxGridEvent& awxEvt );
   void onResize( wxSizeEvent& awxEvt );
   void onSelectRange( wxGridRangeSelectEvent& awxEvt );
   void onSelectColumn( wxGridEvent& awxEvt );
   void onLoadButton( wxCommandEvent& awxEvt );

   void peekCollections();
   void addOption( const wxString& buf );
};

