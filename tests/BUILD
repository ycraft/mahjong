cc_library(
    name = "common_test_util_lib",
    srcs = [
      "CommonTestUtil.cpp",
    ],
    hdrs = [
      "CommonTestUtil.h",
    ],
    deps = [
      "//proto:mahjong-scorecalculator",
    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
      "HandParserTest.cpp",
      "MahjongCommonUtilsTest.cpp",
      "ScoreCalculatorTest.cpp",
      "YakuApplierTest.cpp",
    ],
    data = [
      "//data:rule_pb",
    ],
    deps = [
      ":common_test_util_lib",
      "//src:mahjong_score_calculator_lib",
      "@googletest//:gtest_main",
    ],
)
