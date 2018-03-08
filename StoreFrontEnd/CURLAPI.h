#pragma once
#include <string>
#include <memory>
#include <fstream>

class cURLFunctor
{
public:
   cURLFunctor() {}
   virtual ~cURLFunctor() {}
   virtual size_t Append( void *contents, size_t size, size_t nmemb ) = 0;
};

class FileWriterFunctor : public cURLFunctor
{
public:
   FileWriterFunctor( const std::string& aszFilePath );
   ~FileWriterFunctor();
   size_t Append( void *contents, size_t size, size_t nmemb );

private:
   std::unique_ptr<std::ofstream> m_ptOfStream;
};

class CURLAPI
{
public:
   static void Easy_HTTP( const std::string& aszRequest, cURLFunctor* aptBuffer );
   static void Easy_HTTPS( const std::string& aszRequest, cURLFunctor* aptBuffer );

   static size_t WriteCallback( void *contents, size_t size, size_t nmemb, void *userp );
};

