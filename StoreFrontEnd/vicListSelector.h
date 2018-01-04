#pragma once
#include "wx/wxprec.h"
#include <wx/combobox.h>
#include <string>
#include <vector>
#include "StoreFront.h"

class vicListSelector : public wxPanel
{
public:
   class Option
   {
   public:
      std::string Display;

      // It is expected that the metatags of the id are UIDs.
      std::vector<std::pair<std::string, std::string>> UIDs;
   };

   enum
   {
      AcceptButton = 0x0,
      ComboBox_Text = 0xAF
   };

   vicListSelector(wxWindow* aptParent, wxString aszButtonText, wxWindowID aiID = wxID_ANY);
   ~vicListSelector();

   void ResetOption();
   void SetOptions(const std::vector<Option>& avecOptions);
   void SetText(wxString aszText);
   void SetAutoComplete(const std::vector<Option>& avecOptions);
   void ShowDropdown();
   void DismissDropDown();
   bool IsFocussed();

   wxString GetText();
private:
   wxDECLARE_EVENT_TABLE();

   wxString m_szBoxText;
   wxString m_szButtonText;

   wxComboBox* m_wxComboBox;
   vector<Option> m_vecOptions;

   void onComboBoxTextChanged(wxCommandEvent& awxEvt);
   void onAcceptButton(wxCommandEvent& awxEvt);
};

