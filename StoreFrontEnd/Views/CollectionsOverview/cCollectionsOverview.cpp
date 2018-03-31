#include "cCollectionsOverview.h"
#include "vCollectionsOverview.h"
#include "../ViewTemplates/IMenuEventSource.h"
#include "../MainWindow/MainWindow.h"

cCollectionsOverview::cCollectionsOverview(IMenuEventSource* aParent)
   : IMenuEventHandler(aParent)
{
   m_wxOverview = new vCollectionsOverview(aParent, 8);
}


cCollectionsOverview::~cCollectionsOverview()
{
}

wxPanel*
cCollectionsOverview::GetView()
{
   return m_wxOverview;
}

void
cCollectionsOverview::BindEventHandler()
{

}


void
cCollectionsOverview::handleEvent(unsigned int aiEvent)
{

}

