# See here for image contents: https://github.com/microsoft/vscode-dev-containers/tree/v0.162.0/containers/cpp/.devcontainer/base.Dockerfile

# [Choice] Debian / Ubuntu version: debian-10, debian-9, ubuntu-20.04, ubuntu-18.04
ARG VARIANT="buster"
FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}

# [Optional] Uncomment this section to install additional packages.
RUN apt-get update \
    && apt-get -y install autoconf libtool libssl-dev libcurl4-openssl-dev libmariadb-dev libmariadb-dev-compat libmariadb3 libapr1 libapr1-dev libaprutil1-dev --fix-missing