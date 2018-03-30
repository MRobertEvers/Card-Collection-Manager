#include"cCollectionsOverview.h"
#include"vCollectionsOverview.h"
#include "IMenuEventSource.h"
#include"MainWindow.h"

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

