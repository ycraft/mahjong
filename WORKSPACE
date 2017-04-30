git_repository(
    name = "protobuf",
    remote = "https://github.com/google/protobuf.git",
    tag = "v3.3.0",
)

new_http_archive(
    name = "googletest",
    url = "http://pkgs.fedoraproject.org/repo/pkgs/gmock/gmock-1.7.0.zip/073b984d8798ea1594f5e44d85b20d66/gmock-1.7.0.zip",
    sha256 = "26fcbb5925b74ad5fc8c26b0495dfc96353f4d553492eb97e85a8a6d2f43095b",
    build_file = "googletest.BUILD",
)

new_git_repository(
    name = "styleguide",
    remote = "https://github.com/google/styleguide.git",
    commit = "6d3a7d8a229e189f7a5bb7c3923363356625ece5",
    build_file = "styleguide.BUILD",
)
