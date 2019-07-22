# hostosmonitor
To run the container use below commands:
```
docker pull bbsaurav/hostosmonitor:latest /* Pull latest container */
docker run --rm -it bbsaurav/hostosmonitor:latest /* Run container */
```
    
To build code after making changes use below commands after making changes in the code:
```
docker build --rm -f "Dockerfile" -t bbsaurav/hostosmonitor:staging_<alias>
docker push bbsaurav/hostosmonitor:staging_<alias>
```
**Note: Don't update the latest tag when pushing the changed container**

To build code locally use below command:
```
g++ -I./include -I./external/libssh/include/ -L./external/libssh/lib/ -o sshclient src/SSHClient.cpp src/main.cpp -lssh
```
