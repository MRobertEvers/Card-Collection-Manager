#include "vimTypeBreakDown.h"
#include "../StoreFrontEnd/Group.h"

#include <vector>
#include <wx/chart.h>
#include <wx/chartpanel.h>
#include <wx/pie/pieplot.h>
#include <wx/category/categorysimpledataset.h>

vimTypeBreakDown::vimTypeBreakDown( wxWindow* aptParent,
                                    wxWindowID aiID,
                                    std::shared_ptr<CollectionInterface> aptInterface )
   : wxPanel( aptParent, aiID ), m_ptInterface( aptInterface )
{
   wxGridSizer* sizer = new wxGridSizer( 1, 1, 0, 0 );
   this->SetSizer( sizer );

   Group defaultGrp;
   defaultGrp.GroupOn( "type", false )
             .BroadenGroup( "Creature" )
             .BroadenGroup( "Instant" )
             .BroadenGroup( "Sorcery" )
             .BroadenGroup( "Enchantment" )
             .BroadenGroup( "Artifact" )
             .BroadenGroup( "Planeswalker" )
             .BroadenGroup( "Land" );

   std::map<wxString, unsigned int> mapGroups;
   unsigned int iTotalCMC = 0;
   for( auto& item : m_ptInterface->GetItemInterfaces() )
   {
      wxString szGroup = defaultGrp.GetGroup( item );
      auto iter_grp = mapGroups.find( szGroup );
      if( iter_grp != mapGroups.end() )
      {
         mapGroups[szGroup]++;
      }
      else
      {
         mapGroups[szGroup] = 1;
      }
   }

   std::vector<wxString> vecCategories;
   std::vector<double> vecValues1;
   std::vector<wxColour> vecColours;
   for( auto& cat : mapGroups )
   {
      vecCategories.push_back( cat.first );
      vecValues1.push_back( cat.second );
      vecColours.push_back( GetColorForGroup( cat.first ) );
   }
   //wxStaticText* txt = new wxStaticText( this, 1, "Count: " + std::to_string( m_ptInterface->GetItemCount() ) );
   //sizer->Add( txt, wxSizerFlags( 1 ) );

   this->SetSizeHints( 360, 360 );

   // serie pieplot data
   //double data[] = { 1.0, 2.0, 3.0 };
   //wxString categories[] = { _( "cat 1" ), _( "cat 2" ), _( "cat 3" ) };
   //wxColour colours[] = { wxColour( 0x99, 0xCC, 0xFF ), wxColour( 0xFF, 0xFF, 0x99 ), wxColour( 0x3D, 0xEB, 0x3D ) };

   ColorScheme* colorScheme = new ColorScheme( vecColours.data(), vecColours.size() );

   // first step: create plot
   PiePlot *plot = new PiePlot();

   // create dataset
   CategorySimpleDataset* dataset = new CategorySimpleDataset( vecCategories.data(), vecCategories.size() );

   // and add serie to it
   dataset->AddSerie( wxT( "Serie 1" ), vecValues1.data(), vecValues1.size() );

   // create category renderer for legend drawing
   dataset->SetRenderer( new CategoryRenderer( *colorScheme ) );

   // set color scheme
   plot->SetColorScheme( colorScheme );

   plot->SetDataset( dataset );

   // set legend to plot
   plot->SetLegend( new Legend( wxCENTER, wxRIGHT ) );

   // and finally create chart
   wxChartPanel* wxChart = new wxChartPanel( this, 1, new Chart( plot, "Type Breakdown" ) );
   sizer->Add( wxChart, wxSizerFlags( 1 ).Expand() );
}


vimTypeBreakDown::~vimTypeBreakDown()
{
}

wxColour 
vimTypeBreakDown::GetColorForGroup( const wxString& aszGroup )
{
   if( aszGroup == "Land" )
   {
      return wxColour( 241, 92, 128 );
   }
   else if( aszGroup == "Instant" )
   {
      return wxColour( 124, 181, 236 );
   }
   else if( aszGroup == "Sorcery" )
   {
      return wxColour( 67, 67, 72 );
   }
   else if( aszGroup == "Enchantment" )
   {
      return wxColour( 144, 237, 125 );
   }
   else if( aszGroup == "Creature" )
   {
      return wxColour( 128, 133, 233 );
   }
   else if( aszGroup == "Planeswalker" )
   {
      return wxColour( 248, 177, 117 );
   }
   else if( aszGroup == "Artifact" )
   {
      return wxColour( 228, 211, 84 );
   }

   return wxColour();
}