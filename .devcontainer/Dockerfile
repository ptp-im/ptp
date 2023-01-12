# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.162.0/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version: debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="buster"
#FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}
FROM sanfun/bd-im:bd-centos7-libs

RUN yun install -y automake libtool
# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
