#pragma once
#include "../ViewTemplates/IControlledView.h"

class IMenuEventSource;
class CCollectionsOverview;
class VCollectionsOverview;

class CollectionsOverview : public IControlledView
{
public:
   CollectionsOverview( IMenuEventSource* aParent );
   ~CollectionsOverview();

   CCollectionsOverview* GetController();
   VCollectionsOverview* GetView();

private:
   CCollectionsOverview * m_Controller;
   VCollectionsOverview* m_View;
};

