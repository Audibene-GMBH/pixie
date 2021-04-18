# Copyright 2018- The Pixie Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# SPDX-License-Identifier: Apache-2.0

load("@bazel_gazelle//:deps.bzl", "gazelle_dependencies")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file")
load("@com_github_bazelbuild_buildtools//buildifier:deps.bzl", "buildifier_dependencies")
load("@distroless//package_manager:dpkg.bzl", "dpkg_list", "dpkg_src")
load("@distroless//package_manager:package_manager.bzl", "package_manager_repositories")
load("@io_bazel_rules_docker//container:container.bzl", "container_pull")
load("@io_bazel_rules_docker//go:image.bzl", _go_image_repos = "repositories")
load("@io_bazel_rules_docker//java:image.bzl", _java_image_repos = "repositories")
load("@io_bazel_rules_docker//repositories:deps.bzl", container_deps = "deps")
load(
    "@io_bazel_rules_docker//repositories:repositories.bzl",
    container_repositories = "repositories",
)
load("@io_bazel_rules_k8s//k8s:k8s.bzl", "k8s_repositories")
load("@io_bazel_rules_k8s//k8s:k8s_go_deps.bzl", k8s_go_deps = "deps")

# Sets up package manager which we use build deploy images.
def _package_manager_setup():
    package_manager_repositories()

    dpkg_src(
        name = "debian_sid",
        arch = "amd64",
        distro = "sid",
        sha256 = "a093727908ebb7e46cc83643b21d5e81eadee49efaa3ecae7aa7ff1a62858396",
        snapshot = "20200701T101354Z",
        url = "http://snapshot.debian.org/archive",
    )

    dpkg_list(
        name = "package_bundle",
        packages = [
            "libc6",
            "libelf1",
            "liblzma5",
            "libtinfo6",
            "zlib1g",
            "libsasl2-2",
            "libssl1.1",
            "libgcc1",
        ],
        sources = ["@debian_sid//file:Packages.json"],
    )

# Set-up images used by Stirling BPF tests.
def stirling_docker_images_setup():
    # Busybox container, for basic container tests.
    container_pull(
        name = "distroless_base_image",
        digest = "sha256:f989df6099c5efb498021c7f01b74f484b46d2f5e1cdb862e508569d87569f2b",
        registry = "gcr.io",
        repository = "distroless/base",
    )

    # NGINX with OpenSSL 1.1.0, for OpenSSL tracing tests.
    container_pull(
        name = "nginx_openssl_1_1_0_base_image",
        digest = "sha256:204a9a8e65061b10b92ad361dd6f406248404fe60efd5d6a8f2595f18bb37aad",
        registry = "index.docker.io",
        repository = "library/nginx",
    )

    # NGINX with OpenSSL 1.1.1, for OpenSSL tracing tests.
    container_pull(
        name = "nginx_openssl_1_1_1_base_image",
        digest = "sha256:0b159cd1ee1203dad901967ac55eee18c24da84ba3be384690304be93538bea8",
        registry = "index.docker.io",
        repository = "library/nginx",
    )

    # DNS server image for DNS tests.
    container_pull(
        name = "alpine_dns_base_image",
        digest = "sha256:b9d834c7ca1b3c0fb32faedc786f2cb96fa2ec00976827e3f0c44f647375e18c",
        registry = "index.docker.io",
        repository = "resystit/bind9",
    )

    # Curl container, for OpenSSL tracing tests.
    # curlimages/curl:7.74.0
    container_pull(
        name = "curl_base_image",
        digest = "sha256:5594e102d5da87f8a3a6b16e5e9b0e40292b5404c12f9b6962fd6b056d2a4f82",
        registry = "index.docker.io",
        repository = "curlimages/curl",
    )

    # Ruby container, for OpenSSL tracing tests.
    # ruby:3.0.0-buster
    container_pull(
        name = "ruby_base_image",
        digest = "sha256:beeed8e63b1ae4a1492f4be9cd40edc6bdb1009b94228438f162d0d05e10c8fd",
        registry = "index.docker.io",
        repository = "library/ruby",
    )

    # Datastax DSE server, for CQL tracing tests.
    # datastax/dse-server:6.7.7
    container_pull(
        name = "datastax_base_image",
        digest = "sha256:a98e1a877f9c1601aa6dac958d00e57c3f6eaa4b48d4f7cac3218643a4bfb36e",
        registry = "index.docker.io",
        repository = "datastax/dse-server",
    )

    # Postgres server, for PGSQL tracing tests.
    # postgres:13.2
    container_pull(
        name = "postgres_base_image",
        digest = "sha256:661dc59f4a71e689c51d4823963baa56b8fcc8daa5b16cf740cad236fa5ffe74",
        registry = "index.docker.io",
        repository = "library/postgres",
    )

    # Redis server, for Redis tracing tests.
    # redis:6.2.1
    container_pull(
        name = "redis_base_image",
        digest = "sha256:fd68bec9c2cdb05d74882a7eb44f39e1c6a59b479617e49df245239bba4649f9",
        registry = "index.docker.io",
        repository = "library/redis",
    )

    # MySQL server, for MySQL tracing tests.
    # mysql/mysql-server:8.0.13
    container_pull(
        name = "mysql_base_image",
        digest = "sha256:3d50c733cc42cbef715740ed7b4683a8226e61911e3a80c3ed8a30c2fbd78e9a",
        registry = "index.docker.io",
        repository = "mysql/mysql-server",
    )

    # Custom-built container with python MySQL client, for MySQL tests.
    container_pull(
        name = "python_mysql_connector_image",
        digest = "sha256:5f28e6eee56f40eb13cae7b7e2ccc305183ceb7be7ec44d862209d7f522cd316",
        registry = "gcr.io",
        repository = "pl-dev-infra/python_mysql_connector",
    )

