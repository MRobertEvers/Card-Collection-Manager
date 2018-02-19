#pragma once
#include "wx\wxprec.h"
#include <vector>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "CETraitOption.h"

#include <wx/imagjpeg.h>

class vcEditableTraitList;
class vcImageWrapper;

class viCardEditor : public wxPanel
{
public:
   viCardEditor( wxWindow* aptParent, wxWindowID aiWID,
                 wxString aszColID, wxString aszCardHash );
   ~viCardEditor();

   void DisplayNew(wxString aszColID, wxString aszCardHash);

private:
   //wxDECLARE_EVENT_TABLE();
   vcImageWrapper* m_jpgPanel;
   vcEditableTraitList* m_wxTraitList;
   wxString m_szCardName;
   wxString m_szColID;
   std::vector<CETraitOption> m_vecAttrs;
   std::vector<wxString> m_vecUIDs;

   void fetchImage();
   void setImage(const wxString& aszImagePath);

   void buildTraitListEditor();
};