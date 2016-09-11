cc_library(
    name = "common_test_util_lib",
    srcs = [
      "common_test_util.cc",
    ],
    hdrs = [
      "common_test_util.h",
    ],
    deps = [
      "//ydec/mahjong/proto:mahjong-scorecalculator",
    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
      "hand_parser_test.cc",
      "mahjong_common_utils_test.cc",
      "score_calculator_test.cc",
      "yaku_applier_test.cc",
    ],
    data = [
      "//ydec/mahjong/data:rule_pb",
    ],
    deps = [
      ":common_test_util_lib",
      "//ydec/mahjong/src:mahjong_score_calculator_lib",
      "@googletest//:gtest_main",
    ],
)
