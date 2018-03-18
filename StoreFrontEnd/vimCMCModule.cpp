#include "vimCMCModule.h"
#include <map>
#include <wx/bars/barplot.h>

#include <wx/axis/numberaxis.h>
#include <wx/axis/categoryaxis.h>

#include <wx/xy/xyhistorenderer.h>

#include <wx/category/categorysimpledataset.h>

vimCMCModule::vimCMCModule( wxWindow* aptParent,
                            wxWindowID aiID,
                            std::shared_ptr<CollectionInterface> aptInterface )
   : wxPanel(aptParent, aiID), m_ptInterface(aptInterface)
{
   wxGridSizer* sizer = new wxGridSizer( 1, 1, 0, 0 );
   this->SetSizer( sizer );

   //std::map<unsigned int, unsigned int> mapCMCs;
   //unsigned int iTotalCMC = 0;
   //for( auto& item : m_ptInterface->GetItemInterfaces() )
   //{
   //   auto szCMC = item.GetManaCost();
   //   unsigned int iNumChars;
   //   unsigned int iCMC = std::stoul( szCMC, &iNumChars );
   //   if( iNumChars > 0 )
   //   {
   //      iTotalCMC += iCMC;
   //   }
   //}
   //wxStaticText* txt = new wxStaticText( this, 1, "Count: " + std::to_string( m_ptInterface->GetItemCount() ) );
   //sizer->Add( txt, wxSizerFlags( 1 ) );

   this->SetSizeHints( 220, 220 );


   wxString names[] = { // category names
      wxT( "Cat 1" ),
      wxT( "Cat 2" ),
      wxT( "Cat 3" ),
      wxT( "Cat 4" ),
      wxT( "Cat 5" ),
   };

   // serie 1 values
   double values1[] = {
      10,
      20,
      5,
      50,
      25,
   };

   // serie 2 values
   double values2[] = {
      16,
      10,
      15,
      30,
      45,
   };


   // Create dataset
   CategorySimpleDataset *dataset = new CategorySimpleDataset( names, WXSIZEOF( names ) );

   // add two series to it
   dataset->AddSerie( wxT( "Serie 1" ), values1, WXSIZEOF( values1 ) );
   dataset->AddSerie( wxT( "Serie 2" ), values2, WXSIZEOF( values2 ) );

   // Create stacked bar type
   BarType *barType = new StackedBarType( 40, 0 );

   // Set bar renderer for it, with stacked bar type
   BarRenderer *renderer = new BarRenderer( barType );

   // assign renderer to dataset - necessary step
   dataset->SetRenderer( renderer );

   // Create bar plot
   BarPlot *plot = new BarPlot();

   // Add left category axis
   CategoryAxis *leftAxis = new CategoryAxis( AXIS_LEFT );
   leftAxis->SetMargins( 20, 20 );
   plot->AddAxis( leftAxis );

   // Add bottom number axis
   NumberAxis *bottomAxis = new NumberAxis( AXIS_BOTTOM );
   bottomAxis->SetMargins( 0, 5 );
   plot->AddAxis( bottomAxis );

   // Add dataset to plot
   plot->AddDataset( dataset );

   // Link first dataset with first horizontal axis
   plot->LinkDataHorizontalAxis( 0, 0 );

   // Link first dataset with first vertical axis
   plot->LinkDataVerticalAxis( 0, 0 );

   // and finally construct and return chart


   wxChartPanel* wxChart = new wxChartPanel( this, 1, new Chart( plot, GetName() ) );
   sizer->Add( wxChart, wxSizerFlags(1 ).Expand() );
}

vimCMCModule::~vimCMCModule()
{

}