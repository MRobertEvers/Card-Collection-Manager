#include "vicCollectionEditListItemOption.h"



vicCollectionEditorListItemOption::
vicCollectionEditorListItemOption( wxWindow* aptParent,
                                   CELIOption aOption )
   : wxPanel(aptParent), m_oOption(aOption)
{
   wxFlexGridSizer* fgridSizer = new wxFlexGridSizer(1, 2, 0, 0);
   fgridSizer->AddGrowableCol(0);
   fgridSizer->SetFlexibleDirection(wxHORIZONTAL);
   fgridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   this->SetSizer(fgridSizer);

   buildOptionSelector();
}


vicCollectionEditorListItemOption::
~vicCollectionEditorListItemOption()
{
}

wxString 
vicCollectionEditorListItemOption::
GetSelection()
{
   return m_wxComboBox->GetStringSelection();
}

void 
vicCollectionEditorListItemOption::
buildOptionSelector()
{
   wxStaticText* wxText = new wxStaticText( this, wxID_ANY, m_oOption.Display,
                                            wxDefaultPosition, wxDefaultSize );
   this->GetSizer()->Add(wxText, wxSizerFlags(0).CenterVertical().Border(wxALL, 2));

   buildOption(&m_wxComboBox, m_oOption);
}

void 
vicCollectionEditorListItemOption::
buildOption(wxChoice** aChoice, const CELIOption& aOption)
{
   *aChoice = new wxChoice(this, wxID_ANY);
   for( auto& option : aOption.IDs )
   {
      (*aChoice)->Append(option.second);
   }
   if( aOption.IDs.size() > 0 )
   {
      (*aChoice)->SetSelection(0);
   }
   this->GetSizer()->Add(*aChoice, wxSizerFlags(1).Expand());
}