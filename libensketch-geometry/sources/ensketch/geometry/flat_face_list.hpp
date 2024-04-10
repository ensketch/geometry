#pragma once
#include <ensketch/geometry/utility.hpp>

namespace ensketch::geometry {

namespace generic {

template <typename type, typename vertex>
concept face =
    ranges::range<type> && convertible_to<ranges::range_value_t<type>, vertex>;

// inline bool valid(generic::face auto&& face) noexcept {
//   return ranges::size(forward<decltype(face)>(face)) >= 3;
// }

// consteval bool has_three_nodes(generic::face auto&& face) noexcept {
//   return ranges::size(forward<decltype(face)>(face)) == 3;
// }

template <typename type, typename vertex>
concept triangle = face<type, vertex> /*&& (type::size() == 3)*/;

template <typename type, typename vertex>
concept face_list =
    ranges::range<type> && face<ranges::range_value_t<type>, vertex>;

}  // namespace generic

template <unsigned_integral type>
struct basic_flat_face_list {
  using size_type = type;
  using vertex = size_type;
  using face = span<vertex>;
  using const_face = span<const vertex>;

  template <bool is_constant>
  struct basic_iterator;
  using iterator = basic_iterator<false>;
  using const_iterator = basic_iterator<true>;

  basic_flat_face_list() = default;

  auto size() const noexcept -> size_type { return offset.size() - 1; }
  bool empty() const noexcept { return size() == 0; }

  auto operator[](size_type fid) noexcept -> face {
    return {&vertices[offset[fid]], &vertices[offset[fid + 1]]};
  }
  auto operator[](size_type fid) const noexcept -> const_face {
    return {&vertices[offset[fid]], &vertices[offset[fid + 1]]};
  }

  auto operator[](size_type fid, size_type i) noexcept -> vertex& {
    return vertices[offset[fid] + i];
  }
  auto operator[](size_type fid, size_type i) const noexcept -> vertex {
    return vertices[offset[fid] + i];
  }

  auto begin() noexcept -> iterator { return {this, 0}; }
  auto end() noexcept -> iterator { return {this, size()}; }

  auto begin() const noexcept -> const_iterator { return {this, 0}; }
  auto end() const noexcept -> const_iterator { return {this, size()}; }

  void clear() {
    vertices.clear();
    offset.assign(1, 0);
  }

  // error handling for faces with less than 3 points?
  // in 1D even 2 points would make sense...

  template <convertible_to<vertex> index>
  void push_back(initializer_list<index> face) {
    for (auto v : face) vertices.push_back(v);
    offset.push_back(vertices.size());
  }

  void push_back(generic::face<vertex> auto&& face) {
    for (auto v : face) vertices.push_back(v);
    offset.push_back(vertices.size());
  }

  void push_back(generic::face_list<vertex> auto&& faces) {
    for (const auto& face : faces) push_back(face);
  }

  void pop_back() {
    if (empty()) return;
    offset.pop_back();
    vertices.resize(offset.back());
  }

  auto front() noexcept -> face { return operator[](0); }
  auto back() noexcept -> face { return operator[](size() - 1); }

  auto front() const noexcept -> const_face { return operator[](0); }
  auto back() const noexcept -> const_face { return operator[](size() - 1); }

  // void assign();
  // void insert();
  // void erase();
  // void swap();

 private:
  vector<vertex> vertices{};
  vector<size_type> offset{0};
};

template <unsigned_integral type>
template <bool is_constant>
struct basic_flat_face_list<type>::basic_iterator {
  using iterator_category = random_access_iterator_tag;
  using ptr_type = conditional_t<is_constant,
                                 const basic_flat_face_list*,
                                 basic_flat_face_list*>;
  using offset_type = size_type;
  using value_type = conditional_t<is_constant, const_face, face>;
  using reference_type = value_type;
  using difference_type = make_signed_t<size_type>;

  auto base() const noexcept -> basic_flat_face_list& { return *ptr; }

  auto operator*() noexcept -> reference_type {
    return ptr->operator[](offset);
  }
  auto operator*() const noexcept -> reference_type {
    return ptr->operator[](offset);
  }

  auto operator++() noexcept -> basic_iterator& {
    ++offset;
    return *this;
  }

  auto operator++(int) noexcept -> basic_iterator {
    basic_iterator result = *this;
    ++offset;
    return result;
  }

  auto operator--() noexcept -> basic_iterator& {
    --offset;
    return *this;
  }

  auto operator--(int) noexcept -> basic_iterator {
    basic_iterator result = *this;
    --offset;
    return result;
  }

  friend auto operator-(const basic_iterator& it1,
                        const basic_iterator& it2) noexcept -> difference_type {
    return it2.offset - it1.offset;
  }

  friend auto operator+(const basic_iterator& it, difference_type n) noexcept
      -> basic_iterator {
    return {it->ptr, it->offset + n};
  }
  friend auto operator+(difference_type n, const basic_iterator& it) noexcept
      -> basic_iterator {
    return it + n;
  }

  friend auto operator-(const basic_iterator& it, difference_type n) noexcept
      -> basic_iterator {
    return {it->ptr, it->offset - n};
  }
  friend auto operator-(difference_type n, const basic_iterator& it) noexcept
      -> basic_iterator {
    return it - n;
  }

  auto operator+=(difference_type n) noexcept -> basic_iterator& {
    offset += n;
    return *this;
  }

  auto operator-=(difference_type n) noexcept -> basic_iterator& {
    offset -= n;
    return *this;
  }

  auto operator[](difference_type n) const noexcept -> face {
    return ptr->operator[](offset + n);
  }

  friend auto operator<=>(const basic_iterator&,
                          const basic_iterator&) noexcept = default;

  ptr_type ptr{};
  offset_type offset{};
};

using flat_face_list = basic_flat_face_list<uint32>;
using large_flat_face_list = basic_flat_face_list<uint64>;

}  // namespace ensketch::geometry
