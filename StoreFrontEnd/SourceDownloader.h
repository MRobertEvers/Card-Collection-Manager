#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <fstream>

class SourceDownloader
{
public:
   SourceDownloader();
   ~SourceDownloader();

   void FetchMTGJson();
   int UnzipMTGJson();

private:
   static const char* API_URL_BASE;
};

