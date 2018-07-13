#include "VCardAttributesEditor.h"
#include "CCardAttributesEditor.h"
#include "../StoreFrontEnd/CardInterface.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../StoreFrontEnd/CollectionInterface.h"



wxBEGIN_EVENT_TABLE( VCardAttributesEditor, wxPanel )
EVT_GRID_CELL_CHANGING( VCardAttributesEditor::onAttributeChanged )
EVT_BUTTON( VCardAttributesEditor::NEED_CLEAR_ROW, VCardAttributesEditor::onClearRow )
EVT_BUTTON( VCardAttributesEditor::SAVE_BUTTON, VCardAttributesEditor::onSaveButton )
EVT_BUTTON( VCardAttributesEditor::RESET_BUTTON, VCardAttributesEditor::onResetButton )
wxEND_EVENT_TABLE()

VCardAttributesEditor::VCardAttributesEditor( wxWindow* aptParent, wxWindowID aiWID )
   : wxPanel(aptParent, aiWID)
{
   m_mgr.SetManagedWindow( this );
}

VCardAttributesEditor::~VCardAttributesEditor()
{
   m_mgr.UnInit();
}

void
VCardAttributesEditor::SetController( CCardAttributesEditor* apController )
{
   m_pController = apController;
}

void
VCardAttributesEditor::ViewCard( CardInterface* apCard )
{
   this->Freeze();
   if( m_pMetaTable != nullptr )
   {
      m_mgr.ClosePane( m_mgr.GetPane( m_pMetaTable ) );
   }
   if( m_pAttributesTable != nullptr )
   {
      m_mgr.ClosePane( m_mgr.GetPane( m_pAttributesTable ) );
   }
   if( m_pUIDTable != nullptr )
   {
      m_mgr.ClosePane( m_mgr.GetPane( m_pUIDTable ) );
   }
   if( m_pButtonPanel != nullptr )
   {
      m_mgr.ClosePane( m_mgr.GetPane( m_pButtonPanel ) );
   }
   m_mapAttributeOptions.clear();
   m_mapAttributeDefaults.clear();
   m_mapMetaDefaults.clear();

   m_pCardInterface = apCard;
   m_pAttributesTable = new wxGrid( this, 45, wxDefaultPosition, wxDefaultSize, wxBORDER );
   m_pAttributesTable->CreateGrid( 0, 1 );
   m_pAttributesTable->SetRowLabelSize( 124 );
   m_pAttributesTable->HideColLabels();
   m_pAttributesTable->DisableDragColSize();
   m_pAttributesTable->DisableDragRowSize();
   auto items = apCard->GetAttributes();
   for( auto& item : items )
   {
      int iRow = m_pAttributesTable->GetNumberRows();
      m_pAttributesTable->InsertRows( iRow );
      m_pAttributesTable->SetRowLabelValue( iRow, item.first );
      m_pAttributesTable->SetCellValue( iRow, 0, item.second );

      auto opts = apCard->GetAttributeOptions( item.first );
      m_pAttributesTable->SetCellEditor( iRow, 0, new wxGridCellChoiceEditor( toArrayString( opts ), false ) );

      m_mapAttributeOptions.emplace( item.first, opts );
      m_mapAttributeDefaults.emplace( item.first, item.second );
   }
   m_pAttributesTable->AutoSize();
   m_mgr.AddPane( m_pAttributesTable, 
      wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false ).Floatable( false ).CenterPane() );

   m_pMetaTable = new wxGrid( this, 46, wxDefaultPosition, wxDefaultSize, wxBORDER );
   m_pMetaTable->SetRowLabelSize( 124 );
   m_pMetaTable->CreateGrid( 0, 1 );
   m_pMetaTable->EnableGridLines( false );
   m_pMetaTable->HideColLabels();

   m_pAttributesTable->EnableGridLines( false );
   m_pAttributesTable->DisableDragColSize();
   m_pAttributesTable->DisableDragRowSize();
   items = apCard->GetMetaTags();
   for( auto& item : items )
   {
      if( item.first.substr( 0, 2 ) == "__" )
      {
         continue;
      }

      int iRow = m_pMetaTable->GetNumberRows();
      m_pMetaTable->InsertRows( iRow );
      m_pMetaTable->SetRowLabelValue( iRow, item.first );
      m_pMetaTable->SetCellValue( iRow, 0, item.second );
      m_mapMetaDefaults.emplace( item.first, item.second );
   }

   int iRow = m_pMetaTable->GetNumberRows();
   m_pMetaTable->InsertRows( iRow );
   m_pMetaTable->SetRowLabelValue( iRow, "*" );
   m_pMetaTable->SetCellValue( iRow, 0, "<add new>" );
   m_mapMetaDefaults.emplace( "*", "<add new>" );
   // Add an empty cell for editing
   // If editing from something to nothing -> remove
   // If editing from nothing to something set as label

   m_pMetaTable->AutoSize();
   m_mgr.AddPane( m_pMetaTable,
                  wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false ).Floatable( false ).CenterPane() );

   // Add the uid list.
   m_pUIDTable = new wxGrid( this, ITEM_GRID );
   m_pUIDTable->CreateGrid( 0, 1 );
   m_pUIDTable->HideColLabels();
   auto vecUIDs = apCard->GetRepresentingUIDs();
   bool bFirst = true;
   for( auto& uid : vecUIDs )
   {
      int iRow = m_pUIDTable->GetNumberRows();
      m_pUIDTable->InsertRows( iRow );
      m_pUIDTable->SetRowLabelValue( iRow, uid );
      m_pUIDTable->SetCellRenderer( iRow, 0, new wxGridCellBoolRenderer() );
      m_pUIDTable->SetCellEditor( iRow, 0, new wxGridCellBoolEditor() );

      if( bFirst )
      {
         m_pController->SelectUID( uid );
      }
      bFirst = false;
   }

   m_pUIDTable->DisableDragColSize();
   m_pUIDTable->DisableDragRowSize();
   m_pUIDTable->EnableGridLines( false );
   m_pUIDTable->AutoSize();
   m_mgr.AddPane( m_pUIDTable, wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false )
                                              .Floatable( false ).Right().Fixed().BestSize( wxSize( 200, -1 ) ) );

   m_pButtonPanel = new wxPanel( this, wxID_ANY );
   m_pButtonPanel->SetSizer( new wxGridSizer( 2, 0, 0 ) );
   m_pButtonPanel->GetSizer()->Add( new wxButton( m_pButtonPanel, SAVE_BUTTON, "Save" ), wxSizerFlags( 1 ).Expand() );
   m_pButtonPanel->GetSizer()->Add( new wxButton( m_pButtonPanel, RESET_BUTTON, "Reset" ), wxSizerFlags( 1 ).Expand() );
   m_mgr.AddPane( m_pButtonPanel, wxAuiPaneInfo().CaptionVisible( false ).CloseButton( false )
                                                 .Floatable( false ).Bottom().BestSize( wxSize( 223, 44 ) ) );

   m_mgr.Update();
   this->Thaw();
}

