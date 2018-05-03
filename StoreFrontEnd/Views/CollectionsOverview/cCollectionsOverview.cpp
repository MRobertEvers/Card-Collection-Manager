#include "cCollectionsOverview.h"
#include "vCollectionsOverview.h"
#include "../ViewTemplates/IMenuEventSource.h"
#include "../MainWindow/MainWindow.h"

CCollectionsOverview::CCollectionsOverview(IMenuEventSource* aParent)
   : IMenuEventHandler(aParent)
{
   m_wxOverview = new VCollectionsOverview(aParent, 8);
}


CCollectionsOverview::~CCollectionsOverview()
{
}

wxPanel*
CCollectionsOverview::GetView()
{
   return m_wxOverview;
}

void
CCollectionsOverview::BindEventHandler()
{

}


void
CCollectionsOverview::handleEvent(unsigned int aiEvent)
{

}

