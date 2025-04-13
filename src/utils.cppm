

// SPDX-License-Identifier: MIT

export module uzleo.utils;

import std;
import fmt;

namespace rng = std::ranges;

export namespace uzleo::utils {

template <class T>
concept ExplicitlyByteConvertible = requires(T t) {
  { static_cast<std::byte>(t) } -> std::same_as<std::byte>;
};

template <class R>
concept ByteConvertibleRange =
    rng::input_range<R> and ExplicitlyByteConvertible<rng::range_value_t<R>>;

template <class R>
concept ByteConvertibleView =
    ByteConvertibleRange<R> and
    (std::same_as<std::initializer_list<std::uint8_t>, R> or rng::view<R>);

/// This is currently only big-endian.
template <std::size_t kNumBits, bool kBigEndian = true>
class UintNBitsType final {
  static_assert(kBigEndian,
                "Only big-endian big-number datatypes are supported so far.");

 public:
  constexpr UintNBitsType() = default;
  /// pass in bytes directly from MSB to LSB
  /// e.g. auto value = UintNBitsType<32, true>{0x1, 0x2, 0x3, 0x4};
  /// will be a 32 bit number: 0x01020304
  constexpr explicit UintNBitsType(
      std::initializer_list<std::uint8_t> init_list) {
    FillData(init_list);
  }

  constexpr explicit UintNBitsType(std::span<std::byte const> data_bytes) {
    FillData(data_bytes);
  }

 private:
  constexpr auto FillData(ByteConvertibleView auto data_range) {
    if (rng::size(data_range) > rng::size(m_data)) {
      throw std::invalid_argument{"Not all bytes are provided."};
    }
    rng::transform(data_range, rng::begin(m_data),
                   [](auto const value) { return std::byte{value}; });
  }

  friend constexpr auto operator+(UintNBitsType const& A,
                                  UintNBitsType const& B) -> UintNBitsType {
    UintNBitsType<kNumBits, kBigEndian> result{};
    bool carry{false};
    rng::transform(std::views::zip(A.m_data, B.m_data),
                   rng::begin(result.m_data), [&carry](auto const& tup) {
                     auto const& [a, b] = tup;
                     auto result{std::to_integer<unsigned>(a) +
                                 std::to_integer<unsigned>(b) +
                                 static_cast<unsigned>(carry)};
                     carry = (result > 255);
                     return std::byte{static_cast<std::uint8_t>(result % 255)};
                   });
    return result;
  }

  friend auto format_as(UintNBitsType<kNumBits, kBigEndian> const& value)
      -> std::string {
    return value.m_data | std::views::transform([](auto const value_byte) {
             return fmt::format("{:02}", value_byte);
           }) |
           std::views::join | rng::to<std::string>();
  }

  std::array<std::byte, kNumBits / 8> m_data{};
};

template <class T>
class StackType final {
 public:
  constexpr auto Pop(std::size_t const num_of_elements = 1) {
    m_data.erase(rng::prev(rng::cend(m_data), num_of_elements),
                 rng::cend(m_data));
  }

  constexpr auto Push(T const& value) { m_data.push_back(value); }

  constexpr auto GetElementsSpan(std::size_t const num_of_elements = 1) const {
    if (num_of_elements > rng::size(m_data)) {
      throw std::invalid_argument{
          "more elements requested from stack than its size."};
    }

    return std::span<T const>(rng::prev(rng::cend(m_data), num_of_elements),
                              rng::cend(m_data));
  }

  constexpr auto SetElement(T const& value, std::size_t nth_element = 1) {
    m_data.at(rng::size(m_data) - nth_element) = value;
  };

  constexpr auto Print() const {
    fmt::println("---");
    for (auto const& value : m_data | std::views::reverse) {
      fmt::println("{}", value);
    }
    fmt::println("---");
  }

 private:
  std::vector<T> m_data{};
};

}  // namespace uzleo::utils

namespace {

[[maybe_unused]] auto test() {
  using uint256_t = uzleo::utils::UintNBitsType<16>;
  uint256_t value{0x1, 0x2};
  fmt::println("{}", value);

  uzleo::utils::StackType<int> stack{};
}

}  // namespace
