#pragma once
#include "CURLAPI.h"
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
   class SourceDownloaderFunctor : public cURLFunctor
   {
   public:
      SourceDownloaderFunctor();
      ~SourceDownloaderFunctor();
      size_t Append( void *contents, size_t size, size_t nmemb );

   private:
      std::unique_ptr<std::ofstream> m_ptOfStream;
   };

   class SourceDecompressorFunctor
   {
   public:
      SourceDecompressorFunctor();
      ~SourceDecompressorFunctor();
      size_t Append( const void* pBuf, int len );

   private:
      std::unique_ptr<std::ofstream> m_ptOfStream;
   };
private:
   static const char* API_URL_BASE;
   static int DecompressionCallback( const void* pBuf, int len, void *pUser );
};

