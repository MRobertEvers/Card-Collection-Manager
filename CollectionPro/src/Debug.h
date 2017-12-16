#pragma once
#include<string>

class Debug
{
public:
   Debug();
   ~Debug();

public:
   static void Log(std::string aszClass, std::string aszLog);
};

