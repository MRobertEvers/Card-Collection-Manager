#include "vcItemNameCheckBox.h"


vcItemNameCheckBox::vcItemNameCheckBox(wxWindow* aptParent,
                                       wxWindowID aiID,
                                       const wxString& aszUID)
   : wxPanel(aptParent, aiID)
{
   wxFlexGridSizer* fgridSizer = new wxFlexGridSizer(1, 2, 0 ,0);
   fgridSizer->AddGrowableCol(0);
   fgridSizer->SetFlexibleDirection(wxHORIZONTAL);
   fgridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   this->SetSizer(fgridSizer);
   
   wxStaticText* wxText = new wxStaticText( this, wxID_ANY, aszUID,
                                            wxDefaultPosition, wxDefaultSize );
   fgridSizer->Add(wxText, wxSizerFlags(1).Left());

   m_wxCheckBox = new wxCheckBox(this, 4, "", wxDefaultPosition, wxSize(25, 25));
   fgridSizer->Add(m_wxCheckBox, wxSizerFlags(1).Right());
}

vcItemNameCheckBox::~vcItemNameCheckBox()
{

}


bool 
vcItemNameCheckBox::IsChecked()
{
   return m_wxCheckBox->IsChecked();
}

void 
vcItemNameCheckBox::SetChecked(bool abTrue)
{
   if( abTrue )
   {
      m_wxCheckBox->Set3StateValue(wxCheckBoxState::wxCHK_CHECKED);
   }
   else
   {
      m_wxCheckBox->Set3StateValue(wxCheckBoxState::wxCHK_UNCHECKED);
   }
}