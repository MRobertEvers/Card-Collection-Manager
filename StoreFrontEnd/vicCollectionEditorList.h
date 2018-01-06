#pragma once
#include<wx/wxprec.h>
#include <vector>
#include <wx/scrolwin.h>
#include "CELIOption.h"

class vicCollectionEditorList : public wxScrolledWindow
{
public:
   vicCollectionEditorList(wxWindow* aptParent);
   ~vicCollectionEditorList();

   // If there is a second option. It is what is getting added.
   // The first option is removed.
   void AddItem(CELIOption aszItem, CELIOption aszAdd = CELIOption());
   std::vector<wxString> GetCommandList();
   
private:
   wxDECLARE_EVENT_TABLE();
   int m_iItemCounts;
   void onCancelItem(wxCommandEvent& awxEvt);
};

