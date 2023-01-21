ARG VARIANT="ubuntu-20.04"
FROM sanfun/ptp-cpp:0-${VARIANT}

########################################################
# Essential packages for remote debugging and login in
########################################################

RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    apt-utils gcc g++ openssh-server cmake build-essential gdb gdbserver rsync vim

# Taken from - https://docs.docker.com/engine/examples/running_ssh_service/#environment-variables

RUN mkdir /var/run/sshd
RUN echo 'root:root' | chpasswd
RUN mkdir /root/.ssh && echo "ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAABgQC1loPOPen3t2Nqd1RpW0PrHIsqib6gswVtonz1qNNAP+Dbfxynm600XlybRHiwFNDIpXOzwEGKbdGwjRIDI+sxzUQ8u4QZgPGJvRQA9oyPXcP6Epfqiv6eneYYIjxmu+6VZcEeVdYzbwp2p2+DvO6vMXOH6pW3mVpRBbukzw6+VSuBpM3CGJGl5dwkKkb3sycpeKDaqwoRMMTivzzWWN8tiUwdWCSxSgWLub7uvG0bFTuPMYvEFIaxAC7ghowF4e5WVszlo95ihOlNJSXuqS9nOMmyb6UTm6PRayRPul8i3HkJJHXaYcLYL+2gaHIbxekxxLWazbo7dK6LUR6nBfwfcu5aZWmE5HFDEeqCISgqjMBfblHjn8zIAkaPDk4Jz2OkjRN1cBeT6OSHPQ1GJw6l6nTad9+VkhKmaix/nyytB+lt4ADcOpkgM0POFPJ878R0qvCSQ2sG2YxCbhrhWPTVmj92jCR2L6RoyC/XNListmnwn5dEXlQGvGSSl8zLOpE= jack@jacks-MacBook-Pro.local" > /root/.ssh/authorized_keys

RUN sed -i 's/PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config

# SSH login fix. Otherwise user is kicked off after login
RUN sed 's@session\s*required\s*pam_loginuid.so@session optional pam_loginuid.so@g' -i /etc/pam.d/sshd

ENV NOTVISIBLE "in users profile"
RUN echo "export VISIBLE=now" >> /etc/profile

# 22 for ssh server. 7777 for gdb server.
EXPOSE 22 7777

RUN useradd -ms /bin/bash debugger
RUN echo 'debugger:pwd' | chpasswd


########################################################
# Add custom packages and development environment here
########################################################

########################################################
WORKDIR /workspaces/ptp/
CMD ["/usr/sbin/sshd", "-D"]