void 
VCardAttributesEditor::SelectUID( const wxString & aszUID )
{
   if( m_pUIDTable != nullptr )
   {
      for( int i = 0; i < m_pUIDTable->GetNumberRows(); i++ )
      {
         if( m_pUIDTable->GetRowLabelValue( i ) == aszUID )
         {
            m_pUIDTable->SetCellValue( i, 0, "1" );
            break;
         }
      }
   }
}

void 
VCardAttributesEditor::UnselectUID( const wxString & aszUID )
{
   if( m_pUIDTable != nullptr )
   {
      for( int i = 0; i < m_pUIDTable->GetNumberRows(); i++ )
      {
         if( m_pUIDTable->GetRowLabelValue( i ) == aszUID )
         {
            m_pUIDTable->SetCellValue( i, 0, "0" );
            break;
         }
      }
   }
}

std::map<std::string, std::string> 
VCardAttributesEditor::GetNonDefaultAttributes()
{
   std::map<std::string, std::string> mapRetval;
   for( int i = 0; i < m_pAttributesTable->GetNumberRows(); i++ )
   {
      auto szRow = m_pAttributesTable->GetRowLabelValue( i );
      if( szRow.size() > 1 && szRow[szRow.size() - 1] == '*' )
      {
         auto szVal = m_pAttributesTable->GetCellValue( i, 0 );
         mapRetval.emplace( szRow.substr(0, szRow.size()-1), szVal );
      }
   }
   return mapRetval;
}

