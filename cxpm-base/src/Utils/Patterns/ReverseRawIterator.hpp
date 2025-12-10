#include "RawIterator.hpp"

//-------------------------------------------------------------------
// Raw reverse iterator with random access
//-------------------------------------------------------------------
template <typename DataType>
class ReverseRawIterator : public RawIterator<DataType> {
public:
  ReverseRawIterator(DataType *ptr = nullptr)
      : RawIterator<DataType>(ptr) {}
  ReverseRawIterator(const RawIterator<DataType> &rawIterator) {
    this->m_ptr = rawIterator.getPtr();
  }
  ReverseRawIterator(
      const ReverseRawIterator<DataType> &rawReverseIterator) = default;
  ~ReverseRawIterator() {}

  ReverseRawIterator<DataType> &operator=(
      const ReverseRawIterator<DataType> &rawReverseIterator) = default;
  ReverseRawIterator<DataType> &
  operator=(const RawIterator<DataType> &rawIterator) {
    this->m_ptr = rawIterator.getPtr();
    return (*this);
  }
  ReverseRawIterator<DataType> &operator=(DataType *ptr) {
    this->setPtr(ptr);
    return (*this);
  }

  ReverseRawIterator<DataType> &
  operator+=(const RawIterator<DataType>::difference_type &movement) {
    this->m_ptr -= movement;
    return (*this);
  }
  ReverseRawIterator<DataType> &
  operator-=(const RawIterator<DataType>::difference_type &movement) {
    this->m_ptr += movement;
    return (*this);
  }
  ReverseRawIterator<DataType> &operator++() {
    --this->m_ptr;
    return (*this);
  }
  ReverseRawIterator<DataType> &operator--() {
    ++this->m_ptr;
    return (*this);
  }
  ReverseRawIterator<DataType> operator++(int) {
    auto temp(*this);
    --this->m_ptr;
    return temp;
  }
  ReverseRawIterator<DataType> operator--(int) {
    auto temp(*this);
    ++this->m_ptr;
    return temp;
  }
  ReverseRawIterator<DataType> operator+(const int &movement) {
    auto oldPtr = this->m_ptr;
    this->m_ptr -= movement;
    auto temp(*this);
    this->m_ptr = oldPtr;
    return temp;
  }
  ReverseRawIterator<DataType> operator-(const int &movement) {
    auto oldPtr = this->m_ptr;
    this->m_ptr += movement;
    auto temp(*this);
    this->m_ptr = oldPtr;
    return temp;
  }

  RawIterator<DataType>::difference_type
  operator-(const ReverseRawIterator<DataType> &rawReverseIterator) {
    return std::distance(this->getPtr(), rawReverseIterator.getPtr());
  }

  RawIterator<DataType> base() {
    RawIterator<DataType> forwardIterator(this->m_ptr);
    ++forwardIterator;
    return forwardIterator;
  }
};
//-------------------------------------------------------------------
