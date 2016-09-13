load("//ydec/mahjong/tools:cc_lint_test.bzl",
     "cc_lint_test", "cc_clang_format_test")

cc_binary(
    name = "update_rule",
    srcs = ["update_rule.cc"],
    deps = [
      "//ydec/mahjong/proto:mahjong-rule",
    ],
    visibility = ["//visibility:public"],
)

cc_lint_test(
    name = "cc_lint_test",
    srcs = glob(["*.h", "*.cc"]),
)

cc_clang_format_test(
    name = "cc_clang_format_test",
    srcs = glob(["*.h", "*.cc"]),
)
