# Copy cpplint/cpplint.py to this directory to work around a py_binary()'s
# restriction that all python scripts must be in this directory but not in
# subdirectoriy. This genrule copies cpplint.py from subdirectory and that will
# be used by "cpplint" py_binary below.
genrule(
    name = "cpplint_src",
    srcs = ["cpplint/cpplint.py"],
    outs = ["cpplint.py"],
    cmd = "cp $(SRCS) $(OUTS)",
)

py_binary(
    name = "cpplint",
    srcs = ["cpplint.py"],
    visibility = ["//visibility:public"],
    data = [":cpplint_src"],
)
