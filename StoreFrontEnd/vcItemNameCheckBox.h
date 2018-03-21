#pragma once
#include <wx/wxprec.h>

class vcItemNameCheckBox : public wxPanel
{
public:
   vcItemNameCheckBox(wxWindow* aptParent,
                      wxWindowID aiID,
                      const wxString& aszUID);
   ~vcItemNameCheckBox();

   bool IsChecked();
   void SetChecked(bool abTrue);

private:
   wxCheckBox * m_wxCheckBox;
};
