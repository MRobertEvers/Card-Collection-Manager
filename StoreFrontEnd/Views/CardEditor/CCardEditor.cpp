#include "CCardEditor.h"
#include "VCardEditor.h"
#include "../Views/CardAttributesEditor/CardAttributesEditor.h"
#include "../Views/CardAttributesEditor/CCardAttributesEditor.h"
#include "../Views/CardView/CardView.h"
#include "../Views/CardView/CCardView.h"
#include "../StoreFrontEnd/CollectionInterface.h"
#include "../StoreFrontEnd/CardInterface.h"

CCardEditor::CCardEditor( VCardEditor* apView, int aiMax )
   : m_iMaxSelection( aiMax )
{
   m_pView = apView;
}

CCardEditor::~CCardEditor()
{
}

void
CCardEditor::SetModel( CollectionInterface* aptModel, std::vector<std::string> avecHashes )
{
   m_pModel = std::shared_ptr<CardEditorModel>( new CardEditorModel( aptModel, avecHashes ) );

   if( m_pAttributes == nullptr )
   {
      m_pAttributes = std::shared_ptr<CardAttributesEditor>( new CardAttributesEditor(m_iMaxSelection) );
      m_pView->ShowAttributesTable( m_pAttributes.get() );
   }
   if( m_ptCardViewer == nullptr )
   {
      m_ptCardViewer = std::shared_ptr<CardView>( new CardView() );
      m_pView->SetCardTitle( "" );
      m_pView->ShowCardViewer( m_ptCardViewer.get() );
   }

   m_pLastDrawnVirtual = m_pModel->GetCollectionInterface()->GetVirtualItem( avecHashes.front() );
   if( m_pLastDrawnVirtual != nullptr )
   {
      Draw( m_pLastDrawnVirtual.get() );
   }
}

void 
CCardEditor::Draw( CardInterface* apCard )
{
   m_pView->SetCardTitle( apCard->GetName() );
   m_pAttributes->GetController()->SetModel( apCard );
   m_ptCardViewer->GetController()->SetModel( apCard );
}

void 
CCardEditor::ShowWindow()
{
   m_pView->Show();
}

std::vector<std::string> 
CCardEditor::GetSelectedUIDs()
{
   std::vector<std::string> vecRetval;
   vecRetval = m_pAttributes->GetController()->GetSelected();
   return vecRetval;
}

void 
CCardEditor::uiBuildView()
{
   // wxPanel* pLeftPanel = new wxPanel( m_pView, wxID_ANY );
}

CardEditorModel::CardEditorModel( CollectionInterface* aptModel, std::vector<std::string> avecHashes )
{
   m_pModel = aptModel;
   m_vecHashes = avecHashes;
}

CardEditorModel::~CardEditorModel()
{
}

CollectionInterface*
CardEditorModel::GetCollectionInterface()
{
   return m_pModel;
}

std::vector<std::string>& 
CardEditorModel::GetHashOptions()
{
   return m_vecHashes;
}
