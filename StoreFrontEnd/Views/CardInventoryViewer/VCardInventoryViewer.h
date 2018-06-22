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
   class SetDisplay : public wxPanel
   {
   public:
      SetDisplay( wxWindow* aptParent, wxWindowID aiWID, CardInterface* apInterface );
      ~SetDisplay( );

   private:
      wxAuiManager m_mgr;
   };
public:
   VCardInventoryViewer( wxWindow* aptParent, wxWindowID aiWID );
   ~VCardInventoryViewer();

   void SetController( CCardInventoryViewer* apController );

   void ViewCard( CardInterface* apInterface );

private:
   CCardInventoryViewer* m_pController;
   wxAuiManager m_mgr;

   wxTextCtrl* m_pTitle;
   std::vector<SetDisplay*> m_vecItems;

   // TODO: Put this somewhere esle
public:
   static wxAuiPaneInfo GetPlainPane();
};