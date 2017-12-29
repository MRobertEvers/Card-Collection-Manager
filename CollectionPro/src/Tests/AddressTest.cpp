#include "AddressTest.h"

#include<string>
#include "../Addressing/Addresser.h"

using namespace std;

AddressTest::AddressTest()
{
}


AddressTest::~AddressTest()
{
}

bool 
AddressTest::ParseTestSingle()
{
   // Verifies an Id name is parsed correct and
   // that the default subaddress is 1.
   bool bResult = true;
   string szTestParse = "IDTest";

   Address parseAddr(szTestParse);

   bResult &= parseAddr.GetMain() == szTestParse;

   auto vecSubAddresses = parseAddr.GetAddresses();
   bResult &= vecSubAddresses.size() == 1 && vecSubAddresses[0] == 1;

   return bResult;
}

bool 
AddressTest::ParseTestManySub()
{
   // Verifies that a subaddress can be parsed
   // correctly when multiple subaddresses are present.
   bool bResult = true;
   string szTestParse = "IDTest-2,15";

   Address parseAddr(szTestParse);

   bResult &= parseAddr.GetMain() == "IDTest";

   auto vecSubAddresses = parseAddr.GetAddresses();
   bResult &= vecSubAddresses.size() == 2 && 
              vecSubAddresses[0] == 2 &&
              vecSubAddresses[1] == 15;

   return bResult;
}

bool 
AddressTest::IsResidentInTest()
{
   // Tests that IsResident in
   // returns the subaddress that
   // identify the location.

   bool bResult = true;
   string szParentAddress = "IDTest-3";
   string szChildAddress = "IDTest-2,9,15";

   // Parent would be a collection.
   Location parentLocation(szParentAddress);
   Address childAddr(szChildAddress);

   bResult &= parentLocation.IsSpecifiedBy(childAddr);

   return bResult;
}

bool 
AddressTest::PitheLocationTest()
{
   // Verifies that extracting a location from
   // an address works.
   bool bResult = true;

   string szLocation = "IDTest-3";
   string szAddress = "IDTest-2,9,15";

   Location locationAddr(szLocation);
   Address addressAddr(szAddress);
   Address dummy;

   // We expect to pull out 9 and 15.
   bResult &= addressAddr.ExtractIdentifier(locationAddr);

   auto vecSubAddresses = addressAddr.GetAddresses();
   bResult &= vecSubAddresses.size() == 1 &&
              vecSubAddresses[0] == 2;

   szLocation = "IDTest-5";
   szAddress = "IDTest-2,9,15";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   // Nothing should be pulled out.
   bResult &= !addressAddr.ExtractIdentifier(locationAddr);

   vecSubAddresses = addressAddr.GetAddresses();
   bResult &= vecSubAddresses.size() == 3 &&
              vecSubAddresses[0] == 2 &&
              vecSubAddresses[1] == 9 &&
              vecSubAddresses[2] == 15;

   // Tests that subaddresses that collide after
   // pithing are combined.
   szLocation = "IDTest-9";
   szAddress = "IDTest-2,27,45";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   bResult &= addressAddr.ExtractIdentifier(locationAddr);
   
   vecSubAddresses = addressAddr.GetAddresses();
   bResult &= vecSubAddresses.size() == 2 &&
              vecSubAddresses[0] == 2 &&
              vecSubAddresses[1] == 3;

   return bResult;
}

bool 
AddressTest::InceptLocationTest()
{
   // Verifies behavior of adding a loction
   // in an address.
   bool bResult = true;

   Addresser addr;

   string szLocation = "IDTest-9";
   string szAddress = "IDTest-2";

   Location locationAddr(szLocation);
   Address addressAddr(szAddress);
   Address dummy;

   // We expect to pull out 9 and 15.
   bResult &= addressAddr.MergeIdentifier(locationAddr);

   auto vecSubAddresses = addressAddr.GetAddresses();
   bResult &= vecSubAddresses.size() == 2 &&
              vecSubAddresses[0] == 2 &&
              vecSubAddresses[1] == 9;

   szLocation = "IDTest-27";
   szAddress = "IDTest-2,9,15";

   locationAddr = (szLocation);
   addressAddr = (szAddress);

   // Nothing should be pulled out.
   bResult &= addressAddr.MergeIdentifier(locationAddr);

   vecSubAddresses = addressAddr.GetAddresses();
   bResult &= vecSubAddresses.size() == 3 &&
              vecSubAddresses[0] == 2  &&
              vecSubAddresses[1] == 27 &&
              vecSubAddresses[2] == 15;

   return bResult;
}