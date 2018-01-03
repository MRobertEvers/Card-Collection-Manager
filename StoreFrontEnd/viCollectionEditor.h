#pragma once
#include "wx/wxprec.h"

class vicListSelector;
class vicCollectionEditorList;

class viCollectionEditor : public wxPanel
{
public:
   enum
   {
      Changes_Accept = 0x132,
      Changes_Decline = 0x99
   };

   enum Selectors : int
   {
      Add = 0x0,
      Remove = 0x1
   };

   viCollectionEditor(wxWindow* aptParent, wxWindowID aiWID, wxString aszColID);
   ~viCollectionEditor();

private:
   wxDECLARE_EVENT_TABLE();

   wxString m_szCollectionID;
   vicListSelector* m_vAddSelector;
   vicListSelector* m_vRemSelector;
   vicCollectionEditorList* m_vListView;
   bool m_bHandleTextEvent;

   void buildSelectors();
   void buildListView();
   void buildButtons();

   void onComboBoxTextChanged(wxCommandEvent& awxEvt);
   void onComboBoxAccept(wxCommandEvent& awxEvt);

   const char* ADD_BUTTON_TEXT = "Add";
   const char* REMOVE_BUTTON_TEXT = "Remove/Replace";
};

