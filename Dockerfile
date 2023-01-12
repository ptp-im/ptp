FROM sanfun/bd-im:bd-centos7-libs
ENV VERSION v1.0.67
ADD ./ /usr/local/src/ptp/
ADD ./third_party/libs/linux /usr/local/src/ptp/lib
RUN yun install autoconf

WORKDIR /usr/local/src/ptp
RUN sh clean.sh
ENTRYPOINT ["python","/usr/local/src/ptp/docker-entrypoint.py"]
CMD ["bash"]
