#pragma once
#include <string>
#include <vector>

class CELIOption
{
public:
   std::string Display;

   // It is expected that the metatags of the id are UIDs.
   std::vector<std::pair<std::string, std::string>> IDs;
};