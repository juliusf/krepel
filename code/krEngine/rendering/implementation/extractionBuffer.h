#pragma once

namespace kr
{
  class ExtractionBuffer
  {
  public: // ** Public Types
    struct Mode
    {
      enum Enum
      {
        ReadOnly,
        WriteOnly,
      };
    };
  public: // *** Construction
    ExtractionBuffer(ezAllocatorBase* pAllocator);
    ~ExtractionBuffer();

  public: // *** Public API

    /// \brief Allocates data of the given type.
    template<typename T>
    T* allocate()
    {
      return (T*)allocate(sizeof(T), EZ_ALIGNMENT_OF(T));
    }

    /// \brief Reset the current allocation pointer.
    /// \note Does not do any (re)allocations.
    void reset() { m_current = m_data; }

    /// \brief Number of bytes currently allocated.
    size_t getNumAllocatedBytes() const { return m_current - m_data; }

    /// \brief Number of bytes currently available without having to grow.
    size_t getByteCapacity() const { return m_end - m_data; }

    void setGrowthAllowed(bool allowed) { m_growthAllowed = allowed; }

    void setMode(Mode::Enum mode) { m_mode = mode; }

  public: // *** Friends & Algorithms
    friend void swap(ExtractionBuffer*& readOnly, ExtractionBuffer*& writeOnly)
    {
      kr::swap(readOnly, writeOnly);
      readOnly->m_mode = Mode::ReadOnly;
      writeOnly->m_mode = Mode::WriteOnly;
    }

  private: // *** Internal Functions
    void* allocate(size_t bytes, size_t alignment);
    void grow(size_t newCapacity);

  private: // *** Internal Data
    ezAllocatorBase* m_pAllocator;
    ezUByte* m_data = nullptr;
    ezUByte* m_current = nullptr;
    ezUByte* m_end = nullptr;
    Mode::Enum m_mode = Mode::WriteOnly;
    bool m_growthAllowed = true;

  private:
    EZ_DISALLOW_COPY_AND_ASSIGN(ExtractionBuffer);
  };
}
