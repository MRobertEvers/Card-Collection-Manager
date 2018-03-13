#pragma once
#include "wx/wxprec.h"

class wxImagePanel;

class vcImageWrapper : public wxPanel
{
public:
   vcImageWrapper(wxWindow* aptParent,
                  wxWindowID aiID);
   ~vcImageWrapper();

   bool SetImage(const wxString& aszImagePath);

private:
   wxImagePanel* m_jpgPanel;

   void freeImage();
};