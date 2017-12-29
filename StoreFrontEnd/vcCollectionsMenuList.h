#pragma once
#include "wx/wxprec.h"
#include <wx/listctrl.h>

class vcCollectionsMenuList : public wxPanel
{
public:
   vcCollectionsMenuList(wxWindow* aptParent);
   ~vcCollectionsMenuList();

private:
   wxListCtrl* m_wxListControl;

};

