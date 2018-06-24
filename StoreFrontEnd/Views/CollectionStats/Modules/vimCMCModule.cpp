#include "vimCMCModule.h"
#include "../vStatsViewer.h"
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
#include <wx/tokenzr.h>
#include <wx/ClickableRenderer.h>
#include <wx/ClickableShape.h>


void
BarClickMode::ShowToolTip( ClickableShape* dataShape )
{
   if( dataShape != nullptr )
   {
      auto myData = dataShape->GetData();
      m_Panel->GetChart()->SetTooltip( new TextTooltip( m_LastPoint,
         {
            "CMC: " + myData->GetCategoryName(),
            wxString( myData->GetSeriesName() + ": " + std::to_string( (unsigned int)myData->GetSeriesValue() ) ),
            wxString( "Total: " + std::to_string( (unsigned int)myData->GetCategoryTotalOfAllSeries() ) )
         }
      ) );
   }
   else
   {
      m_Panel->GetChart()->SetTooltip( nullptr );
   }
   m_Panel->ChartChanged( nullptr );
}



BEGIN_EVENT_TABLE( vimCMCModule, wxChartPanel )
END_EVENT_TABLE()


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

   for( auto& serie : SerieArray )
   {
      dataset->AddSerie( serie.first, serie.second.data(), serie.second.size() );//WXSIZEOF( values1 ) );
   }


   unsigned int barSize = 360 / (vecCategories.size()+3);
   if( barSize < 14 )
   {
      barSize = 14;
   }

   // Create stacked bar type
   BarType *barType = new StackedBarType( barSize, 0 );

   // Set bar renderer for it, with stacked bar type
   ClickableBarRenderer *renderer = new ClickableBarRenderer( barType );

   int index = 0;
   for( auto& serie : SerieArray )
   {
      auto color = GetColorFromColorID( serie.first );
      renderer->SetBarDraw( index, new ClickableGradientAreaDraw( color.ChangeLightness(75), color,
                                                                  color, wxNORTH,
                                                                  new ClickableCategoryData( dataset ) ) );
      index++;
   }

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
   this->SetMode( new BarClickMode() );
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

wxColour
vimCMCModule::GetColorFromColorID( const wxString & aszColor )
{
   if( aszColor == "Gold" )
   {
      return wxColour( 237, 207, 94 );
   }
   else if( aszColor == "Blue" )
   {
      return wxColour( 91, 94, 247 );
   }
   else if( aszColor == "White" )
   {
      return wxColour( 249, 252, 207 );
   }
   else if( aszColor == "Black" )
   {
      return wxColour( 75, 75, 79 );
   }
   else if( aszColor == "Red" )
   {
      return wxColour( 232, 53, 53 );
   }
   else if( aszColor == "Green" )
   {
      return wxColour( 82, 186, 40 );
   }
   else // Colorless
   {
      return wxColour( 161, 165, 159 );
   }
}
