#pragma once
#include "wx\wxprec.h"
#include <vector>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "CETraitOption.h"

#include <wx/imagjpeg.h>

class vcEditableTraitList;
class vcEditableItemList;
class vcImageWrapper;

class viCardEditor : public wxPanel
{
public:
   enum
   {
      Changes_Submit = 0x50,
      Changes_Reset = 0x51
   };
   viCardEditor( wxWindow* aptParent, wxWindowID aiWID,
                 wxString aszColID, wxString aszCardHash );
   ~viCardEditor();

   void DisplayNew(wxString aszColID, wxString aszCardHash);

private:
   wxDECLARE_EVENT_TABLE();
   vcImageWrapper* m_jpgPanel;
   vcEditableTraitList* m_wxTraitList;
   vcEditableItemList* m_wxEditableItemList;
   wxString m_szCardName;
   wxString m_szColID;
   wxString m_szDisplayingHash;
   std::vector<CETraitOption> m_vecAttrs;
   std::vector<wxString> m_vecUIDs;

   void fetchImage();
   void setImage(const wxString& aszImagePath);

   bool parseNew(wxString aszColID, wxString aszCardHash);
   void refreshDisplay();
   void refreshEditor();
   void refreshSelector();

   void buildTraitListEditor();
   void buildSubmitResetButtons();

   void onChangesAccept(wxCommandEvent& awxEvt);
   void onChangesReset(wxCommandEvent& awxEvt);
};