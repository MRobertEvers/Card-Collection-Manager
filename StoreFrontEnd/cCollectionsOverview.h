#pragma once
#include<map>
#include<wx/wxprec.h>
#include"IMenuEventHandler.h"

class vCollectionsOverview;
class MainFrame;

class cCollectionsOverview : public IMenuEventHandler
{
public:

   cCollectionsOverview( IMenuEventSource* aParent );
   virtual ~cCollectionsOverview();

   wxPanel* GetView() override;

   void BindEventHandler();

protected:
   void handleEvent(unsigned int aiEvent);

private:
   vCollectionsOverview * m_wxOverview;
   std::map<wxString, unsigned int> m_mapTitleToEventID;
};