load("//ydec/mahjong/tools:cc_lint_test.bzl", "cc_lint_test")

cc_library(
    name = "mahjong_score_calculator_lib",
    srcs = [
      "hand_parser.cc",
      "mahjong_common_util.cc",
      "mahjong_common_value.cc",
      "score_calculator.cc",
      "yaku_applier.cc",
    ],
    hdrs = [
      "hand_parser.h",
      "mahjong_common_util.h",
      "mahjong_common_value.h",
      "score_calculator.h",
      "yaku_applier.h",
    ],
    deps = [
      "//ydec/mahjong/proto:mahjong-scorecalculator",
    ],
    visibility = ["//visibility:public"],
)

cc_lint_test(
    name = "cc_lint_test",
    srcs = glob(["*.h", "*.cc"]),
)
