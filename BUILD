genrule(
    name = "gen_mahjong_proto_cc_hdrs",
    srcs = [
        "proto/mahjong-common.proto",
        "proto/mahjong-rule.proto",
        "proto/mahjong-scorecalculator.proto",
    ],
    outs = [
        "src-gen/cpp/mahjong-common.pb.h",
        "src-gen/cpp/mahjong-rule.pb.h",
        "src-gen/cpp/mahjong-scorecalculator.pb.h",
    ],
    cmd = "protoc -I=proto --cpp_out=$(GENDIR)/src-gen/cpp $(SRCS)",
)

genrule(
    name = "gen_mahjong_proto_cc_srcs",
    srcs = [
        "proto/mahjong-common.proto",
        "proto/mahjong-rule.proto",
        "proto/mahjong-scorecalculator.proto",
    ],
    outs = [
        "src-gen/cpp/mahjong-common.pb.cc",
        "src-gen/cpp/mahjong-rule.pb.cc",
        "src-gen/cpp/mahjong-scorecalculator.pb.cc",
    ],
    cmd = "protoc -I=proto --cpp_out=$(GENDIR)/src-gen/cpp $(SRCS)",
)

cc_library(
    name = "mahjong_proto_cc_lib",
    srcs = [":gen_mahjong_proto_cc_srcs"],
    hdrs = [":gen_mahjong_proto_cc_hdrs"],
    deps = ["@protobuf//:protobuf"],
    visibility = ["//visibility:public"],
)

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
        ":mahjong_proto_cc_lib",
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
        ":mahjong_proto_cc_lib",
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
        "@gmock//:gtest_main",
    ],
)
