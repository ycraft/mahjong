load("@protobuf//:protobuf.bzl", "cc_proto_library")

package(default_visibility = ["//visibility:public"])

cc_proto_library(
    name = "mahjong-common",
    srcs = ["mahjong-common.proto"],
    include = "",
    protoc = "@protobuf//:protoc",
    default_runtime = "@protobuf//:protobuf",
)

cc_proto_library(
    name = "mahjong-rule",
    srcs = ["mahjong-rule.proto"],
    deps = [":mahjong-common"],
    include = "",
    protoc = "@protobuf//:protoc",
    default_runtime = "@protobuf//:protobuf",
)

cc_proto_library(
    name = "mahjong-scorecalculator",
    srcs = ["mahjong-scorecalculator.proto"],
    deps = [
      ":mahjong-common",
      ":mahjong-rule",
    ],
    include = "",
    protoc = "@protobuf//:protoc",
    default_runtime = "@protobuf//:protobuf",
)
