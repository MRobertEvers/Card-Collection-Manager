#include "CollectionTest.h"
#include "../Collection/Collection.h"
#include "../Config.h"
#include "../Support/TypeDefs.h"
#include "../Collection/CollectionSource.h"
#include "../rapidxml-1.13\rapidxml_print.hpp"
#include "../rapidxml-1.13\rapidxml.hpp"
#include "../rapidxml-1.13\rapidxml_utils.hpp"

#include <cstdio>
#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;

CollectionTest::CollectionTest()
{
   Config::SetTestMode(true);
   // This requires at least 3 identifying attributes and 1 static attr.
   writeTestColSourceFile();

   m_ptColSource = new CollectionSource();
   m_ptColSource->LoadLib(Config::Instance()->GetSourceFile());
}


CollectionTest::~CollectionTest()
{
   deleteTestColSourceFile();
   Config::SetTestMode(false);
   delete m_ptColSource;
}


bool 
CollectionTest::AddItem_Test()
{
   // Verifies that AddItem loads a card from the col Source
   // buffer correctly.
   bool bRetval = true;
   auto col = getTestCollection();
   auto szCardName = *cardName(0);
   col->AddItem(szCardName, vector<Tag>(), vector<Tag>() );

   auto addedItem = m_ptColSource->GetCardPrototype(szCardName);
   auto lstCopies = addedItem->FindCopies(col->GetIdentifier(), All);

   bRetval &= lstCopies.size() == 1;

   return bRetval;
}

bool 
CollectionTest::RemoveItem_Test()
{
   // Verifies that Remove Item removes an item from the collection
   // Verifies that when no other collections reference an item, the item
   // is erased.
   // buffer correctly.
   bool bRetval = true;
   auto col = getTestCollection();
   auto szCardName = *cardName(0);
   col->AddItem(szCardName, vector<Tag>(), vector<Tag>() );

   auto addedItem = m_ptColSource->GetCardPrototype(szCardName);
   auto lstCopies = addedItem->FindCopies(col->GetIdentifier(), All);

   unsigned int iCopies = lstCopies.size();
   auto copy = lstCopies[0];

   col->RemoveItem(szCardName, copy->GetUID());

   lstCopies = addedItem->FindCopies(col->GetIdentifier(), All);

   bRetval &= lstCopies.size() < iCopies;

   return bRetval;
}

bool 
CollectionTest::RemoveItem_OtherCollectionsRef_Test()
{
   // Verifies that when other collections reference an item, the item
   // is not erased. Also verifies that the copy becomes virtual
   bool bRetval = true;
   auto col = getTestCollection();
   auto szCardName = *cardName(0);

   // Add the item to the test collection
   col->AddItem(szCardName, vector<Tag>(), vector<Tag>() );
   auto addedItem = m_ptColSource->GetCardPrototype(szCardName);
   auto addedCopy = addedItem->FindCopies(col->GetIdentifier(), All)[0];

   auto colBorrower = getTestCollection("TestID2", false);
   //colBorrower->AddItemFrom(szCardName, addedCopy->GetUID(), col->GetIdentifier());
   
   // Borrow from the test collection.
   auto lstBorrowedCopies = m_ptColSource->GetCollection(colBorrower->GetIdentifier(), All);
   auto iBorrowedCopiesBeforeRemove = lstBorrowedCopies.size();

   // Now remove from the test collection
   col->RemoveItem(szCardName, addedCopy->GetUID());

   lstBorrowedCopies = m_ptColSource->GetCollection(colBorrower->GetIdentifier(), All);
   auto iBorrowedCopiesAfter = lstBorrowedCopies.size();

   bRetval &= iBorrowedCopiesAfter == iBorrowedCopiesBeforeRemove;
   bRetval &= addedCopy->IsVirtual();

   return bRetval;
}

bool 
CollectionTest::ChangeItem_Test()
{
   return false;
}

bool 
CollectionTest::AddItemFrom_Test()
{
   // Verifies that additemfrom correctly adds an existing item.
   bool bRetval = true;
   auto col = getTestCollection();
   auto szCardName = *cardName(0);

   // Add the item to the test collection
   col->AddItem(szCardName, vector<Tag>(), vector<Tag>() );
   auto addedItem = m_ptColSource->GetCardPrototype(szCardName);
   auto addedCopy = addedItem->FindCopies(col->GetIdentifier(), All)[0];

   auto colBorrower = getTestCollection("TestID2", false);
   //colBorrower->AddItemFrom(szCardName, addedCopy->GetUID(), col->GetIdentifier());
   
   auto lstBorrowedCopies = m_ptColSource->GetCollection(colBorrower->GetIdentifier(), All);
   auto addedFromCopy = lstBorrowedCopies[0];

   bRetval &= lstBorrowedCopies.size() == 1;
   bRetval &= addedFromCopy.get() == addedCopy.get();

   return bRetval;
}

