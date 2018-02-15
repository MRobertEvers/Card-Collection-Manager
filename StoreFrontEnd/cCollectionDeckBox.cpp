#include "cCollectionDeckBox.h"
#include "vCollectionDeckBox.h"
#include "StoreFrontEnd.h"
#include "TestPopup.h"

cCollectionDeckBox::cCollectionDeckBox(MainFrame* aParent, const wxString& aszColName)
   : IMenuEventHandler(aParent), m_ColID(aszColName)
{
   auto ptSF = StoreFrontEnd::Instance();
   m_wxDeckbox = new vCollectionDeckBox(aParent, 9, wxString(m_ColID));

   registerSendMenuEvents();
}

cCollectionDeckBox::~cCollectionDeckBox()
{
   m_wxDeckbox->Destroy();
}

wxPanel* 
cCollectionDeckBox::GetView()
{
   return m_wxDeckbox;
}

void
cCollectionDeckBox::BindEventHandler()
{
   prepareMenuItem("Edit", Menu_DeckEditor);
   registerMenu("Collection");
}

void
cCollectionDeckBox::handleEvent(unsigned int aiEvent)
{
   if( aiEvent == Menu_DeckEditor )
   {
      m_wxDeckbox->ShowCollectionEditor();
   }
}
