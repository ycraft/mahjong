load("//ydec/mahjong/tools:cc_lint_test.bzl",
     "cc_lint_test", "cc_clang_format_test")

cc_library(
    name = "common_test_util_lib",
    srcs = [
      "common_test_util.cc",
    ],
    hdrs = [
      "common_test_util.h",
    ],
    deps = [
      "//ydec/mahjong/proto:mahjong_scorecalculator_cc_proto",
    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
      "hand_parser_test.cc",
      "mahjong_common_util_test.cc",
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

cc_lint_test(
    name = "cc_lint_test",
    srcs = glob(["*.h", "*.cc"]),
)

cc_clang_format_test(
    name = "cc_clang_format_test",
    srcs = glob(["*.h", "*.cc"]),
)
