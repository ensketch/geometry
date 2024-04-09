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

  basic_flat_face_list() = default;

  auto size() const noexcept -> size_type { return offset.size() - 1; }
  bool empty() const noexcept { return size() == 0; }

  auto operator[](size_type fid) noexcept -> face {
    return {&vertices[offset[fid]], &vertices[offset[fid + 1]]};
  }
  auto operator[](size_type fid) const noexcept -> face {
    return {&vertices[offset[fid]], &vertices[offset[fid + 1]]};
  }

  auto operator[](size_type fid, size_type i) const noexcept -> vertex {
    return vertices[offset[fid] + i];
  }

  // auto begin() const noexcept;
  // auto end() const noexcept;
  // auto begin() noexcept;
  // auto end()  noexcept;

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

  // void assign();
  // void insert();
  // void erase();
  // void swap();

 private:
  vector<vertex> vertices{};
  vector<size_type> offset{0};
};

using flat_face_list = basic_flat_face_list<uint32>;
using large_flat_face_list = basic_flat_face_list<uint64>;

}  // namespace ensketch::geometry
