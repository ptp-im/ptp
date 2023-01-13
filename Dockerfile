ARG VARIANT="ubuntu-20.04"
#FROM mcr.microsoft.com/vscode/devcontainers/base:dev-${VARIANT}
#FROM mcr.microsoft.com/vscode/devcontainers/cpp:0-${VARIANT}
FROM sanfun/ptp-cpp:0-${VARIANT}

#RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#    && apt-get -y install build-essential cmake cppcheck valgrind clang lldb llvm gdb
#RUN apt-get -y install --no-install-recommends autoconf libtool libssl-dev curl libcurl4 libcurl4-openssl-dev libapr1 libapr1-dev libaprutil1-dev \
#    libmariadb-dev libmariadb-dev-compat libmariadb3 --fix-missing
#RUN apt-get -y install --no-install-recommends redis-server redis-tools
#RUN apt-get autoremove -y && apt-get clean -y && rm -rf /var/lib/apt/lists/*

#WORKDIR     /workspaces/ptp/
ADD     ./  /workspaces/ptp/

RUN sh clean.sh

# docker build -t ptp-cpp:latest -f Dockerfile ./
# docker run -it ptp-cpp:latest bash
#
# docker tag ptp-cpp:1.0.1 sanfun/ptp-cpp:1.0.1
# docker tag sanfun/ptp-cpp:1.0.1 sanfun/ptp-cpp:latest
# docker tag sanfun/ptp-cpp:1.0.1 sanfun/ptp-cpp:0-ubuntu-20.04
# docker login
# docker push sanfun/ptp-cpp:latest
# docker push sanfun/ptp-cpp:1.0.1
# docker push sanfun/ptp-cpp:0-ubuntu-20.04
#
# apt-cache search redis
# apt-get -y install redis-server
# cd /opt && sudo redis-server --appendonly yes --daemonize yes --requirepass s9mE_s3cUrE_prsS
# redis-cli "CONFIG SET requirepass s9mE_s3cUrE_prsS"
# AUTH s9mE_s3cUrE_prsS
# killall redis-server
