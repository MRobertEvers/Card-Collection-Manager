#include "vicCollectionPeeker.h"



vicCollectionPeeker::vicCollectionPeeker( wxWindow* aptParent )
   : wxPanel(aptParent, 2)
{
}


vicCollectionPeeker::~vicCollectionPeeker()
{
}

void 
vicCollectionPeeker::SetPeek( const wxString& aszColName,
                              const wxString& aszIconName,
                              const wxString& aszSet,
                              const wxString& aszMud,
                              const std::map<std::string, std::string> amapPeekVals )
{

}

void 
vicCollectionPeeker::SetPeek( const wxString& aszColName,
                              const std::map<std::string, std::string> amapPeekVals )
{

}
