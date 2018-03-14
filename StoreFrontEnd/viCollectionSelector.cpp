#include "viCollectionSelector.h"
#include "StoreFrontEnd.h"
#include "vicCollectionPeeker.h"

//wxBEGIN_EVENT_TABLE( viCollectionSelector, wxFrame )
//EVT_SIZE( viCollectionSelector::onResize )
//EVT_GRID_RANGE_SELECT( viCollectionSelector::onSelectRange )
//EVT_GRID_LABEL_LEFT_CLICK( viCollectionSelector::onSelectColumn )
//EVT_GRID_CELL_LEFT_CLICK( viCollectionSelector::onItemSelection )
//wxEND_EVENT_TABLE()

viCollectionSelector::viCollectionSelector( wxPanel* aptParent, wxWindowID aiWID )
   : wxFrame(aptParent, aiWID, "Collection Selector")
{
   wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
   this->SetSizer( boxSizer );

   m_Peeker = new vicCollectionPeeker( this );
   boxSizer->Add( m_Peeker );
   //wxGrid grid = new wxGrid( this, aiWID, wxDefaultPosition, wxDefaultSize, wxBORDER );
   //boxSizer->Add( grid );
}


viCollectionSelector::~viCollectionSelector()
{
}



void
viCollectionSelector::onItemSelection( wxGridEvent& awxEvt )
{

}

void
viCollectionSelector::onResize( wxSizeEvent& awxEvt )
{
   awxEvt.Skip();
}

void
viCollectionSelector::onSelectRange( wxGridRangeSelectEvent& awxEvt )
{
   awxEvt.StopPropagation();
}

void
viCollectionSelector::onSelectColumn( wxGridEvent& awxEvt )
{
   awxEvt.StopPropagation();
}
