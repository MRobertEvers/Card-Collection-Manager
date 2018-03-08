#include "CURLAPI.h"
#include "curl/curl.h"

FileWriterFunctor::FileWriterFunctor( const std::string& aszFilePath )
   : m_ptOfStream( new std::ofstream )
{
   m_ptOfStream->open( aszFilePath, std::ofstream::binary );
}

FileWriterFunctor::~FileWriterFunctor()
{
   m_ptOfStream->close();
}

size_t 
FileWriterFunctor::Append( void *contents, size_t size, size_t nmemb )
{
   m_ptOfStream->write( (char*)contents, size * nmemb );
   return size * nmemb;
}

void
CURLAPI::Easy_HTTP( const std::string& aszRequest, cURLFunctor* aptBuffer )
{
   CURL *curl;
   CURLcode res;

   curl = curl_easy_init();
   if( curl )
   {
      curl_easy_setopt( curl, CURLOPT_URL, aszRequest.c_str() );
      curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteCallback );
      curl_easy_setopt( curl, CURLOPT_WRITEDATA, aptBuffer );
      res = curl_easy_perform( curl );
      curl_easy_cleanup( curl );
   }
}

void 
CURLAPI::Easy_HTTPS( const std::string& aszRequest, cURLFunctor* aptBuffer )
{
   CURL *curl;
   CURLcode res;

   curl = curl_easy_init();
   if( curl )
   {
      curl_easy_setopt( curl, CURLOPT_URL, aszRequest.c_str() );

      curl_easy_setopt( curl, CURLOPT_SSL_VERIFYPEER, 0L );
      curl_easy_setopt( curl, CURLOPT_SSL_VERIFYHOST, 0L );

      curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, WriteCallback );
      curl_easy_setopt( curl, CURLOPT_WRITEDATA, aptBuffer );
      res = curl_easy_perform( curl );
      curl_easy_cleanup( curl );
   }
}

size_t 
CURLAPI::WriteCallback( void *contents, size_t size, size_t nmemb, void *userp )
{
   return ((cURLFunctor*)userp)->Append( contents, size, nmemb );
}