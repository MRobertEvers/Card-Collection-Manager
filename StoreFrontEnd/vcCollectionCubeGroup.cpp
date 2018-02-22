#include "vcCollectionCubeGroup.h"
#include "MainWindow.h"
#include "wx/wxprec.h"

vcCollectionCubeGroup::vcCollectionCubeGroup( MainFrame* aptParent, 
                                              wxWindowID aiWID, 
                                              const Group& aGroup )
   : wxListView(aptParent, aiWID), m_Group(aGroup)
{

}

vcCollectionCubeGroup::~vcCollectionCubeGroup()
{

}