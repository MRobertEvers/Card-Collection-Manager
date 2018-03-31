#pragma once
#include "../ViewTemplates/IControlledView.h"

class IMenuEventSource;
class cCollectionsOverview;
class vCollectionsOverview;

class CollectionsOverview : public IControlledView
{
public:
   CollectionsOverview( IMenuEventSource* aParent );
   ~CollectionsOverview();

   cCollectionsOverview* GetController();
   vCollectionsOverview* GetView();

private:
   cCollectionsOverview * m_Controller;
   vCollectionsOverview* m_View;
};

