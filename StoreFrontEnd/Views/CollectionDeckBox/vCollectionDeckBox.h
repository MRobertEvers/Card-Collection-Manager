#pragma once
#include "../ViewTemplates/ivCollectionView.h"
#include <wx/wxprec.h>
#include <wx/listctrl.h>
#include <wx/object.h> 

class vcCollectionDeckBoxItemList;
class MainFrame;

class vCollectionDeckBox : public ivCollectionView
{
public:
   vCollectionDeckBox( IMenuEventSource* aptParent, wxWindowID aiWID, const wxString& aszColID);
   ~vCollectionDeckBox();

   void PrepareInterface(  );

private:
   wxDECLARE_EVENT_TABLE();

   vcCollectionDeckBoxItemList* m_vcItemList;

   void onNewItemSelected(wxListEvent& awxEvt);
   void onEditorAccept( wxCommandEvent& awxEvt ) override;
   void onCardChanged( wxCommandEvent& awxEvt ) override;
   void buildItemList();
};

