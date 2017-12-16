#pragma once

template<typename T>
class TryGet
{
public:
   TryGet()
   {
      m_FoundValue = nullptr;
   }

   ~TryGet() 
   {
      if( m_bOwns )
      {
         delete m_FoundValue;
      }
   };

   void Set(T* aptValue, bool own = false)
   {
      m_FoundValue = aptValue;
      m_bOwns = own;
   }

   bool Good() const
   {
      return m_FoundValue != nullptr;
   }

   T* Value()
   {
      return m_FoundValue;
   }

   T* operator ->()
   {
      return Value();
   }

private:
   T* m_FoundValue;

   bool m_bOwns;
};

template<typename T>
class TryGetCopy
{
public:
   TryGetCopy()
   {
      m_FoundValue = nullptr;
   }

   ~TryGetCopy() 
   {

   };

   void Set(T aptValue)
   {
      m_FoundValue = aptValue;
   }

   bool Good() const
   {
      return m_FoundValue != NULL;
   }

   T* Value()
   {
      return &m_FoundValue;
   }

   T* operator ->()
   {
      return Value();
   }

private:
   T m_FoundValue;
};