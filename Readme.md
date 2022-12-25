

    docker build -t clion/remote-cpp-env:0.5 -f Dockerfile .

    docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 --name clion_remote_env clion/remote-cpp-env:0.5

