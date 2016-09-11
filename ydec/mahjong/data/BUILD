package(default_visibility = ["//visibility:public"])

genrule(
    name = "rule_pb",
    srcs = ["rule.pb.txt"],
    outs = ["rule.pb"],
    tools = ["//ydec/mahjong/tools:update_rule"],
    cmd = "$(location //ydec/mahjong/tools:update_rule) $(SRCS) $(OUTS)",
)
