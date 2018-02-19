#include "vcEditableTraitListItem.h"

vcEditableTraitListItem::vcEditableTraitListItem( wxWindow* aptParent,
                                                  wxWindowID aiID,
                                                  wxString aszTraitName )
   : wxPanel(aptParent, aiID)
{
   wxFlexGridSizer* fgridSizer = new wxFlexGridSizer(1, 2, 0, 0);
   fgridSizer->AddGrowableCol(0);
   fgridSizer->SetFlexibleDirection(wxHORIZONTAL);
   fgridSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_NONE);
   this->SetSizer(fgridSizer);

   wxStaticText* wxText = new wxStaticText( this, wxID_ANY, aszTraitName,
                                            wxDefaultPosition, wxDefaultSize );
   fgridSizer->Add(wxText);
   m_wxComboBox = new wxChoice(this, 1);
   fgridSizer->Add(m_wxComboBox);
}

vcEditableTraitListItem::~vcEditableTraitListItem()
{

}