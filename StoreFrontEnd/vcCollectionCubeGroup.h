#pragma once
#include "wx/wxprec.h"
#include "wx/listctrl.h"
#include "Group.h"

class MainFrame;

class vcCollectionCubeGroup : public wxListView
{
public:
   vcCollectionCubeGroup( MainFrame* aptParent, wxWindowID aiWID, const Group& aGroup );
   ~vcCollectionCubeGroup();

private:
   Group m_Group;
};