#pragma once
#include "wx\wxprec.h"
#include "CETraitOption.h"
#include "ImageFetcher.h"
#include <memory>
#include <mutex>
#include <vector>
#include <wx/imagjpeg.h>
#include <wx/image.h>
#include <wx/bitmap.h>

class vcEditableTraitList;
class vcEditableItemList;
class vcImageWrapper;

class viCardEditor : public wxPanel
{
public:
   enum
   {
      Changes_Submit = 0x50,
      Changes_Reset = 0x51,
      Image_Changed = 0x52
   };
   class viCardEditorImageCallBack : public ImageFetcherCallback
   {
   public:
      viCardEditorImageCallBack( viCardEditor* aptCE, std::shared_ptr<std::mutex> amutex );

      virtual void CallBack() override;

   private:
      viCardEditor * m_ptCardEditor;
      std::shared_ptr<std::mutex> m_mutex;
   };

public:
   viCardEditor( wxWindow* aptParent, wxWindowID aiWID,
                 wxString aszColID, wxString aszCardHash );
   ~viCardEditor();

   void DisplayNew(wxString aszColID, wxString aszCardHash );
   wxString GetDisplayingCard();

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
   std::vector<std::shared_ptr<ImageFetcherCallback>> m_vecImageCallbacks;
   std::shared_ptr<std::mutex> m_mutex;

   void fetchImage();
   void setImage(const wxString& aszImagePath);
   void stopCallbacks( bool abBlock = true );

   bool parseNew(wxString aszColID, wxString aszCardHash );
   void refreshDisplay();
   void refreshEditor();
   void refreshSelector();

   void buildTraitListEditor();
   void buildSubmitResetButtons();

   void onChangesAccept(wxCommandEvent& awxEvt);
   void onChangesReset(wxCommandEvent& awxEvt);

   std::vector<std::string> submitChangesToServer();
};