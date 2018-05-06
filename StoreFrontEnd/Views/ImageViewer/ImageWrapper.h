#pragma once
#include <wx/wxprec.h>

class wxImagePanel;

/*
* Allows an interface for easily switching images. Does not handle errors.
*/
class ImageWrapper : public wxPanel
{
public:
   ImageWrapper(wxWindow* aptParent,
                  wxWindowID aiID,
                  bool abDoScale = true);
   ~ImageWrapper();

   // Changes the displayed image.
   bool SetImage(const wxString& aszImagePath);

private:
   bool m_bDoScale;
   wxImagePanel* m_jpgPanel;

   void freeImage();
};