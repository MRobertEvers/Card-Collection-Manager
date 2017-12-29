#include "CollectionItemTest.h"

#include "../Collection/CollectionObject.h"

using namespace std;

CollectionItemTest::CollectionItemTest()
{
}

CollectionItemTest::~CollectionItemTest()
{
}

bool 
CollectionItemTest::AddCopy_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   int iCopies = testItem.FindCopies(copyParent, All).size();
   auto createdCopy = testItem.AddCopy(copyParent, vecIDs, vecMeta);

   bResult &= createdCopy->GetIdentifyingAttribute("Collar")=="blue";
   bResult &= createdCopy->GetIdentifyingAttribute("Leash")=="chain";

   string szUID = createdCopy->GetUID();
   bResult &= testItem.FindCopy(szUID).Good();

   int iNewCopies = testItem.FindCopies(copyParent, All).size();
   bResult &= iNewCopies == iCopies+1;

   return bResult;
}

// Also tests UID uniqueness
bool 
CollectionItemTest::RemoveCopy_EntireChain_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   auto createdCopy = testItem.AddCopy(copyParent, vecIDs, vecMeta);
   auto createdCopy2 = testItem.AddCopy(copyParent, vecIDs, vecMeta);

   string szUID = createdCopy->GetUID();
   string szUIDTwo = createdCopy2->GetUID();

   bResult &= szUID != szUIDTwo;
   bResult &= testItem.FindCopy(szUID).Good();
   bResult &= testItem.FindCopy(szUIDTwo).Good();

   int iCopies = testItem.FindCopies(copyParent, All).size();
   bResult &= testItem.RemoveCopy(Location("IDTest-1"), szUID );

   int iNewCopies = testItem.FindCopies(copyParent, All).size();

   bResult &= iCopies == iNewCopies+1;

   return bResult;
}

bool 
CollectionItemTest::RemoveCopy_PartialChain_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   auto createdCopy = testItem.AddCopy(copyParent, vecIDs, vecMeta);

   string szUID = createdCopy->GetUID();

   bResult &= testItem.FindCopy(szUID).Good();

   int iCopies = testItem.FindCopies(copyParent, All).size();
   bResult &= testItem.RemoveCopy(Location("IDTest-6"), szUID );
   int iNewCopies = testItem.FindCopies(Location("IDTest-1"), All).size();

   bResult &= iCopies == iNewCopies;
   bResult &= createdCopy->GetParent() == "IDTest-2";

   return bResult;
}

bool 
CollectionItemTest::FindCopies_All_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   testItem.AddCopy( copyParent, vecIDs, vecMeta );
   testItem.AddCopy( copyParent, vecIDs, vecMeta );

   int iCopies = testItem.FindCopies(copyParent, All).size();

   bResult &= iCopies == 2;

   return bResult;
}

bool 
CollectionItemTest::FindCopies_Virtual_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   testItem.AddCopy( copyParent, vecIDs, vecMeta );
   testItem.AddCopy( copyParent, vecIDs, vecMeta );

   auto lstCopies = testItem.FindCopies(copyParent, All);
   auto copy = lstCopies[0];

   // Try to get virtual copies, there should be any.
   int iVirtualCopies = testItem.FindCopies(copyParent, Virtual).size();

   // Make the copy virtual.
   copy->RemoveResident(copyParent, CopyItem::Family);

   // Need to add this back. This will not change the parent from ""
   copy->AddResident(copyParent);

   // Now that one should be a virtual copy, there are some.
   int iNewVirtualCopies = testItem.FindCopies(copyParent, Virtual).size();

   bResult &= iVirtualCopies == 0;
   bResult &= iNewVirtualCopies == 1;

   return bResult;
}

bool 
CollectionItemTest::FindCopies_Borrowed_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   Location borrowerParent("ID2-1");

   // Create 1 copy that is not borrowed and borrowing parent owns.
   testItem.AddCopy( borrowerParent, vecIDs, vecMeta );

   // Create 1 copy that borrowerParent is borrowing
   vecMeta.push_back(make_pair(CopyItem::GetAddressKey(), copyParent.GetFullAddress()));
   testItem.AddCopy( borrowerParent, vecIDs, vecMeta );

   // Try to get virtual copies, there should be any.
   int iBorrowedCopies = testItem.FindCopies(borrowerParent, Borrowed).size();
   int iAllCopies = testItem.FindCopies(borrowerParent, All).size();

   bResult &= iBorrowedCopies == 1;
   bResult &= iAllCopies == 2;

   return bResult;
}

bool 
CollectionItemTest::FindCopies_Local_Test()
{
   bool bResult = true;
   vector<Tag> vecIDs, vecMeta;
   auto testItem = getTestCollectionItem();

   Location copyParent("IDTest-6");
   Location borrowerParent("ID2-1");

   // Create 1 copy that is not borrowed and borrowing parent owns.
   testItem.AddCopy( borrowerParent, vecIDs, vecMeta );

   // Create 1 copy that borrowerParent is borrowing
   vecMeta.push_back(make_pair(CopyItem::GetAddressKey(), copyParent.GetFullAddress()));
   testItem.AddCopy( borrowerParent, vecIDs, vecMeta );

   // Try to get virtual copies, there should be any.
   int iLocalInBorrowCopies = testItem.FindCopies(borrowerParent, Local).size();
   int iLocalCopies = testItem.FindCopies(copyParent, Local).size();

   bResult &= iLocalInBorrowCopies == 1;
   bResult &= iLocalCopies == 1;

   return bResult;
}

CollectionObject
CollectionItemTest::getTestCollectionItem()
{
   // For this, I don't want to be dependent on outside files
   // Create a custom item.
   std::vector<Tag> vecCommonTags;
   vecCommonTags.push_back(make_pair("set", "BFZ"));
   vecCommonTags.push_back(make_pair("height", "5"));
   vecCommonTags.push_back(make_pair("degree", "Good"));

   std::vector<TraitItem> vecRestrictedTraits;
   TraitItem traitOne( "Collar", { "blue", "red" } );
   TraitItem traitTwo( "Leash", { "chain", "rope" } );
   vecRestrictedTraits.push_back(traitOne);
   vecRestrictedTraits.push_back(traitTwo);

   return CollectionObject( "TestItem", vecCommonTags, vecRestrictedTraits);
}