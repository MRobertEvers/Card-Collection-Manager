#pragma once
#include "wx/wxprec.h"
#include <wx/listctrl.h>
#include <wx/object.h> 

class vcCollectionDeckBoxItemList;
class viCardEditor;
class viCollectionEditor;
class MainFrame;

class vCollectionDeckBox : public wxPanel
{
public:
   vCollectionDeckBox(MainFrame* aptParent, wxWindowID aiWID, const wxString& aszColID);
   ~vCollectionDeckBox();

   void ShowCollectionEditor();
   void CloseCollectionEditor();

private:
   wxDECLARE_EVENT_TABLE();

   vcCollectionDeckBoxItemList* m_vcItemList;
   viCardEditor* m_viCardEditor;
   viCollectionEditor* m_viColEditor;
   wxString m_wxszColID;

   void onEditorAccept(wxCommandEvent& awxEvt);
   void onEditorDecline(wxCommandEvent& awxEvt);
   void onCardChanged( wxCommandEvent& awxEvt );
   void onNewItemSelected(wxListEvent& awxEvt);
   void onDeckEditor(wxCommandEvent& awxEvt);
   void buildItemList();
   void notifyCardEditor(const wxString& aszHash);
};

