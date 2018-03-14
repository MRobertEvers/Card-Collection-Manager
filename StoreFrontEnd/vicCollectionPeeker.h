#pragma once
#include "wx/wxprec.h""
#include <string>
#include <map>

class vicCollectionPeeker : public wxPanel
{
public:
   vicCollectionPeeker( wxWindow* aptParent );
   ~vicCollectionPeeker();

   void SetPeek( const wxString& aszColName,
                 const wxString& aszIconName,
                 const wxString& aszSet,
                 const wxString& aszMud,
                 const std::map<std::string,std::string> amapPeekVals );

   void SetPeek( const wxString& aszColName, 
                 const std::map<std::string, std::string> amapPeekVals );

private:



};

