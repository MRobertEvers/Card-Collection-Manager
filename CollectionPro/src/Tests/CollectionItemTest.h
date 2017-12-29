#pragma once

class CollectionObject;

class CollectionItemTest
{
public:
   CollectionItemTest();
   ~CollectionItemTest();

   bool AddCopy_Test();
   bool RemoveCopy_EntireChain_Test();
   bool RemoveCopy_PartialChain_Test();
   bool FindCopies_All_Test();
   bool FindCopies_Virtual_Test();
   bool FindCopies_Borrowed_Test();
   bool FindCopies_Local_Test();

private:
   CollectionObject getTestCollectionItem();
};