std::map<std::string, std::string>
VCardAttributesEditor::GetNonDefaultMetaTags()
{
   auto mapRemed = m_mapMetaDefaults;
   mapRemed.erase( "*" );
   std::map<std::string, std::string> mapRetval;
   for( int i = 0; i < m_pMetaTable->GetNumberRows(); i++ )
   {
      auto szRow = m_pMetaTable->GetRowLabelValue( i );
      if( szRow.size() > 1 && szRow[szRow.size() - 1] == '*' )
      {
         auto szVal = m_pMetaTable->GetCellValue( i, 0 );
         mapRetval.emplace( szRow.substr( 0, szRow.size() - 1 ), szVal );
         szRow = szRow.substr( 0, szRow.size() - 1 );
      }

      auto iter_didhave = mapRemed.find(szRow);
      if( iter_didhave != mapRemed.end() )
      {
         mapRemed.erase( iter_didhave );
      }
   }

   for( auto& erased : mapRemed )
   {
      mapRetval.emplace( erased.first, "" );
   }

   return mapRetval;
}

void
VCardAttributesEditor::ReassignDefaults()
{
   for( int i = 0; i < m_pAttributesTable->GetNumberRows(); i++ )
   {
      auto szRow = m_pAttributesTable->GetRowLabelValue( i );
      if( szRow.size() > 1 && szRow[szRow.size() - 1] == '*' )
      {
         szRow = szRow.substr( 0, szRow.size() - 1 );
      }

      auto iter_find = m_mapAttributeDefaults.find( szRow );
      if( iter_find != m_mapAttributeDefaults.end() )
      {
         iter_find->second = m_pAttributesTable->GetCellValue( i, 0 );
      }
      m_pAttributesTable->SetRowLabelValue( i, szRow );
   }

   for( int i = 0; i < m_pMetaTable->GetNumberRows(); i++ )
   {
      auto szRow = m_pMetaTable->GetRowLabelValue( i );
      if( szRow.size() > 1 && szRow[szRow.size() - 1] == '*' )
      {
         szRow = szRow.substr( 0, szRow.size() - 1 );
      }

      auto iter_find = m_mapMetaDefaults.find( szRow );
      if( iter_find != m_mapMetaDefaults.end() )
      {
         iter_find->second = m_pMetaTable->GetCellValue( i, 0 );
      }
      m_pMetaTable->SetRowLabelValue( i, szRow );
   }
}

void 
VCardAttributesEditor::onAttributeChanged( wxGridEvent& awxEvt )
{
   int iRow = awxEvt.GetRow();
   auto szNewVal = awxEvt.GetString();
   if( awxEvt.GetId() == 45 )
   {
      auto szRow = m_pAttributesTable->GetRowLabelValue( iRow );
      if( szRow[szRow.size() - 1] == '*' )
      {
         szRow = szRow( 0, szRow.size() - 1 );
      }

      auto iter_type = m_mapAttributeDefaults.find( szRow );
      if( iter_type != m_mapAttributeDefaults.end() )
      {
         if( iter_type->second != szNewVal )
         {
            m_pAttributesTable->SetRowLabelValue( iRow, szRow + "*" );
         }
         else
         {
            m_pAttributesTable->SetRowLabelValue( iRow, szRow );
         }

         auto iter_find = std::find( m_mapAttributeOptions[szRow].begin(), m_mapAttributeOptions[szRow].end(), awxEvt.GetString() );
         if( iter_find == m_mapAttributeOptions[szRow].end() )
         {
            return;
         }

         int ind = std::distance( m_mapAttributeOptions[szRow].begin(), iter_find );
         auto vecPairs = m_pCardInterface->GetPairedAttributes( szRow.ToStdString() );
         for( auto& pair : vecPairs )
         {
            for( int i = 0; i < m_pAttributesTable->GetNumberRows(); i++ )
            {
               auto szTargetRow = m_pAttributesTable->GetRowLabelValue( i );
               if( szTargetRow[szTargetRow.size() - 1] == '*' )
               {
                  szTargetRow = szTargetRow( 0, szTargetRow.size() - 1 );
               }
               if( szTargetRow == pair )
               {
                  m_pAttributesTable->SetCellValue( i, 0, m_mapAttributeOptions[szTargetRow][ind] );
                  m_pAttributesTable->SetRowLabelValue( i, szTargetRow + "*" );
               }
            }
            
         }
      }
   }
   else if( awxEvt.GetId() == 46 )
   {
      auto szRow = m_pMetaTable->GetRowLabelValue( iRow );
      if( szRow != "*" )
      {
         if( szRow[szRow.size() - 1] == '*' )
         {
            szRow = szRow( 0, szRow.size() - 1 );
         }

         if( !szNewVal.IsEmpty() )
         {
            auto iter_type = m_mapMetaDefaults.find( szRow );
            if( iter_type != m_mapMetaDefaults.end() )
            {
               if( iter_type->second != awxEvt.GetString() )
               {
                  m_pMetaTable->SetRowLabelValue( iRow, szRow + "*" );
               }
               else
               {
                  m_pMetaTable->SetRowLabelValue( iRow, szRow );
               }
            }
            else
            {
               m_pMetaTable->SetRowLabelValue( iRow, szRow + "*" );
            }
         }
         else
         {
            wxCommandEvent updateEvt(wxEVT_BUTTON);
            updateEvt.SetId( NEED_CLEAR_ROW );
            updateEvt.SetInt( iRow );
            // Data used in the callback.

            ::wxPostEvent( this, updateEvt );
         }
      }
      else if( !szNewVal.IsEmpty() )
      {
         m_pMetaTable->SetRowLabelValue( iRow, szNewVal + "*" );
         m_pMetaTable->SetCellValue( iRow, 0, "<value>" );

         // Add another new row.
         int iRow = m_pMetaTable->GetNumberRows();
         m_pMetaTable->InsertRows( iRow );
         m_pMetaTable->SetRowLabelValue( iRow, "*" );
         m_pMetaTable->SetCellValue( iRow, 0, "<add new>" );
      }
      else
      {
         // They tried to add a blank tag
      }
   }
   else if( awxEvt.GetId() == ITEM_GRID )
   {
      auto szRow = m_pUIDTable->GetRowLabelValue( iRow );
      if( awxEvt.GetString() == "1" )
      {
         m_pController->SelectUID( szRow.ToStdString() );
      }
      else
      {
         m_pController->UnselectUID( szRow.ToStdString() );
      }
   }
}

