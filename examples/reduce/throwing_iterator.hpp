#pragma once

#include <utility>
#include <type_traits>
#include <iterator>


template<class Iterator>
class throwing_iterator
{
  public:
    template<class OtherIterator,
             class = std::enable_if_t<
               std::is_constructible_v<Iterator,OtherIterator&&>
             >>
    explicit throwing_iterator(OtherIterator&& iterator)
      : iterator_(std::forward<OtherIterator>(iterator))
    {}

    // CopyConstructible requirements
    throwing_iterator(const throwing_iterator& other)
      : iterator_(other.iterator_)
    {}

    // LegacyIterator requirements
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using reference = typename std::iterator_traits<Iterator>::reference;
    using pointer = typename std::iterator_traits<Iterator>::pointer;
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

    throwing_iterator& operator++()
    {
      ++iterator_;
      return *this;
    }

    // LegacyInputIterator requirements
    bool operator!=(const throwing_iterator& other) const
    {
      return iterator_ != other.iterator_;
    }

    reference operator*() const
    {
      throw 13;
      return *iterator_;
    }

    pointer operator->() const
    {
      // XXX this doesn't seem right because it wouldn't work if Iterator is a raw pointer
      return iterator_.operator->();
    }

    throwing_iterator operator++(int)
    {
      throwing_iterator result(*this);
      operator++();
      return result;
    }

    // LegacyBidirectionalIterator requirements
    throwing_iterator& operator--()
    {
      --iterator_;
      return *this;
    }

    throwing_iterator operator--(int)
    {
      throwing_iterator result(*this);
      operator--();
      return result;
    }

    // LegacyRandomAccessIterator requirements
    throwing_iterator& operator+=(const difference_type& n)
    {
      iterator_ += n;
      return *this;
    }

    friend throwing_iterator operator+(const throwing_iterator& i, const difference_type& n)
    {
      return throwing_iterator{i.iterator_ + n};
    }

    friend throwing_iterator operator+(const difference_type& n, const throwing_iterator& i)
    {
      return {n + i.iterator_};
    }

    throwing_iterator& operator-=(const difference_type& n)
    {
      iterator_ -=n;
      return *this;
    }

    throwing_iterator operator-(const difference_type& n) const
    {
      return {iterator_ - n};
    }

    difference_type operator-(const throwing_iterator& rhs) const
    {
      return iterator_ - rhs.iterator_;
    }

    reference operator[](const difference_type& n) const
    {
      throw 13;
      return iterator_[n];
    }

    bool operator<(const throwing_iterator& rhs) const
    {
      return iterator_ < rhs.iterator_;
    }

    bool operator>(const throwing_iterator& rhs) const
    {
      return iterator_ > rhs.iterator_;
    }

    bool operator>=(const throwing_iterator& rhs) const
    {
      return iterator_ >= rhs.iterator_;
    }

    bool operator<=(const throwing_iterator& rhs) const
    {
      return iterator_ <= rhs.iterator_;
    }

  private:
    mutable Iterator iterator_;
};

template<class Iterator>
throwing_iterator<Iterator> make_throwing_iterator(const Iterator& iter)
{
  return throwing_iterator<Iterator>(iter);
}

