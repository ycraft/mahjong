def cc_lint_test_impl(ctx):
  args = " ".join(
      [ctx.expand_make_variables("cmd", arg, {})
          for arg in ctx.attr.linter_args])
  ctx.file_action(
      content = "%s %s %s" % (
          ctx.executable.linter.short_path,
          args,
          " ".join([src.short_path for src in ctx.files.srcs])),
      output = ctx.outputs.executable,
      executable = True)

  return struct(
      runfiles = ctx.runfiles(files = ctx.files.linter + ctx.files.srcs),
  )

cc_lint_test = rule(
    attrs = {
      "srcs": attr.label_list(allow_files = True),
      "linter": attr.label(
          default = Label("@styleguide//:cpplint"),
          executable = True,
          cfg = "host"),
      "linter_args": attr.string_list(
          default=["--root=$$(pwd)"]),
    },
    test = True,
    implementation = cc_lint_test_impl,
)

def cc_clang_format_test_impl(ctx):
  cmds = []
  cmds.append("for src in %s; do" % (
      " ".join([src.short_path for src in ctx.files.srcs])))
  cmds.append("clang-format -style=google $src > /tmp/formatted_src ;")
  cmds.append("diff -u $src /tmp/formatted_src ;")
  cmds.append("done")
  ctx.file_action(
      content = " ".join(cmds),
      output = ctx.outputs.executable,
      executable = True)
  return struct(
      runfiles = ctx.runfiles(files = ctx.files.srcs),
  )

cc_clang_format_test = rule(
    attrs = {
      "srcs": attr.label_list(allow_files = True),
    },
    test = True,
    implementation = cc_clang_format_test_impl,
)
