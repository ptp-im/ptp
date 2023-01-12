FROM sanfun/bd-im:bd-centos7-libs
ENV VERSION v1.0.67
ADD ./ /usr/local/src/ptp/
ADD ./third_party/libs/linux /usr/local/src/ptp/lib
#ADD ./third_party/secp256k1/include /usr/local/src/ptp/include/secp256k1
#ADD ./third_party/redis/*.h /usr/local/src/ptp/include/redis
#ADD ./third_party/protobuf/google /usr/local/src/ptp/include/protobuf/google
WORKDIR /usr/local/src/ptp
RUN sh clean.sh
ENTRYPOINT ["python","/usr/local/src/ptp/docker-entrypoint.py"]
CMD ["bash"]
