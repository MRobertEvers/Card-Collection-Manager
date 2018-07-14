#pragma once
#include "../Views/ViewTemplates/IMenuEventHandler.h"

class CCollectionView;

class CollectionViewMenu : public IMenuEventHandler
{
public:
   enum Events
   {
      Menu_Save = 0x3,
      Menu_Edit,
      Menu_Stats,
      Menu_History,
      Menu_XMage
   };

public:
   CollectionViewMenu( CCollectionView* apController );
   ~CollectionViewMenu();

   // IMenuEventHandler
   // The instance handle events here
   virtual void OnMenuEvent( wxCommandEvent& awxEvt );

   // The instance puts itself on the menubar
   virtual void BindToMenu( wxMenuBar* apMenu, wxEvtHandler* apFrame );

   virtual void ReleaseMenu( wxMenuBar* apMenu, wxEvtHandler* apFrame );

private:
   CCollectionView* m_pController;

   void appendToMenu( wxEvtHandler* apFrame, wxMenu* apMenu, const wxString& aszItemName, unsigned int aiID );
};