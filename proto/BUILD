package(default_visibility = ["//visibility:public"])

proto_library(
    name = "mahjong_common_proto",
    srcs = ["mahjong_common.proto"],
)

proto_library(
    name = "mahjong_rule_proto",
    srcs = ["mahjong_rule.proto"],
    deps = [":mahjong_common_proto"],
)

cc_proto_library(
    name = "mahjong_rule_cc_proto",
    deps = [":mahjong_rule_proto"],
)

proto_library(
    name = "mahjong_scorecalculator_proto",
    srcs = ["mahjong_scorecalculator.proto"],
    deps = [
      ":mahjong_common_proto",
      ":mahjong_rule_proto",
    ],
)

cc_proto_library(
    name = "mahjong_scorecalculator_cc_proto",
    deps = [":mahjong_scorecalculator_proto"],
)
