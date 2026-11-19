# Build and test FeenoX in a container, using the same Ubuntu packages
# as the GitHub Actions workflow (.github/workflows/compile_and_test.yaml).
# This is handy to reproduce CI failures locally.
#
# It works with either docker or podman (podman needs no daemon nor root).
# From the root of a git checkout of FeenoX:
#
#   podman build -t feenox .                 # or: docker build -t feenox .
#   podman run --rm feenox feenox --version  # run feenox
#   podman run --rm feenox make check        # run the test suite
#
# To solve a problem in the current directory:
#
#   podman run --rm -v $PWD:/work:Z -w /work feenox feenox input.fee
#
# Notes:
#  - The source tree is copied into the image (including .git, which autogen.sh
#    needs to figure out the version), so local uncommitted changes are built too.
#  - If the root partition is small, podman may fail with "no space left on device"
#    while building. Point TMPDIR somewhere with room, i.e.
#    TMPDIR=$HOME/tmp podman build -t feenox .
#  - Remove the image with "podman rmi feenox" (or "docker rmi feenox").

FROM ubuntu:26.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq && \
    apt-get install -y -qq --no-install-recommends \
      build-essential autoconf automake libtool m4 git ca-certificates \
      libgsl-dev libsundials-dev petsc-dev slepc-dev gmsh && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /feenox
COPY . /feenox

# the files are owned by whoever built the image, let git work anyway
RUN git config --global --add safe.directory /feenox && \
    ./autogen.sh && \
    ./configure && \
    make -j"$(nproc)"

ENV PATH="/feenox:${PATH}"

CMD ["feenox", "--version"]