def _docker_images_setup():
    _go_image_repos()
    _java_image_repos()

    # Import NGINX repo.
    container_pull(
        name = "nginx_base",
        digest = "sha256:204a9a8e65061b10b92ad361dd6f406248404fe60efd5d6a8f2595f18bb37aad",
        registry = "index.docker.io",
        repository = "library/nginx",
    )

    container_pull(
        name = "openresty",
        # Stretch image.
        digest = "sha256:1702786dcbb5b6b6d096f5e56b2153d8b508e62396fd4324367913b6645bb0b8",
        registry = "index.docker.io",
        repository = "openresty/openresty",
    )

    container_pull(
        name = "base_image",
        digest = "sha256:e37cf3289c1332c5123cbf419a1657c8dad0811f2f8572433b668e13747718f8",
        registry = "gcr.io",
        repository = "distroless/base",
    )

    container_pull(
        name = "base_image_debug",
        digest = "sha256:f989df6099c5efb498021c7f01b74f484b46d2f5e1cdb862e508569d87569f2b",
        registry = "gcr.io",
        repository = "distroless/base",
    )

    stirling_docker_images_setup()

def _artifacts_setup():
    http_file(
        name = "linux_headers_4_14_176_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-4.14.176-trimmed-pl3.tar.gz"],
        sha256 = "67a59f55cb8592ed03719fedb925cdf7a2dc8529fcf9ab1002405540a855212c",
        downloaded_file_path = "linux-headers-4.14.176.tar.gz",
    )

    http_file(
        name = "linux_headers_4_15_18_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-4.15.18-trimmed-pl3.tar.gz"],
        sha256 = "0a82dea437d1798a88df95498892f9d14a5158f25184f42a90c5ce093645529d",
        downloaded_file_path = "linux-headers-4.15.18.tar.gz",
    )

    http_file(
        name = "linux_headers_4_16_18_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-4.16.18-trimmed-pl3.tar.gz"],
        sha256 = "738362e58aa11a51ff292c0520dd36ddfecc9ca1494c8b2841d01e51ceaf769a",
        downloaded_file_path = "linux-headers-4.16.18.tar.gz",
    )

    http_file(
        name = "linux_headers_4_17_19_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-4.17.19-trimmed-pl3.tar.gz"],
        sha256 = "38855fd5786fd459d92ce7193fc7379af2c1a7480e0bac95b0ba291fc08b4eea",
        downloaded_file_path = "linux-headers-4.17.19.tar.gz",
    )

    http_file(
        name = "linux_headers_4_18_20_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-4.18.20-trimmed-pl3.tar.gz"],
        sha256 = "efff57e9642ad968ceee4b7c0f7387fd2507499c12bda79b850b40fa35951265",
        downloaded_file_path = "linux-headers-4.18.20.tar.gz",
    )

    http_file(
        name = "linux_headers_4_19_118_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-4.19.118-trimmed-pl3.tar.gz"],
        sha256 = "43253ad88cc276b293c0cbe35b684e5462af2ffa180775c0973b0e278b4f9ee6",
        downloaded_file_path = "linux-headers-4.19.118.tar.gz",
    )

    http_file(
        name = "linux_headers_4_20_17_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-4.20.17-trimmed-pl3.tar.gz"],
        sha256 = "e87bdd101fd2441718a4f0999669d767c52247c4cb7294f71f9136b7eb723d78",
        downloaded_file_path = "linux-headers-4.20.17.tar.gz",
    )

    http_file(
        name = "linux_headers_5_0_21_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-5.0.21-trimmed-pl3.tar.gz"],
        sha256 = "848a1135a69763bac3afff1c1bf9ac3ba63d04026479d146936d701619b44bb1",
        downloaded_file_path = "linux-headers-5.0.21.tar.gz",
    )

    http_file(
        name = "linux_headers_5_1_21_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-5.1.21-trimmed-pl3.tar.gz"],
        sha256 = "4750ca03b38301f3627b47a4dc5690e6d5ba641c18a6eafdb37cb8f86614572f",
        downloaded_file_path = "linux-headers-5.1.21.tar.gz",
    )

    http_file(
        name = "linux_headers_5_2_21_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-5.2.21-trimmed-pl3.tar.gz"],
        sha256 = "36c90df582a85c865e7fefe99db51fd82117c32bdd72452da5a47e73da8b7355",
        downloaded_file_path = "linux-headers-5.2.21.tar.gz",
    )

    http_file(
        name = "linux_headers_5_3_18_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-5.3.18-trimmed-pl3.tar.gz"],
        sha256 = "2e4b3eff995177122c4f28096f5a9a815fb2a1d0c025dc5340d6d86a9a7796e9",
        downloaded_file_path = "linux-headers-5.3.18.tar.gz",
    )

    http_file(
        name = "linux_headers_5_4_35_tar_gz",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/linux-headers-5.4.35-trimmed-pl3.tar.gz"],
        sha256 = "f371fc16c3542b6a7a47788693f00e743ec82996925c3dee7123c588e59210f7",
        downloaded_file_path = "linux-headers-5.4.35.tar.gz",
    )

    http_file(
        name = "timeconst_100",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/timeconst_100.h"],
        sha256 = "082496c45ab93af811732da56000caf5ffc9e6734ff633a2b348291f160ceb7e",
        downloaded_file_path = "timeconst_100.h",
    )

    http_file(
        name = "timeconst_250",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/timeconst_250.h"],
        sha256 = "0db01d74b846e39dca3612d96dee8b8f6addfaeb738cc4f5574086828487c2b9",
        downloaded_file_path = "timeconst_250.h",
    )

    http_file(
        name = "timeconst_300",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/timeconst_300.h"],
        sha256 = "91c6499df71695699a296b2fdcbb8c30e9bf35d024e048fa6d2305a8ac2af9ab",
        downloaded_file_path = "timeconst_300.h",
    )

    http_file(
        name = "timeconst_1000",
        urls = ["https://storage.googleapis.com/pl-infra-dev-artifacts/timeconst_1000.h"],
        sha256 = "da0ba6765f2969482bf8eaf21249552557fe4d6831749d9cfe4c25f4661f8726",
        downloaded_file_path = "timeconst_1000.h",
    )

# TODO(zasgar): remove this when downstream bugs relying on bazel version are removed.
def _impl(repository_ctx):
    bazel_verision_for_upb = "bazel_version = \"" + native.bazel_version + "\""
    bazel_version_for_foreign_cc = "BAZEL_VERSION = \"" + native.bazel_version + "\""
    repository_ctx.file("bazel_version.bzl", bazel_verision_for_upb)
    repository_ctx.file("def.bzl", bazel_version_for_foreign_cc)
    repository_ctx.file("BUILD", "")

bazel_version_repository = repository_rule(
    implementation = _impl,
    local = True,
)

def pl_workspace_setup():
    gazelle_dependencies()
    buildifier_dependencies()

    bazel_version_repository(
        name = "bazel_version",
    )

    container_repositories()
    container_deps()

    k8s_repositories()
    k8s_go_deps()

def pl_docker_images():
    _package_manager_setup()
    _docker_images_setup()
    _artifacts_setup()
