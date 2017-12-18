#pragma once
#include <vector>
#include <string>

class CollectionDeltaClass
{
public:
   CollectionDeltaClass();
   ~CollectionDeltaClass();

   std::vector<std::string> Additions;
   std::vector<std::string> Removals;
   std::vector<std::string> Changes;
};