void
VCardAttributesEditor::onClearRow( wxCommandEvent & awxEvt )
{
   int iRow = awxEvt.GetInt();
   for( int i = iRow; i < m_pMetaTable->GetNumberRows()-1; i++ )
   {
      m_pMetaTable->SetRowLabelValue( i, m_pMetaTable->GetRowLabelValue( i + 1 ) );
   }
   m_pMetaTable->DeleteRows( iRow );
   
}

void 
VCardAttributesEditor::onSaveButton( wxCommandEvent & awxEvt )
{
   m_pController->OnSave();
}

void
VCardAttributesEditor::onResetButton( wxCommandEvent & awxEvt )
{
   for( auto& def : m_mapAttributeDefaults )
   {
      for( int i = 0; i < m_pAttributesTable->GetNumberRows(); i++ )
      {
         if( m_pAttributesTable->GetRowLabelValue( i ).find( def.first ) == 0 )
         {
            m_pAttributesTable->SetRowLabelValue( i, def.first );
            m_pAttributesTable->SetCellValue( i, 0, def.second );
            break;
         }
      }
   }
   auto tmpDef = m_mapMetaDefaults;
   tmpDef.erase( "*" );
   auto iter_tmp = tmpDef.begin();
   while( iter_tmp != tmpDef.end() )
   {
      auto& def = *iter_tmp;
      bool bErase = false;
      for( int i = 0; i < m_pMetaTable->GetNumberRows(); i++ )
      {
         if( m_pMetaTable->GetRowLabelValue( i ).find( def.first ) == 0 )
         {
            m_pMetaTable->SetRowLabelValue( i, def.first );
            m_pMetaTable->SetCellValue( i, 0, def.second );
            bErase = true;
            break;
         }
      }

      if( bErase )
      {
         iter_tmp = tmpDef.erase( iter_tmp );
      }
      else
      {
         iter_tmp++;
      }
   }

   for( auto addback : tmpDef )
   {
      int iRow = m_pMetaTable->GetNumberRows();
      if( iRow > 0 )
      {
         iRow--;
      }
      iRow = m_pMetaTable->InsertRows( iRow );
      m_pMetaTable->SetRowLabelValue( iRow, addback.first );
      m_pMetaTable->SetCellValue( iRow, 0, addback.second );
   }
}

wxArrayString
VCardAttributesEditor::toArrayString( const std::vector<std::string>& avecString )
{
   wxArrayString arrStr;
   for( auto& str : avecString )
   {
      arrStr.Add( str );
   }
   return arrStr;
}
