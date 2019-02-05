#pragma once

#include <utility>
#include <type_traits>
#include <iterator>


namespace impl
{


template<class Iterator>
class noexcept_iterator
{
  public:
    template<class OtherIterator,
             class = std::enable_if_t<
               std::is_constructible_v<Iterator,OtherIterator&&>
             >>
    explicit noexcept_iterator(OtherIterator&& iterator) noexcept
      : iterator_(std::forward<OtherIterator>(iterator))
    {}

    // CopyConstructible requirements
    noexcept_iterator(const noexcept_iterator& other) noexcept
      : iterator_(other.iterator_)
    {}

    // CopyAssignable requirements
    noexcept_iterator& operator=(const noexcept_iterator& other) noexcept
    {
      iterator_ = other.iterator_;
      return *this;
    }

    // Destructible requirements
    ~noexcept_iterator() noexcept
    {}

    // Swappable requirements
    void swap(const noexcept_iterator& other) noexcept
    {
      using std::swap;
      swap(iterator_, other.iterator_);
    }

    // LegacyIterator requirements
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using reference = typename std::iterator_traits<Iterator>::reference;
    using pointer = typename std::iterator_traits<Iterator>::pointer;
    using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;

    noexcept_iterator& operator++() noexcept
    {
      ++iterator_;
      return *this;
    }

    // LegacyInputIterator requirements
    bool operator!=(const noexcept_iterator& other) const noexcept
    {
      return iterator_ != other.iterator_;
    }

    reference operator*() const noexcept
    {
      return *iterator_;
    }

    pointer operator->() const noexcept
    {
      // XXX this doesn't seem right because it wouldn't work if Iterator is a raw pointer
      return iterator_.operator->();
    }

    noexcept_iterator operator++(int) noexcept
    {
      noexcept_iterator result(*this);
      operator++();
      return result;
    }

    // LegacyBidirectionalIterator requirements
    noexcept_iterator& operator--() noexcept
    {
      --iterator_;
      return *this;
    }

    noexcept_iterator operator--(int) noexcept
    {
      noexcept_iterator result(*this);
      operator--();
      return result;
    }

    // LegacyRandomAccessIterator requirements
    noexcept_iterator& operator+=(const difference_type& n) noexcept
    {
      iterator_ += n;
      return *this;
    }

    friend noexcept_iterator operator+(const noexcept_iterator& i, const difference_type& n) noexcept
    {
      return noexcept_iterator{i.iterator_ + n};
    }

    friend noexcept_iterator operator+(const difference_type& n, const noexcept_iterator& i) noexcept
    {
      return noexcept_iterator{n + i.iterator_};
    }

    noexcept_iterator& operator-=(const difference_type& n) noexcept
    {
      iterator_ -=n;
      return *this;
    }

    noexcept_iterator operator-(const difference_type& n) const noexcept
    {
      return {iterator_ - n};
    }

    difference_type operator-(const noexcept_iterator& rhs) const noexcept
    {
      return iterator_ - rhs.iterator_;
    }

    reference operator[](const difference_type& n) const noexcept
    {
      return iterator_[n];
    }

    bool operator<(const noexcept_iterator& rhs) const noexcept
    {
      return iterator_ < rhs.iterator_;
    }

    bool operator>(const noexcept_iterator& rhs) const noexcept
    {
      return iterator_ > rhs.iterator_;
    }

    bool operator>=(const noexcept_iterator& rhs) const noexcept
    {
      return iterator_ >= rhs.iterator_;
    }

    bool operator<=(const noexcept_iterator& rhs) const noexcept
    {
      return iterator_ <= rhs.iterator_;
    }

  private:
    mutable Iterator iterator_;
};


template<class Iterator>
noexcept_iterator<std::decay_t<Iterator>> make_noexcept_iterator(Iterator&& iter)
{
  return noexcept_iterator<std::decay_t<Iterator>>{std::forward<Iterator>(iter)};
}


} // end impl

