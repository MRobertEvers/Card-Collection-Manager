#include "CCardAttributesEditor.h"
#include "VCardAttributesEditor.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../StoreFrontEnd/CollectionInterface.h"
#include "../Views/CollectionView/VCollectionView.h"

CCardAttributesEditor::CCardAttributesEditor( VCardAttributesEditor* apView, int aiMaxSelection )
   : m_iMaxSelection(aiMaxSelection)
{
   m_pView = apView;
}

CCardAttributesEditor::~CCardAttributesEditor()
{
}

void 
CCardAttributesEditor::SetModel( CardInterface* aptModel )
{
   if( aptModel != nullptr )
   {
      m_pModel = aptModel;
      m_pView->ViewCard( aptModel );
   }
}

void 
CCardAttributesEditor::SelectUID( const std::string& aszUID )
{
   if( m_iMaxSelection != -1 && m_vecSelectedUIDs.size() + 1 > m_iMaxSelection )
   {
      m_vecSelectedUIDs.erase( m_vecSelectedUIDs.begin() );
      m_pView->UnselectUID( aszUID );
   }
   m_vecSelectedUIDs.push_back( aszUID );
   m_pView->SelectUID( aszUID );
}

void 
CCardAttributesEditor::UnselectUID( const std::string& aszUID )
{
   // Expects that the view already unselected the item.
   auto iter_erase = std::find( m_vecSelectedUIDs.begin(), m_vecSelectedUIDs.end(), aszUID );
   if( iter_erase != m_vecSelectedUIDs.end() )
   {
      m_vecSelectedUIDs.erase( iter_erase );
   }
}

int
CCardAttributesEditor::GetSelectedNum()
{
   return m_vecSelectedUIDs.size();
}

std::vector<std::string>
CCardAttributesEditor::GetSelected()
{
   return m_vecSelectedUIDs;
}

void 
CCardAttributesEditor::DeselectOne( const std::string & aszUID )
{
   if( aszUID == "" && m_vecSelectedUIDs.size() > 0 )
   {
      auto szFirst = *m_vecSelectedUIDs.begin();
      m_vecSelectedUIDs.erase( m_vecSelectedUIDs.begin() );
      m_pView->UnselectUID( szFirst );
   }
   else
   {
      // Expects that the view already unselected the item.
      auto iter_erase = std::find( m_vecSelectedUIDs.begin(), m_vecSelectedUIDs.end(), aszUID );
      if( iter_erase != m_vecSelectedUIDs.end() )
      {
         m_vecSelectedUIDs.erase( iter_erase );
         m_pView->UnselectUID( aszUID );
      }
   }
}

void 
CCardAttributesEditor::OnSave()
{
   auto mapAttr = m_pView->GetNonDefaultAttributes();
   auto mapMeta = m_pView->GetNonDefaultMetaTags();
   
   std::vector<std::pair<std::string, std::string>> vecAttr( mapAttr.begin(), mapAttr.end() );
   std::vector<std::pair<std::string, std::string>> vecMeta( mapMeta.begin(), mapMeta.end() );

   std::vector<std::string> vecChanges;
   for( auto& uid : m_vecSelectedUIDs )
   {
      auto szChange = m_pModel->SetAttributes( uid, vecAttr );
      m_pModel->SetMetaTags( uid, vecMeta );
      vecChanges.push_back( szChange );
   }

   m_pView->ReassignDefaults();

   wxCommandEvent updateEvt( wxEVT_BUTTON );
   updateEvt.SetId( VCollectionView::COLLECTION_EDITED );
   updateEvt.SetClientData( new CollectionDelta( vecChanges ) );

   ::wxPostEvent( m_pView, updateEvt );
}
