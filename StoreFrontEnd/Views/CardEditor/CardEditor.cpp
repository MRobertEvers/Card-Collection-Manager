#include "CardEditor.h"
#include "VCardEditor.h"
#include "CCardEditor.h"

CardEditor::CardEditor( int aiMaxSelection )
   : m_iMaxSelectionTemp(aiMaxSelection)
{
}

CardEditor::~CardEditor()
{
   delete m_pController;
}

wxWindow* 
CardEditor::GetView( wxWindow* aptParent )
{
   if( m_pView == nullptr )
   {
      m_pView = new VCardEditor( aptParent, wxID_ANY );

      m_pController = new CCardEditor( m_pView, m_iMaxSelectionTemp );
      m_pView->SetController( m_pController );
   }

   return m_pView;
}

CCardEditor* 
CardEditor::GetController()
{
   return m_pController;
}
