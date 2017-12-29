#pragma once

// Tests Address and Addresser class.
class AddressTest
{
public:
   AddressTest();
   ~AddressTest();

   bool ParseTestSingle();
   bool ParseTestManySub();

   bool IsResidentInTest();
   bool PitheLocationTest();
   bool InceptLocationTest();
};

