FROM ubuntu:latest

COPY ./build/include  /usr/local/include
COPY ./build/bin  /usr/local/bin
COPY ./build/lib  /usr/local/lib
COPY ./build/lib-gnu   /usr/lib/x86_64-linux-gnu/

# cp -a ../../include/ ./build/include
# cp -a ../../third_party/libs/linux/ ./build/lib
# cp -a ../../third_party/libs/linux-gnu ./build/lib-gnu
# docker build -t ptp:latest -f Dockerfile ./
# docker run -it ptp:latest bash
#
# docker tag ptp:latest sanfun/ptp:1.0.1
# docker tag sanfun/ptp:1.0.1 sanfun/ptp:latest
# docker login
# docker push sanfun/ptp:latest
# docker push sanfun/ptp:1.0.1
#