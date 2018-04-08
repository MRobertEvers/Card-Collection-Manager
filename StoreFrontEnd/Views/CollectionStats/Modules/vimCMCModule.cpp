#include "vimCMCModule.h"
#include "../StoreFrontEnd/StoreFrontEnd.h"

#include <map>
#include <vector>
#include <string>
#include <set>
#include <wx/bars/barplot.h>
#include <wx/axis/numberaxis.h>
#include <wx/axis/categoryaxis.h>
#include <wx/xy/xyhistorenderer.h>
#include <wx/category/categorysimpledataset.h>
#include <wx/ClickableShape.h>
#include <wx/tokenzr.h>
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
            "CMC: " + myData->GetCategoryName(),
            wxString( myData->GetSeriesName() + ": " + std::to_string( myData->GetSeriesValue() ) ),
            wxString( "Total: " + std::to_string( myData->GetCategoryTotalOfAllSeries() ) )
         }
      ) );
   }
   else
   {
      m_Panel->GetChart()->SetTooltip( nullptr );
   }
   m_Panel->ChartChanged( nullptr );
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

   std::map<unsigned int, std::map<wxString, unsigned int>> mapCMCs;
   std::set<wxString> setColors;
   unsigned int iTotalCMC = 0;
   
   // TODO: Use group class to generate this grouping
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
      setColors.insert( GetColorID( item ) );
      if( iter_cmc == mapCMCs.end() )
      {
         mapCMCs[iCMC][GetColorID(item)] = iNumber;
      }
      else
      {
         mapCMCs[iCMC][GetColorID( item )] += iNumber;
      }

      iTotalCMC += iCMC;
   }

   this->SetSizeHints( 360, 360 );

   std::vector<wxString> vecCategories;
   std::map<wxString, std::vector<double>> SerieArray;
   int iLastCMC = -1;
   for( auto& cmc : mapCMCs )
   {
      while( iLastCMC < ((int)cmc.first - 1) )
      {
         vecCategories.push_back( std::to_string( ++iLastCMC ) );
         for( auto& color : setColors )
         {
            SerieArray[color].push_back( 0 );
         }
      }
      vecCategories.push_back( std::to_string(cmc.first) );
      for( auto& color : setColors )
      {
         auto iter_color_set = cmc.second.find( color );
         if( iter_color_set != cmc.second.end() )
         {
            SerieArray[color].push_back( iter_color_set->second );
         }
         else
         {
            SerieArray[color].push_back( 0 );
         }
      }

      iLastCMC = cmc.first;
   }

   // Create dataset
   CategorySimpleDataset *dataset = new CategorySimpleDataset( vecCategories.data(), vecCategories.size() );//WXSIZEOF( names ) );

   // add two series to it
   for( auto& serie : SerieArray )
   {
      dataset->AddSerie( serie.first, serie.second.data(), serie.second.size() );//WXSIZEOF( values1 ) );
   }
   //dataset->AddSerie( wxT( "Serie 2" ), values2, WXSIZEOF( values2 ) );


   unsigned int barSize = 360 / (vecCategories.size()+3);
   if( barSize < 14 )
   {
      barSize = 14;
   }

   // Create stacked bar type
   BarType *barType = new StackedBarType( barSize, 0 );

   // Set bar renderer for it, with stacked bar type
   ClickableBarRenderer *renderer = new ClickableBarRenderer( barType );

   std::vector<wxColour> colors
      = std::vector<wxColour>({
      DEFAULT_BAR_FILL_COLOUR_0,
      DEFAULT_BAR_FILL_COLOUR_1,
      DEFAULT_BAR_FILL_COLOUR_2,
      DEFAULT_BAR_FILL_COLOUR_3,
      DEFAULT_BAR_FILL_COLOUR_4,
      DEFAULT_BAR_FILL_COLOUR_5,
      DEFAULT_BAR_FILL_COLOUR_6
   });
   
   for( size_t i = 0; i < SerieArray.size(); i++ )
   {
      renderer->SetBarDraw( i, new ClickableGradientAreaDraw( colors[i], colors[i],
                                                              colors[i].ChangeLightness( 150 ), wxNORTH,
                                                              new ClickableCategoryData( dataset ) ) );
   }
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

wxString 
vimCMCModule::GetColorID( CardInterface& item )
{
   wxString szColor = item.GetAttribute( "colorIdentity" );
   wxStringTokenizer tkzer( szColor, "::", false);
   std::vector<wxString> vecColors;// = StringHelper::Str_Split( szColor.ToStdString(), "::" );
   while( tkzer.HasMoreTokens() )
   {
      vecColors.push_back( tkzer.GetNextToken() );
   }

   if( vecColors.size() > 1 )
   {
      return "Gold";
   }
   else if( vecColors.size() == 1 )
   {
      szColor = vecColors[0];
      if( szColor == "U" )
      {
         return "Blue";
      }
      else if( szColor == "W" )
      {
         return "White";
      }
      else if( szColor == "B" )
      {
         return "Black";
      }
      else if( szColor == "G" )
      {
         return "Green";
      }
      else if( szColor == "R" )
      {
         return "Red";
      }
   }
   else
   {
      return "Colorless";
   }
}