bool 
CollectionTest::ReplaceItem_Test()
{
   return false;
}

bool 
CollectionTest::LoadCollection_Test()
{
   return false;
}

bool 
CollectionTest::SaveCollection_Test()
{
   return false;
}

void 
CollectionTest::writeTestColSourceFile()
{
   Config* config = Config::Instance();
   rapidxml::xml_document<>* xmlCardDoc = new rapidxml::xml_document<>;

   rapidxml::xml_node<>* decl = xmlCardDoc->allocate_node(rapidxml::node_declaration);
   decl->append_attribute(xmlCardDoc->allocate_attribute("version", "1.0"));
   decl->append_attribute(xmlCardDoc->allocate_attribute("encoding", "utf-8"));
   xmlCardDoc->append_node(decl);

   // Add the outermose element.
   rapidxml::xml_node<>* xmlNode_root = xmlCardDoc->
      allocate_node(rapidxml::node_element, "Card_Database");
   xmlCardDoc->append_node(xmlNode_root);

   // Create the 'Cards' element.
   rapidxml::xml_node<>* xmlNode_Cards = xmlCardDoc->
      allocate_node(rapidxml::node_element, "cards");
   xmlNode_root->append_node(xmlNode_Cards);

   // Add a test card
   rapidxml::xml_node<>* xmlNode_Card = xmlCardDoc->
      allocate_node(rapidxml::node_element, "card");
   rapidxml::xml_node<>* xmlNode_attr;

   xmlNode_attr= xmlCardDoc->allocate_node( rapidxml::node_element,
                                            config->GetStaticAttributes()[0].c_str(),
                                            cardName(0)->c_str() );
   xmlNode_Card->append_node(xmlNode_attr);
   
   xmlNode_attr= xmlCardDoc->allocate_node( rapidxml::node_element, 
                                            config->GetIdentifyingAttributes()[0].c_str(),
                                            getAttr(0)->c_str() );
   xmlNode_Card->append_node(xmlNode_attr);

   xmlNode_attr= xmlCardDoc->allocate_node( rapidxml::node_element, 
                                            config->GetIdentifyingAttributes()[1].c_str(),
                                            getAttr(1)->c_str() );
   xmlNode_Card->append_node(xmlNode_attr);

   xmlNode_attr= xmlCardDoc->allocate_node( rapidxml::node_element, 
                                            config->GetIdentifyingAttributes()[2].c_str(),
                                            getAttr(2)->c_str() );
   xmlNode_Card->append_node(xmlNode_attr);

   xmlNode_Cards->append_node(xmlNode_Card);

   // Save to file
   // Expects the config folder to already exist.
   // Create the test folder
   auto srcFile = Config::Instance()->GetSourceFolder();
   CreateDirectory(srcFile.c_str(), NULL);

   // Get the source file name.
   srcFile = Config::Instance()->GetSourceFile();
   std::fstream file_stored(srcFile.c_str(), ios::out);
   file_stored << *xmlCardDoc;
   file_stored.close();
   xmlCardDoc->clear();
   delete xmlCardDoc;
}

void
CollectionTest::deleteTestColSourceFile()
{
   std::remove( Config::Instance()->GetSourceFile().c_str() );
}

std::shared_ptr<Collection> 
CollectionTest::getTestCollection(string aszId, bool abClearCache)
{
   if( abClearCache )
   {
      m_ptColSource->ClearCache();
   }

   return std::shared_ptr<Collection>(new Collection("TestCol", m_ptColSource, aszId));
}

const std::string*
CollectionTest::cardName( int iName )
{
   static const std::vector<std::string> lstNames( { "Card One" } );
   return &lstNames[iName];
}

const std::string*
CollectionTest::getAttr( int iAttr )
{
   static const std::vector<std::string> lstIdAttrs( { "BFZ::ABC", "1::2", "cat::dog::mouse" } );
   return &lstIdAttrs[iAttr];
}