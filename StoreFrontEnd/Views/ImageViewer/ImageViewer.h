#pragma once
#include "ImageWrapper.h"
#include "../StoreFrontEnd/ImageFetcher.h"
#include <wx/wxprec.h>
#include <vector>
#include <memory>
#include <string>
#include <mutex>

/*
* Handles errors that occur when displaying an image. Prevents crashing.
*/
class ImageViewer : public wxPanel
{
public:
   ImageViewer( wxWindow* aptParent, wxWindowID aiWID, bool abDoScale = true );
   ~ImageViewer();

   // Returns true if the image was successfully displayed.
   bool DisplayImage( const wxString& aszFilePath );

private:
   wxDECLARE_EVENT_TABLE();

   ImageWrapper* m_ptImageWrapper;
   bool m_bDoScale;
};

