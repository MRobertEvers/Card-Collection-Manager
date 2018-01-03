#pragma once
#include "wx/wxprec.h"
#include <wx/combobox.h>
#include <string>
#include <vector>

class vicListSelector : public wxPanel
{
public:
   enum
   {
      AcceptButton = 0x0,
      ComboBox_Text = 0xAF
   };

   vicListSelector(wxWindow* aptParent, wxString aszButtonText, wxWindowID aiID = wxID_ANY);
   ~vicListSelector();

   void SetOptions(const std::vector<std::string>& avecOptions);
   void SetText(wxString aszText);
   void SetAutoComplete(wxString aszText);

   wxString GetText();
private:
   wxDECLARE_EVENT_TABLE();

   wxString m_szBoxText;
   wxString m_szButtonText;

   wxComboBox* m_wxComboBox;

   void onComboBoxTextChanged(wxCommandEvent& awxEvt);
   void onAcceptButton(wxCommandEvent& awxEvt);
};

