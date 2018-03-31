#pragma once
#include <wx/wxprec.h>

class wxImagePanel;

class vcImageWrapper : public wxPanel
{
public:
   vcImageWrapper(wxWindow* aptParent,
                  wxWindowID aiID,
                  bool abDoScale = true);
   ~vcImageWrapper();

   bool SetImage(const wxString& aszImagePath);

private:
   bool m_bDoScale;
   wxImagePanel* m_jpgPanel;

   void freeImage();
};