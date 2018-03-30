#pragma once

class IMenuEventSource;
class cCollectionsOverview;
class vCollectionsOverview;

class CollectionsOverview
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

