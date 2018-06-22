#pragma once
class wxPanel;
class IViewFactory
{
public:
   virtual wxPanel* GetView( wxPanel* aptParent ) = 0;
};