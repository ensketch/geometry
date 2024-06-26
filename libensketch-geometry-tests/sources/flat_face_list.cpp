#include <doctest/doctest.h>
//
#include <ensketch/geometry/flat_face_list.hpp>
//
#include <list>

using namespace ensketch::geometry;

static_assert(ensketch::geometry::generic::face<array<uint32, 3>, uint32>);
static_assert(
    ensketch::geometry::generic::face<initializer_list<uint32>, uint32>);
// static_assert(ensketch::geometry::generic::triangle<array<uint32, 3>, uint32>);

static_assert(std::weakly_incrementable<flat_face_list::iterator>);
static_assert(std::input_or_output_iterator<flat_face_list::iterator>);
static_assert(std::input_iterator<flat_face_list::iterator>);
static_assert(std::forward_iterator<flat_face_list::iterator>);
static_assert(std::bidirectional_iterator<flat_face_list::iterator>);
static_assert(std::random_access_iterator<flat_face_list::iterator>);

static_assert(std::ranges::random_access_range<flat_face_list>);

SCENARIO("flat_face_list") {
  flat_face_list faces{};
  const auto& cfaces = faces;

  const auto check_content = [&] {
    // Standard For Loop and Subscript Operator
    //
    for (size_t fid = 0; fid < size(cfaces); ++fid) {
      for (size_t i = 0; i < size(cfaces[fid]); ++i) {
        CHECK(cfaces[fid][i] == fid + 1 + i);
        CHECK(cfaces[fid, i] == fid + 1 + i);
      }
    }

    // Range-Based For Loop
    //
    for (int fid = 0; auto face : cfaces) {
      for (int i = 0; auto vid : face) {
        CHECK(vid == fid + 1 + i);
        ++i;
      }
      ++fid;
    }
  };

  CHECK(faces.size() == 0);
  CHECK(faces.empty());
  check_content();

  faces.push_back({1, 2, 3});

  CHECK(faces.size() == 1);
  CHECK(!faces.empty());
  CHECK(size(faces.back()) == 3);
  check_content();

  faces.push_back(array<uint32, 4>{2, 3, 4, 5});

  CHECK(faces.size() == 2);
  CHECK(!faces.empty());
  CHECK(size(faces.back()) == 4);
  check_content();

  faces.push_back(vector<uint64>{3, 4, 5, 6, 7});

  CHECK(faces.size() == 3);
  CHECK(!faces.empty());
  CHECK(size(faces.back()) == 5);
  check_content();

  faces.push_back(list<uint16>{4, 5, 6});

  CHECK(faces.size() == 4);
  CHECK(!faces.empty());
  CHECK(size(faces.back()) == 3);
  check_content();

  faces.pop_back();

  CHECK(faces.size() == 3);
  CHECK(!faces.empty());

  faces.pop_back();

  CHECK(faces.size() == 2);
  CHECK(!faces.empty());

  faces.clear();

  CHECK(faces.size() == 0);
  CHECK(faces.empty());

  faces.pop_back();

  CHECK(faces.size() == 0);
  CHECK(faces.empty());
}
