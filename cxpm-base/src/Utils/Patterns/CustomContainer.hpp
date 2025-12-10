#include "RawIterator.hpp"
#include "ReverseRawIterator.hpp"

template <typename DataType> class blCustomContainer {
public: // The typedefs
  typedef RawIterator<DataType> iterator;
  typedef RawIterator<const DataType> const_iterator;

  typedef ReverseRawIterator<DataType> reverse_iterator;
  typedef ReverseRawIterator<const DataType> const_reverse_iterator;

public: // The begin/end functions
  iterator begin() { return iterator(&m_data[0]); }
  iterator end() { return iterator(&m_data[m_size]); }

  const_iterator cbegin() { return const_iterator(&m_data[0]); }
  const_iterator cend() { return const_iterator(&m_data[m_size]); }

  reverse_iterator rbegin() { return reverse_iterator(&m_data[m_size - 1]); }
  reverse_iterator rend() { return reverse_iterator(&m_data[-1]); }

  const_reverse_iterator crbegin() {
    return const_reverse_iterator(&m_data[m_size - 1]);
  }
  const_reverse_iterator crend() { return const_reverse_iterator(&m_data[-1]); }

  // This is the pointer to the
  // beginning of the data
  // This allows the container
  // to either "view" data owned
  // by other containers or to
  // own its own data
  // You would implement a "create"
  // method for owning the data
  // and a "wrap" method for viewing
  // data owned by other containers

  int m_size;
  DataType *m_data;
};