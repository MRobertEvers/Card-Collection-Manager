#pragma once
class wxWindow;
class IViewFactory
{
public:
   virtual wxWindow* GetView( wxWindow* aptParent ) = 0;
};