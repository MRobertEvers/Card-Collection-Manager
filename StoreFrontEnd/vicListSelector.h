#pragma once
#include "wx/wxprec.h"
#include <wx/combobox.h>
#include "StoreFrontEnd.h"
#include "CELIOption.h"

class vicListSelector : public wxPanel
{
public:
   enum
   {
      AcceptButton = 0x0,
      ComboBox = 0xAF
   };

   vicListSelector(wxWindow* aptParent, wxString aszButtonText, wxWindowID aiID = wxID_ANY);
   ~vicListSelector();

   void ResetOption();
   void SetOptions(const std::vector<CELIOption>& avecOptions);
   void SetText(wxString aszText);
   void SetAutoComplete(const std::vector<CELIOption>& avecOptions);
   void ShowDropdown();
   void DismissDropDown();
   bool IsFocussed();
   CELIOption GetBestSelection();
   CELIOption GetSelection();

   wxString GetText();
private:
   wxDECLARE_EVENT_TABLE();

   wxComboBox* m_wxComboBox;
   vector<CELIOption> m_vecOptions;
   CELIOption m_oSelection;

   void onComboBoxTextChanged(wxCommandEvent& awxEvt);
   void onAcceptButton(wxCommandEvent& awxEvt);
   void onSelectionMade(wxCommandEvent& awxEvt);
};

