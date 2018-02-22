#pragma once
#include "wx/wxprec.h"
#include <string>

class wxListEvent;
class wxListView;
class wxButton;

class vcCollectionsMenuList : public wxPanel
{
public:
   enum
   {
      List = 0x0
   };

   vcCollectionsMenuList(wxWindow* aptParent);
   ~vcCollectionsMenuList();

   void AddCollectionOption(std::string aszCollectionName);
   void OnViewCollection( wxCommandEvent& awxEvt );
   void OnAddCollection( wxCommandEvent& awxEvt );
   wxString GetSelectionText();
   void FocusLatest();

private:
   // This is so we can populate event data before sending it up.
   wxDECLARE_EVENT_TABLE();

   wxListView* m_wxListControl;
   wxButton* m_wxViewButton;
   int m_iSelection;

   void buildButtons();

   void onItemSelection(wxListEvent& awxEvt);
   void onItemDeselection(wxListEvent& awxEvt);
};

