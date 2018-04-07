# proto_library rules implicitly depend on @com_google_protobuf//:protoc,
# which is the proto-compiler.
# This statement defines the @com_google_protobuf repo.
http_archive(
    name = "com_google_protobuf",
    urls = ["https://github.com/google/protobuf/archive/b5fbb742af122b565925987e65c08957739976a7.zip"],
    strip_prefix = "protobuf-b5fbb742af122b565925987e65c08957739976a7",
    sha256 = "9cd0c6c71274f730dfc1a401e59d63c104542144c705a99518b93b39896517c5",
)

# cc_proto_library rules implicitly depend on @com_google_protobuf_cc//:cc_toolchain,
# which is the C++ proto runtime (base classes and common utilities).
http_archive(
    name = "com_google_protobuf_cc",
    urls = ["https://github.com/google/protobuf/archive/b5fbb742af122b565925987e65c08957739976a7.zip"],
    strip_prefix = "protobuf-b5fbb742af122b565925987e65c08957739976a7",
    sha256 = "9cd0c6c71274f730dfc1a401e59d63c104542144c705a99518b93b39896517c5",
)

# java_proto_library rules implicitly depend on @com_google_protobuf_java//:java_toolchain,
# which is the Java proto runtime (base classes and common utilities).
http_archive(
    name = "com_google_protobuf_java",
    urls = ["https://github.com/google/protobuf/archive/b5fbb742af122b565925987e65c08957739976a7.zip"],
    strip_prefix = "protobuf-b5fbb742af122b565925987e65c08957739976a7",
    sha256 = "9cd0c6c71274f730dfc1a401e59d63c104542144c705a99518b93b39896517c5",
)

# java_lite_proto_library rules implicitly depend on @com_google_protobuf_javalite//:javalite_toolchain,
# which is the JavaLite proto runtime (base classes and common utilities).
http_archive(
    name = "com_google_protobuf_javalite",
    urls = ["https://github.com/google/protobuf/archive/b5fbb742af122b565925987e65c08957739976a7.zip"],
    strip_prefix = "protobuf-b5fbb742af122b565925987e65c08957739976a7",
    sha256 = "9cd0c6c71274f730dfc1a401e59d63c104542144c705a99518b93b39896517c5",
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
