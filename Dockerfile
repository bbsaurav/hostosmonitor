# Get the latest hostosmonitor-dependency from Docker Hub
FROM bbsaurav/hostosmonitor-dependency:latest

COPY . /usr/src/myapp
WORKDIR /usr/src/myapp

# Build code
RUN g++ -I./include -I./external/libssh/include/ -L./external/libssh/lib/ -o sshclient src/SSHClient.cpp src/main.cpp -lssh

# Set Environmental variable LD_LIBRARY_PATH to search dependent libraries
ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# This command runs your application, comment out this line to compile only
CMD ["./sshclient"]
#CMD ["echo $LD_LIBRARY_PATH"]

LABEL Name=docker Version=0.0.1
