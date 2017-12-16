#include "..\stdafx.h"

#include "../rapidxml-1.13\rapidxml_print.hpp"
#include "../rapidxml-1.13\rapidxml.hpp"
#include "../rapidxml-1.13\rapidxml_utils.hpp"
#include "CollectionSource.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace rapidxml;

CollectionSource::CollectionSource() {

}

CollectionSource::~CollectionSource() {

}

void 
CollectionSource::LoadLib(string aszFilePath) {

   // Verify the file is good.
   std::ifstream file(aszFilePath);
   if (!file.good()) {
      // TODO Close collection.
      return Debug::Log( "CollectionSource",
                         "Could not load library from " + aszFilePath );
   }

   stringstream buffer;
   buffer << file.rdbuf();
   file.close();

   // Prepare the document to be read.
   xml_document<> doc;
   string* textContent = new std::string(buffer.str());
   doc.parse<0>(&textContent->front());


}