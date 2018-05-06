#pragma once
#include "../ViewTemplates/IControlledView.h"

class wxFrame;
class wxPanel;
class IMenuEventSource;
class CCollectionsOverview;
class VCollectionsOverview;

class CollectionsOverview : public IControlledView
{
public:
   CollectionsOverview( wxFrame* aParent );
   ~CollectionsOverview();

   // IControlledView
   std::shared_ptr<IMenuEventHandler> GetEventHandler();
   wxPanel* GetView();

private:
   CCollectionsOverview * m_pController;
   VCollectionsOverview* m_pView;
};

