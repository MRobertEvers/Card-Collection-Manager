#pragma once
#include <wx/grid.h>
#include <wx/wxprec.h>
#include <string>
#include <map>

class CardRenderer;

class vicCollectionPeeker : public wxPanel
{
public:
   vicCollectionPeeker( wxWindow* aptParent );
   ~vicCollectionPeeker();

   void SetPeek( const wxString& aszColName,
                 const wxString& aszIconName,
                 const std::vector<std::pair<std::string, std::string>> amapPeekVals );

   void SetPeek( const wxString& aszColName, 
                 const std::vector<std::pair<std::string, std::string>> amapPeekVals );



private:
   wxGrid* m_Grid;
   CardRenderer* m_Viewer;
   void setPeek( const wxString& aszColName, 
                 const std::vector<std::pair<std::string, std::string>> amapPeekVals);

   void addOption( const wxString& buf );
};

