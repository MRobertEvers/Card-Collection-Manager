#include "CardAttributesEditor.h"
#include "VCardAttributesEditor.h"
#include "CCardAttributesEditor.h"

CardAttributesEditor::CardAttributesEditor( int aiMaxSelection )
   : m_iMaxSelection(aiMaxSelection)
{
}

CardAttributesEditor::~CardAttributesEditor()
{
   delete m_pController;
}

wxWindow*
CardAttributesEditor::GetView( wxWindow* aptParent )
{
   m_pView = new VCardAttributesEditor( aptParent, wxID_ANY );

   m_pController = new CCardAttributesEditor( m_pView, m_iMaxSelection );
   m_pView->SetController( m_pController );

   return m_pView;
}

CCardAttributesEditor*
CardAttributesEditor::GetController()
{
   return m_pController;
}
