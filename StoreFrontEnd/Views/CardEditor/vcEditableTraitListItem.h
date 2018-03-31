#pragma once
#include "vcEditableTraitList.h"
#include <vector>
#include <string>
#include <wx/wxprec.h>

class vcEditableTraitListItem : public wxPanel
{
public:
   vcEditableTraitListItem( wxWindow* aptParent,
                            wxWindowID aiID,
                            const wxString& aszTraitName,
                            std::vector<std::string> avecOptions );
   ~vcEditableTraitListItem();

   wxString GetTraitName();
   wxString GetSelection();
   int GetSelectionInt();
   bool TrySetSelection(const wxString& aszSelect);
   bool TrySetSelection(const int aiSelect);

private:
   wxDECLARE_EVENT_TABLE();
   wxChoice *m_wxDropBox;
   wxString m_szName;
   std::vector<std::string> m_vecOptions;

   void onSelectionChanged(wxCommandEvent& awxEvt);
};