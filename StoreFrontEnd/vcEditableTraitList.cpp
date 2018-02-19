#include "vcEditableTraitList.h"

vcEditableTraitList::vcEditableTraitList( wxWindow* aptParent,
                                          wxWindowID aiID )
   : wxScrolledWindow(aptParent, aiID)
{
   wxBoxSizer* boxSizer = new wxBoxSizer(wxVERTICAL);
   this->SetSizer(boxSizer);
   //this->FitInside();
}

vcEditableTraitList::~vcEditableTraitList()
{

}