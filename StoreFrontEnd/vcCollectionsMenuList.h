#pragma once
#include "wx/wxprec.h"
#include <wx/listctrl.h>
#include <string>

class vcCollectionsMenuList : public wxPanel
{
public:
   vcCollectionsMenuList(wxWindow* aptParent);
   ~vcCollectionsMenuList();

   void AddCollectionOption(std::string aszCollectionName);

private:
   wxListCtrl* m_wxListControl;

};

