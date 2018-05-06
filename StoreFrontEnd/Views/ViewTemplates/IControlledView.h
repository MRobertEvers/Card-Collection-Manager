#pragma once
#include <memory>

class IMenuEventHandler;
class wxPanel;

class IControlledView
{
public:
   virtual std::shared_ptr<IMenuEventHandler> GetEventHandler() = 0;
   virtual wxPanel* GetView() = 0;
};

