genrule(
    name = "gen_binary_rule_proto",
    srcs = ["data/rule.pb.txt"],
    outs = ["res/raw/rule.pb"],
    tools = ["//tools:update_rule"],
    cmd = "$(location //tools:update_rule) $(SRCS) $(OUTS)",
)

cc_library(
    name = "mahjong_score_calculator_lib",
    srcs = [
      "src/HandParser.cpp",
      "src/MahjongCommonUtils.cpp",
      "src/MahjongCommonValue.cpp",
      "src/ScoreCalculator.cpp",
      "src/YakuApplier.cpp",
    ],
    hdrs = [
      "src/HandParser.h",
      "src/MahjongCommonUtils.h",
      "src/MahjongCommonValue.h",
      "src/ScoreCalculator.h",
      "src/YakuApplier.h",
    ],
    deps = [
      "//proto:mahjong-scorecalculator",
    ],
)

cc_library(
    name = "common_test_util_lib",
    srcs = [
      "tests/CommonTestUtil.cpp",
    ],
    hdrs = [
      "tests/CommonTestUtil.h",
    ],
    deps = [
      "//proto:mahjong-scorecalculator",
    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
      "tests/HandParserTest.cpp",
      "tests/MahjongCommonUtilsTest.cpp",
      "tests/ScoreCalculatorTest.cpp",
      "tests/YakuApplierTest.cpp",
    ],
    data = [
      ":gen_binary_rule_proto",
    ],
    deps = [
      ":mahjong_score_calculator_lib",
      ":common_test_util_lib",
      "@googletest//:gtest_main",
    ],
)
