#pragma once
#include "wx/wxprec.h"

class vcEditableTraitListItem : public wxPanel
{
public:
   vcEditableTraitListItem( wxWindow* aptParent,
                            wxWindowID aiID,
                            wxString aszTraitName );
   ~vcEditableTraitListItem();

private:
   wxChoice *m_wxComboBox;
};