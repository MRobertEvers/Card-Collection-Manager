#pragma once
#include "wx/wxprec.h"
#include "ivCollectionView.h"
#include <wx/listctrl.h>
#include <wx/object.h> 

class vcCollectionDeckBoxItemList;
class MainFrame;

class vCollectionDeckBox : public ivCollectionView
{
public:
   vCollectionDeckBox(MainFrame* aptParent, wxWindowID aiWID, const wxString& aszColID);
   ~vCollectionDeckBox();

private:
   wxDECLARE_EVENT_TABLE();

   vcCollectionDeckBoxItemList* m_vcItemList;

   void onNewItemSelected(wxListEvent& awxEvt);
   void buildItemList();
};

