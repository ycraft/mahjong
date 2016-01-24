genrule(
    name = "gen_mahjong_proto",
    srcs = [
        "proto/mahjong-common.proto",
        "proto/mahjong-rule.proto",
        "proto/mahjong-scorecalculator.proto",
    ],
    outs = [
        "src-gen/cpp/mahjong-common.pb.h",
        "src-gen/cpp/mahjong-common.pb.cc",
        "src-gen/cpp/mahjong-rule.pb.h",
        "src-gen/cpp/mahjong-rule.pb.cc",
        "src-gen/cpp/mahjong-scorecalculator.pb.h",
        "src-gen/cpp/mahjong-scorecalculator.pb.cc",
    ],
    cmd = "protoc -I=proto --cpp_out=$(GENDIR)/src-gen/cpp $(SRCS)",
)

cc_library(
    name = "mahjong_proto_lib",
    srcs = [":gen_mahjong_proto"],
    hdrs = [":gen_mahjong_proto"],
    deps = [
        "@protobuf//:protobuf",
    ],
)

cc_library(
    name = "mahjong_score_calculator_lib",
    srcs = [
        "src/HandParser.cpp",
        "src/MahjongCommonUtils.cpp",
        "src/ScoreCalculater.cpp",
        "src/YakuApplier.cpp",
    ],
    hdrs = [
        "src/HandParser.h",
        "src/MahjongCommonUtils.h",
        "src/MahjongCommonValue.h",
        "src/ScoreCalculater.h",
        "src/YakuApplier.h",
    ],
    deps = [
        ":mahjong_proto_lib",
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
        ":mahjong_proto_lib",
    ],
)

cc_test(
    name = "unit_tests",
    srcs = [
        "tests/HandParserTest.cpp",
        "tests/MahjongCommonUtilsTest.cpp",
        "tests/YakuApplierTest.cpp",
    ],
    data = [
        "res/raw/rule.pb",
    ],
    deps = [
        ":mahjong_score_calculator_lib",
        ":common_test_util_lib",
        "@gmock//:gtest_main",
    ],
)
