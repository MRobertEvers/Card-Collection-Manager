#include "CardInventoryViewer.h"
#include "VCardInventoryViewer.h"
#include "CCardInventoryViewer.h"

CardInventoryViewer::CardInventoryViewer(  )
{

}

CardInventoryViewer::~CardInventoryViewer()
{
   delete m_pController;
}

// TODO: Right now this should only be called once.
wxPanel* 
CardInventoryViewer::GetView( wxPanel * aptParent )
{
   m_pView = new VCardInventoryViewer( aptParent, wxID_ANY );

   m_pController = new CCardInventoryViewer( m_pView );
   m_pView->SetController( m_pController );

   return m_pView;
}

// TODO: This wont work for multiple "GetView" Calls
CCardInventoryViewer* 
CardInventoryViewer::GetController()
{
   return m_pController;
}
