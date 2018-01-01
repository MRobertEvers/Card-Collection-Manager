#pragma once
#include "wx/wxprec.h"
class vcCollectionDeckBoxItemList;

class vCollectionDeckBox : public wxPanel
{
public:
   vCollectionDeckBox(wxWindow* aptParent, wxWindowID aiWID, const wxString& aszColID);
   ~vCollectionDeckBox();

private:
   vcCollectionDeckBoxItemList* m_vcItemList;
   wxString m_wxszColID;

   void buildItemList();
};

