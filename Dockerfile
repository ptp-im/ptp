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

RUN curl -fsSL https://packages.redis.io/gpg | sudo gpg --dearmor -o /usr/share/keyrings/redis-archive-keyring.gpg \
  && echo "deb [signed-by=/usr/share/keyrings/redis-archive-keyring.gpg] https://packages.redis.io/deb $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/redis.list

RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install build-essential redis

ADD     ./  /workspaces/ptp/
WORKDIR     /workspaces/ptp/
RUN chmod +x ./ptp-ctl.sh && ./ptp-ctl.sh clean_ptp

# docker build -t ptp-cpp:latest -f Dockerfile ./
# docker run -it ptp-cpp:latest bash
#
# docker tag ptp-cpp:latest sanfun/ptp-cpp:1.0.2
# docker tag sanfun/ptp-cpp:1.0.2 sanfun/ptp-cpp:latest
# docker tag sanfun/ptp-cpp:1.0.2 sanfun/ptp-cpp:0-ubuntu-20.04
# docker login
# docker push sanfun/ptp-cpp:latest
# docker push sanfun/ptp-cpp:1.0.2
# docker push sanfun/ptp-cpp:0-ubuntu-20.04
#