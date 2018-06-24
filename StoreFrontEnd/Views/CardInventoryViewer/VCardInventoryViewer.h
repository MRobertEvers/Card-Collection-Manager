#pragma once
#include <wx/wxprec.h>
#include <wx/aui/framemanager.h>
#include <vector>

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
   private:
      wxDECLARE_EVENT_TABLE();

      void onPreview( wxCommandEvent& awxEvt ); // Combobox event
      std::string m_szDefault;
      std::vector<std::string> m_vecOpts;
      wxComboBox* m_pCombo;

      wxAuiManager m_mgr;
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
   //INV_VIEWER_SAVE_COPY = 0,
   //   INV_VIEWER_PREVIEWED_COPY = 1,
   //   INV_VIEWER_RESET_COPY = 2,
   //   INV_VIEWER_SAVE_COPY = 3,
   //   INV_VIEWER_OPEN_EDITOR = 4
   void onSave( wxCommandEvent& awxEvt );
   void onReset( wxCommandEvent& awxEvt );
   void onOpenEditor( wxCommandEvent& awxEvt ); //
   void onResize( wxSizeEvent& awxEvt );

   // TODO: Put this somewhere esle
public:
   static wxAuiPaneInfo GetPlainPane();
};