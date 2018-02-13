#include"cCollectionsOverview.h"
#include"vCollectionsOverview.h"
#include"MainWindow.h"

cCollectionsOverview::cCollectionsOverview(MainFrame* aParent)
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

