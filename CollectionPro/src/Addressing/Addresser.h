#pragma once
#include<vector>
#include<string>

class Addresser
{
public:
   Addresser();
   ~Addresser();

   int GetPrime(unsigned int aiPrimeIndex) const;
   int GetLowPrimeIndex(unsigned int aiComposite) const;
   int GetLowPrime(unsigned int aiComposite) const;
   int GetHighPrimeIndex(unsigned int aiComposite) const;
   int GetHighPrime(unsigned int aiComposite) const;
   unsigned int PopFactor( unsigned int ) const;

   int GetRandom();
private:
   static const std::vector<int> Primes;
   static unsigned int ms_iRandom;
};

class Address;
class Location;
class Identifier
{
public:
   Identifier();
   Identifier(const std::string& aszId);
   ~Identifier();

   virtual bool IsEmpty() const;
   virtual std::vector<unsigned int> GetSubAddresses() const;
   virtual std::vector<Location> GetLocations() const;

   std::string GetMain() const;
   std::string GetFullAddress() const;
   Location GetBase() const;

   virtual Address ToAddress() const;
   
   bool operator==(const Identifier& rhs) const;
   bool operator!=(const Identifier& rhs) const;
   bool operator<(const Identifier& rhs) const;

protected:
   std::string m_szMain;
   std::vector<unsigned int> m_veciSubAddresses;

   void parseIdName(const std::string& aszID);
   int addSubAddress(std::vector<unsigned int>& avecSAs, unsigned int aiSA);
   bool isSuperSet( unsigned int aiSuper, 
                    unsigned int aiSub ) const;

private:
   int compareSubAddress( unsigned int aiSOne,
                          unsigned int aiSTwo ) const;
};

class Location;
class Address : public Identifier
{
public:
   Address();
   Address(const std::string& aszId);
   ~Address();

   std::vector<unsigned int> GetSubAddresses() const override;

   bool ContainsLocation(const Location& aLoc) const;
   bool AddSubAddress(unsigned int aiSub);
   int RemoveSubAddress(unsigned int aiSub);
   int SetSubAddress(unsigned int aiAlreadySub, unsigned int aiSub);
   bool MergeIdentifier(const Identifier& aID);
   bool ExtractIdentifier(const Identifier& aID);
private:

};

class Location : public Identifier
{
public:
   Location();
   Location(const std::string& aszId);
   Location(const std::string& aszMain, unsigned int aiSA);
   ~Location();

   bool IsSpecifiedBy(const Address& aAddr) const;
   std::vector<unsigned int> GetSubAddresses() const override;
   std::vector<Location> GetLocationsSpecified() const;
   unsigned int GetSubAddress() const;

   Address ToAddress() const;

private:
   unsigned int m_iAddress;
};