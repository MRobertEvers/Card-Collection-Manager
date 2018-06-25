#pragma once
#include <wx/wxprec.h>
#include <wx/aui/framemanager.h>
#include <vector>
#include <map>

class CardInterface;
class wxRichTextCtrl;
class CCardInventoryViewer;

class VCardInventoryViewer : public wxPanel
{
public:
   enum Buttons
   {
      INV_VIEWER_SAVE_COPY = 0,
      INV_VIEWER_PREVIEWED_COPY = 1,
      INV_VIEWER_RESET_COPY = 2,
      INV_VIEWER_OPEN_EDITOR = 3
   };
   
   class SetDisplay : public wxPanel
   {
   public:
      SetDisplay( wxWindow* aptParent, wxWindowID aiWID, CardInterface* apInterface );
      ~SetDisplay( );

      wxString GetCurrentValue();
   private:
      wxDECLARE_EVENT_TABLE();

      void onPreview( wxCommandEvent& awxEvt ); // Combobox event
      std::string m_szDefault;
      wxString m_szHashRepresenting;
      std::vector<std::string> m_vecOpts;
      wxComboBox* m_pCombo;

      wxAuiManager m_mgr;

      void onSave( wxCommandEvent& awxEvt );
      void onReset( wxCommandEvent& awxEvt );
      void onOpenEditor( wxCommandEvent& awxEvt ); //
   };
public:
   VCardInventoryViewer( wxWindow* aptParent, wxWindowID aiWID );
   ~VCardInventoryViewer();

   void SetController( CCardInventoryViewer* apController );

   void ViewCard( CardInterface* apInterface );

private:
   wxDECLARE_EVENT_TABLE();

   CCardInventoryViewer* m_pController;
   wxAuiManager m_mgr;

   wxTextCtrl* m_pTitle;
   wxScrolledWindow* m_pOptions;
   wxAuiManager* m_scrollmgr;
   std::vector<SetDisplay*> m_vecItems;
   std::map<std::string, CardInterface> m_mapShownInterfaces;

   void onSave( wxCommandEvent& awxEvt );
   void onOpenEditor( wxCommandEvent& awxEvt ); //

   // TODO: Put this somewhere esle
public:
   static wxAuiPaneInfo GetPlainPane();
};