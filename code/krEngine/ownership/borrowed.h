#pragma once
#include <krEngine/ownership/owned.h>

namespace kr
{
  template<typename T>
  struct Borrowed
  {
    using ElementType = NonRef<T>;
    using Data        = OwnershipData<T>;
    using DataToConst = OwnershipData<const T>;
    using Ptr         = ElementType*;
    using PtrToConst  = const ElementType*;
    using Ref         = ElementType&;
    using RefToConst  = const ElementType&;


    Borrowed() = default;

    Borrowed(const Borrowed& other) : pData(other.pData)
    {
      this->addRefIfNotNull();
    }

    template<typename U>
    Borrowed(const Borrowed<U>& other) : pData(other.pData)
    {
      this->addRefIfNotNull();
    }

    Borrowed(const Data& data) : pData(&data)
    {
      this->validateData();
      addRef(*this->pData);
    }

    ~Borrowed()
    {
      this->removeRefIfNotNull();
      this->pData = nullptr;
    }

    void operator =(const Borrowed& other)
    {
      *this = *other.pData;
    }

    template<typename U>
    void operator =(const Borrowed<U>& other)
    {
      *this = *other.pData;
    }

    void operator =(const Data& newData)
    {
      addRef(newData);
      this->removeRefIfNotNull();
      this->pData = &newData;
    }

    Ref operator *()
    {
      this->validate();
      return *this->pData->ptr;
    }

    RefToConst operator *() const
    {
      this->validate();
      return *this->pData->ptr;
    }

    /// Get the actual object.
    Ptr operator ->()
    {
      this->validate();
      return this->pData->ptr;
    }

    PtrToConst operator ->() const
    {
      this->validate();
      return this->pData->ptr;
    }

    operator Ptr()
    {
      this->validateData();
      return this->pData->ptr;
    }

    operator PtrToConst() const
    {
      this->validateData();
      return this->pData->ptr;
    }

    operator Borrowed<const ElementType>() const
    {
      auto& data = reinterpret_cast<const OwnershipData<const ElementType>&>(*this->pData);
      return Borrowed<const ElementType>(data);
    }

    bool operator ==(std::nullptr_t) const { return this->pData == nullptr || this->pData->ptr == nullptr; }
    bool operator !=(std::nullptr_t) const { return !(*this == nullptr); }
    operator bool() const { return *this != nullptr; }

    void addRefIfNotNull()
    {
      if (*this != nullptr)
      {
        this->validateData();
        addRef(*this->pData);
      }
    }

    void removeRefIfNotNull()
    {
      if (*this != nullptr)
      {
        this->validateData();
        removeRef(*this->pData);
      }
    }

  public: // *** Data
    const Data* pData = nullptr;

  private: // *** Internal
    inline void validateData() const
    {
      EZ_ASSERT_ALWAYS(this->pData != nullptr, "Invalid state: Internal data ptr is null.");
    }

    inline void validate() const
    {
      this->validateData();
      EZ_ASSERT_ALWAYS(this->pData->ptr != nullptr, "Validation failed: we have a nullptr.");
    }
  };

  template<typename T>
  Borrowed<T> borrow(Owned<T>& owned)
  {
    Borrowed<T> borrowed;
    borrowed = owned.data;
    return move(borrowed);
  }

  template<typename T>
  Borrowed<const T> borrow(const Owned<T>& owned)
  {
    Borrowed<const T> borrowed;
    // Add const to the ownership data.
    borrowed = reinterpret_cast<const OwnershipData<const T>&>(owned.data);
    return move(borrowed);
  }
}
