//
//  networking.cpp
//  sctest
//
//  Created by Dora Orak on 15.12.2024.
//

#import <iostream>
#import <sys/socket.h>
#import <netinet/in.h>
#import <arpa/inet.h>
#import <unistd.h>
#import <errno.h>

//#define IP "127.0.0.1" //localhost
//#define IP "192.168.1.164" //iphone
#define IP "192.168.1.106" //google tv


class connection
{
public:
    int socketfd = 0;
    struct sockaddr_in serverAddr = {0};
    
    struct PacketBody {
        unsigned order;
        unsigned byteOffset;
        unsigned byteCount;
        unsigned frameSize;
        char bytes[4080]; // pick a number
        
    } typedef packet;
    
    int setupSocket(){
        
        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketfd < 0) {
            std::cout << "Error creating socket";
            return -1;
        }
        
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(27779); // Port number
        serverAddr.sin_addr.s_addr = inet_addr(IP);//inet_addr("192.168.1.106");inet_addr("127.0.0.1"); // Broadcast address or receiver's IP;
        inet_pton(AF_INET, IP, &serverAddr.sin_addr);
        
        int reuse = 1;
        if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
            perror("setsockopt");
            return -1;
        }
        
        int sbuf_size = 1024 * 1024 * 100; // Example: 100MB
        if(setsockopt(socketfd, SOL_SOCKET, SO_SNDBUF, &sbuf_size, sizeof(sbuf_size)) < 0){
            perror("setsockopt");
            return -1;
        }
        if(setsockopt(socketfd, SOL_SOCKET, SO_RCVBUF, &sbuf_size, sizeof(sbuf_size)) < 0){
            perror("setsockopt");
            return -1;
        }
            
        
        return 0;
    }
    
    int destroySocket(){
            return close(socketfd);
        }
    
    size_t sendData(const void* data, size_t size){
        
        long ret = sendto(socketfd, data, size, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if(ret <= 0){
            std::cout << "sendto error:" << strerror(errno) << "\n";
        }
        free((char*)data);
        return (size_t)ret;
        
    }
    
    void sendDataFragmented(const void* data, size_t size){
        
        size_t const fragmentSize = sizeof(((packet *)0)->bytes);
        
        size_t packetCount = size/fragmentSize;
        if(size % fragmentSize != 0){
            packetCount++;
        }
        
        for (unsigned packetIndex = 0; packetIndex < packetCount; packetIndex++) {
            packet* pkg = (packet*)malloc(sizeof(packet));
            
            pkg->byteOffset = fragmentSize * packetIndex;
            pkg->order = packetIndex+1;
            pkg->frameSize = (unsigned)size;
            
            if(packetIndex != packetCount - 1){
                pkg->byteCount = fragmentSize;
                
            }
            else{
                pkg->byteCount = (unsigned)size - pkg->byteOffset;
            }
            
            memcpy(pkg->bytes, (char *)data + pkg->byteOffset, pkg->byteCount);
            
            if(sendData(pkg, sizeof(packet)) <= 0){
                std::cout << "couldn't send packet fragment";
            }
        }
    }
    
    ~connection(){
        this->destroySocket();
    }
};

extern std::unique_ptr<connection> con;
