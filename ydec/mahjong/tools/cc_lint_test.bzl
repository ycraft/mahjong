def cc_lint_test_impl(ctx):
  ctx.file_action(
      content = "%s --root=$(pwd) %s" % (
          ctx.executable.linter.short_path,
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
          cfg = HOST_CFG),
    },
    test = True,
    implementation = cc_lint_test_impl,
)
