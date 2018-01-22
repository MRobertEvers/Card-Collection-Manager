#pragma once
#include "wx\wxprec.h"
#include <vector>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "CETraitOption.h"
#include "wxImagePanel.h"
#include <wx/imagjpeg.h>

class viCardEditor : public wxPanel
{
public:
   viCardEditor( wxWindow* aptParent, wxWindowID aiWID,
                 wxString aszColID, wxString aszCardHash );
   ~viCardEditor();

private:
   //wxDECLARE_EVENT_TABLE();
   wxImagePanel* m_jpgPanel;
   wxString m_szCardName;
   wxString m_szColID;
   std::vector<CETraitOption> m_vecAttrs;
   std::vector<wxString> m_vecUIDs;

   void fetchImage();
   void downloadImage(const wxString& aszMUD, const wxString& aszFileName);
};