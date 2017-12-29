#pragma once
#include "wx/wxprec.h"
#include "vcCollectionsMenuList.h"

class vCollectionsOverview : public wxPanel
{
public:
   vCollectionsOverview(wxWindow* aptParent, wxWindowID aiWID);
   ~vCollectionsOverview();

private:
   vcCollectionsMenuList* m_vcCollectionsPanel;

   void buildCollectionsPreview();
   void buildCollectionSelector();


};

