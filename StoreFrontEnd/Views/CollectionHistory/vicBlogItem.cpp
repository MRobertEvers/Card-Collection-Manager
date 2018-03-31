#include "vicBlogItem.h"
#include "vicBlogHeader.h"
#include "vicBlogFooter.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"
#include "../wxExtras/wxImagePanel.h"
#include "../wxExtras/wxInfiniteGrid.h"


myImageGridCellRenderer::myImageGridCellRenderer( const wxString& aszImagePath )
   : m_szPath( aszImagePath )
{
   wxImage cellImage;
   if( cellImage.LoadFile( m_szPath ) )
   {
      m_bitMap = wxBitmap( cellImage );
   }
}

void
myImageGridCellRenderer::Draw( wxGrid& grid, wxGridCellAttr& attr, wxDC& dc,
                               const wxRect& rect, int row, int col, bool isSelected )
{
   auto ptSFE = StoreFrontEnd::Client();

   // Erase this cells background
   wxGridCellRenderer::Draw( grid, attr, dc, rect, row, col, isSelected );

   wxRect newRect = rect;
   newRect.SetLeft( rect.GetLeft() + m_bitMap.GetSize().GetWidth() );

   // Draw the text shifted to the right past the image (so the image doesnt draw over it).
   wxGridCellStringRenderer::Draw( grid, attr, dc, newRect, row, col, isSelected );

   // Draw the image.
   dc.DrawBitmap( m_bitMap, rect.x, rect.y + (rect.height/2 - m_bitMap.GetSize().GetHeight()/2) );
}

wxSize
myImageGridCellRenderer::GetBestSize( wxGrid& grid,
                                      wxGridCellAttr& attr,
                                      wxDC& dc,
                                      int row, int col )
{
   return wxGridCellStringRenderer::GetBestSize(grid, attr, dc, row, col) + m_bitMap.GetSize();
}


BEGIN_EVENT_TABLE( vicBlogItem, wxPanel )
EVT_SIZE( vicBlogItem::onResize )
EVT_SCROLLWIN( vicBlogItem::onScroll )
END_EVENT_TABLE()

vicBlogItem::vicBlogItem( wxWindow* aptParent,
                          const wxString& aszTitle,
                          const wxString& aszFooter,
                          const std::vector<std::string>& avecHistory )
 : wxPanel( aptParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER )
{
   auto ptSF = StoreFrontEnd::Server();
   auto ptSFE = StoreFrontEnd::Client();

   wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( sizer );

   // Add the header
   vicBlogHeader* header = new vicBlogHeader( this, aszTitle, "" );
   sizer->Add( header, wxSizerFlags( 0 ).Expand() );

   m_ptGrid = new wxInfiniteGrid( this, wxID_ANY );
   m_ptGrid->CreateGrid( 0, 1 );
   m_ptGrid->DisableDragColSize();
   m_ptGrid->DisableDragRowSize();
   m_ptGrid->HideRowLabels();
   m_ptGrid->HideColLabels();
   m_ptGrid->EnableGridLines( false );
   sizer->Add( m_ptGrid, wxSizerFlags( 0 ).Expand() );

   for( auto& szHistLine : avecHistory )
   {
      InterfaceLineParser FirstItem;
      InterfaceLineParser SecondItem;
      StringInterface::InterfaceLineType Type;

      bool ParseResult = StringInterface::ParseInterfaceLine( szHistLine,
         FirstItem.Count, FirstItem.Name, FirstItem.DetailVec, FirstItem.MetaVec,
         SecondItem.Count, SecondItem.Name, SecondItem.DetailVec, SecondItem.MetaVec,
         Type );

      if( !ParseResult )
      {
         continue;
      }

      if( FirstItem.Name != "" )
      {
         wxString szImgFilePath;
         wxString szLineText;
         if( Type == StringInterface::AddLine )
         {
            szImgFilePath = ptSFE->GetAddSymbolFilePath();
            szLineText = FirstItem.Name;
         }
         else if( Type == StringInterface::RemoveLine )
         {
            szImgFilePath = ptSFE->GetRemSymbolFilePath();
            szLineText = FirstItem.Name;
         }
         else if( ( Type == StringInterface::ChangeLine ) &&
                  ( SecondItem.Name != "" ) )
         {
            szImgFilePath = ptSFE->GetSwapSymbolFilePath();
            szLineText = FirstItem.Name + " > " + SecondItem.Name;
         }

         // Add the row
         int iRow = m_ptGrid->GetNumberRows();
         m_ptGrid->InsertRows( m_ptGrid->GetNumberRows() );
         m_ptGrid->SetCellValue( iRow, 0, szLineText );
         // grid->SetCellFont( iRow, 0, awxFont );
         m_ptGrid->SetReadOnly( iRow, 0, true );
         // grid->SetCellBackgroundColour( iRow, 0, awxColor );
         //grid->SetCellTextColour( iRow, 0, awxFontColor );
         m_ptGrid->SetCellRenderer( iRow, 0, new myImageGridCellRenderer( szImgFilePath ) );
      }
   }
   m_ptGrid->AutoSizeColumn( 0, true );
   vicBlogFooter* footer = new vicBlogFooter( this, aszFooter );
   sizer->Add( footer, wxSizerFlags( 0 ).Expand() );
}


vicBlogItem::~vicBlogItem()
{
}

void
vicBlogItem::onScroll( wxScrollWinEvent& awxEvt )
{
   awxEvt.ResumePropagation( 999 );
   awxEvt.Skip();
}


void 
vicBlogItem::onResize( wxSizeEvent &ev )
{
   // m_ptGrid->SetColSize( 0, this->GetClientSize().GetWidth() );

   ev.Skip();
}

