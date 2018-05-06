#pragma once
#include <wx/wxprec.h>

class vcCollectionsMenuList;
class CCollectionsOverview;

class VCollectionsOverview : public wxPanel
{
public:
   enum
   {
      Load_Collection = 0x0,
      Add_Collection = 0x1,
      View_Collection = 0x2
   };

   VCollectionsOverview(wxWindow* aptParent, wxWindowID aiWID);
   ~VCollectionsOverview();

   void SetController( CCollectionsOverview* aptController );

   void OnLoadCollection(wxCommandEvent& awxEvt);


private:
   wxDECLARE_EVENT_TABLE();

   CCollectionsOverview* m_pController;
   vcCollectionsMenuList* m_vcCollectionsPanel;

   void onLoadCollection( wxCommandEvent& awxEvt );
   void buildCollectionsPreview();
   void buildCollectionSelector();
   void getLoadedCollections();
};

