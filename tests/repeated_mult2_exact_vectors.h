#ifndef OPENFHE_2023_1788_REPEATED_MULT2_EXACT_VECTORS_H
#define OPENFHE_2023_1788_REPEATED_MULT2_EXACT_VECTORS_H

#include <array>
#include <cstdint>

// Literal transcription of SECOND_MULT2_EXACT_VECTORS.json, not binary64.
// Source SHA-256: 6a0dae07b55adf8552272407d4e8885b1e993808d9914fc250b508ecc8d772e6
namespace repeated_mult2_exact_test {
struct RawDyadic { const char* numerator; std::uint32_t denominatorBits; };
struct RawComplex { RawDyadic real; RawDyadic imag; };

inline constexpr std::array<RawComplex, 16> kX{{
    {{"442721857769029238785", 70U}, {"-1", 2U}},  // slot 0
    {{"3", 3U}, {"-1", 2U}},  // slot 1
    {{"-5", 4U}, {"7", 5U}},  // slot 2
    {{"1", 1U}, {"0", 0U}},  // slot 3
    {{"0", 0U}, {"0", 0U}},  // slot 4
    {{"1", 90U}, {"-1", 91U}},  // slot 5
    {{"-7", 4U}, {"-3", 3U}},  // slot 6
    {{"1", 3U}, {"1", 4U}},  // slot 7
    {{"-1", 54U}, {"1", 55U}},  // slot 8
    {{"9", 5U}, {"-5", 6U}},  // slot 9
    {{"-3", 5U}, {"11", 6U}},  // slot 10
    {{"1", 2U}, {"-1", 3U}},  // slot 11
    {{"-1", 6U}, {"0", 0U}},  // slot 12
    {{"0", 0U}, {"1", 5U}},  // slot 13
    {{"15", 6U}, {"-1", 6U}},  // slot 14
    {{"-1", 3U}, {"1", 2U}},  // slot 15
}};

inline constexpr std::array<RawComplex, 16> kY{{
    {{"-11805916207174113034239", 75U}, {"3", 4U}},  // slot 0
    {{"-5", 4U}, {"3", 4U}},  // slot 1
    {{"7", 5U}, {"-1", 3U}},  // slot 2
    {{"-3", 3U}, {"1", 4U}},  // slot 3
    {{"5", 4U}, {"-7", 5U}},  // slot 4
    {{"-1", 20U}, {"1", 22U}},  // slot 5
    {{"1", 2U}, {"-1", 3U}},  // slot 6
    {{"-3", 4U}, {"5", 5U}},  // slot 7
    {{"1", 20U}, {"-1", 21U}},  // slot 8
    {{"-7", 6U}, {"9", 6U}},  // slot 9
    {{"5", 5U}, {"3", 6U}},  // slot 10
    {{"-1", 4U}, {"-1", 2U}},  // slot 11
    {{"1", 3U}, {"1", 4U}},  // slot 12
    {{"-1", 5U}, {"1", 6U}},  // slot 13
    {{"1", 4U}, {"7", 5U}},  // slot 14
    {{"-5", 5U}, {"-3", 4U}},  // slot 15
}};

inline constexpr std::array<RawComplex, 16> kZ{{
    {{"-3136042293543368879289823576440580920573951", 145U}, {"22431240793630814765079", 77U}},  // slot 0
    {{"-9", 7U}, {"19", 7U}},  // slot 1
    {{"-21", 9U}, {"89", 10U}},  // slot 2
    {{"-3", 4U}, {"1", 5U}},  // slot 3
    {{"0", 0U}, {"0", 0U}},  // slot 4
    {{"-7", 113U}, {"3", 112U}},  // slot 5
    {{"-5", 5U}, {"-5", 7U}},  // slot 6
    {{"-17", 9U}, {"1", 7U}},  // slot 7
    {{"-3", 76U}, {"1", 74U}},  // slot 8
    {{"-81", 12U}, {"197", 12U}},  // slot 9
    {{"-93", 12U}, {"23", 10U}},  // slot 10
    {{"-3", 6U}, {"-7", 7U}},  // slot 11
    {{"-1", 9U}, {"-1", 10U}},  // slot 12
    {{"-1", 11U}, {"-1", 10U}},  // slot 13
    {{"37", 11U}, {"103", 11U}},  // slot 14
    {{"17", 8U}, {"-1", 6U}},  // slot 15
}};

inline constexpr std::array<RawComplex, 16> kW{{
    {{"-33996705613950258741109198139716277009593921004588144285776977055325756955196942974975", 290U}, {"-70345319825481558302451952075714562053923623296797289457911857129", 221U}},  // slot 0
    {{"-35", 11U}, {"-171", 13U}},  // slot 1
    {{"-6157", 20U}, {"-1869", 18U}},  // slot 2
    {{"35", 10U}, {"-3", 8U}},  // slot 3
    {{"0", 0U}, {"0", 0U}},  // slot 4
    {{"13", 226U}, {"-21", 224U}},  // slot 5
    {{"375", 14U}, {"25", 11U}},  // slot 6
    {{"273", 18U}, {"-17", 15U}},  // slot 7
    {{"-7", 152U}, {"-3", 149U}},  // slot 8
    {{"-4031", 21U}, {"-15957", 23U}},  // slot 9
    {{"185", 24U}, {"-2139", 21U}},  // slot 10
    {{"-13", 14U}, {"21", 12U}},  // slot 11
    {{"3", 20U}, {"1", 18U}},  // slot 12
    {{"-3", 22U}, {"1", 20U}},  // slot 13
    {{"-1155", 19U}, {"3811", 21U}},  // slot 14
    {{"273", 16U}, {"-17", 13U}},  // slot 15
}};

inline constexpr RawComplex kZDelta{{"-11363194349405083795455", 145U}, {"23", 77U}};
inline constexpr RawComplex kWDelta{{"-18614770304696143351801321175702663870259884894931964105794256895", 290U}, {"-327019521387827965911665040961750299758821353", 221U}};

}  // namespace repeated_mult2_exact_test

#endif
