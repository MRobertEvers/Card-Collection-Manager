#include "vicListSelector.h"
#include <wx/arrstr.h>
#include <algorithm>

using namespace std;

wxBEGIN_EVENT_TABLE(vicListSelector, wxPanel)
EVT_TEXT(ComboBox, vicListSelector::onComboBoxTextChanged)
EVT_BUTTON(AcceptButton, vicListSelector::onAcceptButton)
EVT_COMBOBOX(ComboBox, vicListSelector::onSelectionMade)
wxEND_EVENT_TABLE()

vicListSelector::vicListSelector(wxWindow* aptParent, wxString aszButtonText, wxWindowID aiID)
   : wxPanel(aptParent, aiID)
{
   m_wxComboBox = new wxComboBox( this, ComboBox, wxEmptyString, wxDefaultPosition,
                                  wxDefaultSize, 0, NULL, wxCB_DROPDOWN);

   wxButton* addButt = new wxButton( this, vicListSelector::AcceptButton,
                                     aszButtonText );

   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);

   boxSizer->Add(m_wxComboBox, 1, wxEXPAND);
   boxSizer->Add(addButt, 1, wxEXPAND);
}


vicListSelector::~vicListSelector()
{

}

void 
vicListSelector::ResetOption()
{
   m_wxComboBox->Clear();
}

void
vicListSelector::SetOptions(const vector<Option>& avecOptions)
{
   wxArrayString wxArStr;
   m_vecOptions = avecOptions;
   for( auto option : avecOptions )
   {
      wxArStr.Add(option.Display);
   }
   m_wxComboBox->Append(wxArStr);
}

void 
vicListSelector::SetText(wxString aszText)
{
   m_wxComboBox->SetValue(aszText);
   m_wxComboBox->SetInsertionPointEnd();
}

// TODO: This Does not work.
void 
vicListSelector::SetAutoComplete(const std::vector<Option>& avecOptions)
{
   wxArrayString wxArStr;
   m_vecOptions = avecOptions;
   for( auto option : avecOptions )
   {
      wxArStr.Add(option.Display);
   }
   m_wxComboBox->AutoComplete(wxArStr);
}

void 
vicListSelector::ShowDropdown()
{
   auto szText = m_wxComboBox->GetValue();
   m_wxComboBox->Freeze();
   m_wxComboBox->Popup();
   m_wxComboBox->SetValue(szText);
   m_wxComboBox->SetInsertionPointEnd();
   m_wxComboBox->Thaw();
}

void 
vicListSelector::DismissDropDown()
{
   m_wxComboBox->Dismiss();
}

bool 
vicListSelector::IsFocussed()
{
   return m_wxComboBox->HasFocus();
}

wxString
vicListSelector::GetText()
{
   return m_wxComboBox->GetValue();
}

void
vicListSelector::onComboBoxTextChanged(wxCommandEvent& awxEvt)
{
   // For some reason, when an option in the combobox is selected via mouse,
   // the event fires twice. Once with the option, and then once with blank text...
   auto szSend = m_wxComboBox->GetValue();
   if( szSend.size() == 0 )
   {
      awxEvt.StopPropagation();
      return;
   }

   awxEvt.SetString(szSend);
   awxEvt.SetInt(GetId());
   awxEvt.Skip();
}

void 
vicListSelector::onAcceptButton(wxCommandEvent& awxEvt)
{
   awxEvt.SetInt(GetId());
   awxEvt.Skip();
}

void 
vicListSelector::onSelectionMade(wxCommandEvent& awxEvt)
{
   awxEvt.Skip();
}