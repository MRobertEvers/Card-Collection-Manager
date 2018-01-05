#pragma once
#include<wx/wxprec.h>
#include<vector>
#include <wx/scrolwin.h>

class vicCollectionEditorList : public wxScrolledWindow
{
public:
   vicCollectionEditorList(wxWindow* aptParent);
   ~vicCollectionEditorList();

   void AddItem(const wxString& aszItem);
   std::vector<wxString> GetCommandList();
   
private:
   wxDECLARE_EVENT_TABLE();
   int m_iItemCounts;
   void onCancelItem(wxCommandEvent& awxEvt);
};

