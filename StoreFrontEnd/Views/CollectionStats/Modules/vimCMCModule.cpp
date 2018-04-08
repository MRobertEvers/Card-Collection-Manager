#include "vimCMCModule.h"

#include <map>
#include <wx/bars/barplot.h>
#include <wx/axis/numberaxis.h>
#include <wx/axis/categoryaxis.h>
#include <wx/xy/xyhistorenderer.h>
#include <wx/category/categorysimpledataset.h>
#include <wx/ClickableShape.h>
#include <wx/ClickableRenderer.h>

BEGIN_EVENT_TABLE( vimCMCModule, wxChartPanel )
END_EVENT_TABLE()

void 
ClickMode::Init( wxChartPanel* panel )
{
   m_Panel = panel;
}

void
ClickMode::ShowToolTip( ClickableShape* dataShape )
{
   if( dataShape != nullptr )
   {
      auto myData = dataShape->GetData();
      m_Panel->GetChart()->SetTooltip( new TextTooltip( m_LastPoint,
         {
            myData->GetCategoryName(),
            wxString( myData->GetSeriesName() + ": " + std::to_string( myData->GetSeriesValue() ) ),
            wxString( "Total: " + std::to_string( myData->GetCategoryTotalOfAllSeries() ) )
         }
      ) );
      m_Panel->ChartChanged( nullptr );
   }
   else
   {
      m_Panel->GetChart()->SetTooltip( nullptr );
   }
}

void 
ClickMode::ChartMouseDown( wxPoint &pt, int key )
{
   m_LastPoint = pt;

   auto plot = m_Panel->GetChart()->GetPlot();
   auto intPlot = dynamic_cast<InteractivePlot*>(plot);
   if( intPlot != nullptr )
   {
      auto data = intPlot->GetDataAtPoint( pt );
      ShowToolTip( data );
   }
}

vimCMCModule::vimCMCModule( wxWindow* aptParent,
                            wxWindowID aiID,
                            std::shared_ptr<CollectionInterface> aptInterface )
   : wxChartPanel(aptParent, aiID), m_ptInterface(aptInterface)
{
   wxGridSizer* sizer = new wxGridSizer( 1, 1, 0, 0 );
   this->SetSizer( sizer );

   std::map<unsigned int, unsigned int> mapCMCs;
   unsigned int iTotalCMC = 0;
   for( auto& item : m_ptInterface->GetItemInterfaces() )
   {
      auto szCMC = item.GetCMC();
      unsigned int iNumber = item.GetNumber();
      unsigned int iNumChars = 0;
      unsigned int iCMC = std::stoul( szCMC, &iNumChars );
      if( iNumChars == 0 )
      {
         iCMC = 0;
      }

      auto iter_cmc = mapCMCs.find( iCMC );
      if( iter_cmc == mapCMCs.end() )
      {
         mapCMCs[iCMC] = iNumber;
      }
      else
      {
         mapCMCs[iCMC] += iNumber;
      }

      iTotalCMC += iCMC;
   }

   //wxStaticText* txt = new wxStaticText( this, 1, "Count: " + std::to_string( m_ptInterface->GetItemCount() ) );
   //sizer->Add( txt, wxSizerFlags( 1 ) );

   this->SetSizeHints( 360, 360 );

   std::vector<wxString> vecCategories;
   std::vector<double> vecValues1;
   int iLastCMC = -1;
   for( auto& cmc : mapCMCs )
   {
      while( iLastCMC < ((int)cmc.first - 1) )
      {
         vecCategories.push_back( std::to_string( ++iLastCMC ) );
         vecValues1.push_back( 0 );
      }
      vecCategories.push_back( std::to_string(cmc.first) );
      vecValues1.push_back( cmc.second );
      iLastCMC = cmc.first;
   }
   //wxString names[] = { // category names
   //   wxT( "0" ),
   //   wxT( "1" ),
   //   wxT( "2" ),
   //   wxT( "3" ),
   //   wxT( "4" ),
   //};

   // serie 1 values
   //double values1[] = {
   //   10,
   //   20,
   //   5,
   //   50,
   //   25,
   //};

   // serie 2 values
   //double values2[] = {
   //   16,
   //   10,
   //   15,
   //   30,
   //   45,
   //};


   // Create dataset
   CategorySimpleDataset *dataset = new CategorySimpleDataset( vecCategories.data(), vecCategories.size() );//WXSIZEOF( names ) );

   // add two series to it
   dataset->AddSerie( wxT( "Serie 1" ), vecValues1.data(), vecValues1.size() );//WXSIZEOF( values1 ) );
   //dataset->AddSerie( wxT( "Serie 2" ), values2, WXSIZEOF( values2 ) );


   unsigned int barSize = 360 / (vecValues1.size()+3);
   if( barSize < 14 )
   {
      barSize = 14;
   }

   // Create stacked bar type
   BarType *barType = new StackedBarType( barSize, 0 );

   // Set bar renderer for it, with stacked bar type
   ClickableBarRenderer *renderer = new ClickableBarRenderer( barType );

   renderer->SetBarDraw( 0, new ClickableGradientAreaDraw( DEFAULT_BAR_FILL_COLOUR_0, DEFAULT_BAR_FILL_COLOUR_0,
                                                           DEFAULT_BAR_FILL_COLOUR_0.ChangeLightness( 150 ), wxNORTH,
                                                           new ClickableCategoryData( dataset ) ) );
   //renderer->SetBarDraw( 1, new GradientAreaDraw( *wxTRANSPARENT_PEN, DEFAULT_BAR_FILL_COLOUR_1,
   //   DEFAULT_BAR_FILL_COLOUR_1.ChangeLightness( 50 ), wxSOUTH ) );

   // assign renderer to dataset - necessary step
   dataset->SetRenderer( (BarRenderer*)renderer );



   // Create bar plot
   BarPlot *plot = new BarPlot();

   // Add left category axis
   CategoryAxis *leftAxis = new CategoryAxis( AXIS_BOTTOM );
   leftAxis->SetMargins( 20, 20 );
   plot->AddAxis( leftAxis );

   // Add bottom number axis
   NumberAxis *bottomAxis = new NumberAxis( AXIS_LEFT );
   bottomAxis->SetMargins( 0, 5 );
   plot->AddAxis( bottomAxis );

   // Add dataset to plot
   plot->AddDataset( dataset );

   // Link first dataset with first horizontal axis
   plot->LinkDataHorizontalAxis( 0, 0 );

   // Link first dataset with first vertical axis
   plot->LinkDataVerticalAxis( 0, 0 );

   // and finally construct and return chart

   this->SetChart( new Chart( plot, "Mana Curve" ) );
   this->SetMode( new ClickMode() );
}

vimCMCModule::~vimCMCModule()
{

}