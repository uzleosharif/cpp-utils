

// SPDX-License-Identifier: MIT

export module uzleo_utils;

import std;
import fmt;

namespace rng = std::ranges;

export namespace uzleo::utils {

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
      std::initializer_list<std::uint8_t> init_list);

 private:
  std::array<std::byte, kNumBits / 8> m_data{};

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
           std::views::reverse | std::views::join | rng::to<std::string>();
  }
};

}  // namespace uzleo::utils

namespace uzleo::utils {

template <std::size_t kNumBits, bool kBigEndian>
constexpr UintNBitsType<kNumBits, kBigEndian>::UintNBitsType(
    std::initializer_list<std::uint8_t> init_list) {
  if (rng::size(init_list) > rng::size(m_data)) {
    throw std::invalid_argument{"Not all bytes are provided."};
  }

  rng::transform(init_list, rng::begin(m_data),
                 [](auto const value) { return std::byte{value}; });
}

}  // namespace uzleo::utils
