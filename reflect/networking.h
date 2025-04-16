//
//  networking.cpp
//  sctest
//
//  Created by Dora Orak on 15.12.2024.
//
#pragma once

#import <iostream>
#import <sys/socket.h>
#import <netinet/in.h>
#import <arpa/inet.h>
#import <unistd.h>
#import <errno.h>
#import "lz4.h"

//#define IP "127.0.0.1" //localhost
#define IP "192.168.1.187" //iphone
//#define IP "192.168.1.153" //google tv
//#define IP "0.0.0.0" //any


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
        unsigned stride;
        char bytes[512]; // pick a number
        
    } typedef packet;
    
    int setupSocket(){
        
        socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketfd < 0) {
            std::cout << "Error creating socket";
            return -1;
        }
        
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(27779); // Port number
        serverAddr.sin_addr.s_addr = inet_addr(IP);
        inet_pton(AF_INET, IP, &serverAddr.sin_addr);
        //serverAddr.sin_addr.s_addr = INADDR_ANY;

        /*
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
        */
        
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
    
        return (size_t)ret;
        
    }
    
    void sendDataFragmented(const void* data, size_t size, size_t stride){
        
        size_t constexpr fragmentSize = sizeof(((packet *)0)->bytes);
        
        size_t newSize = 0;
        void* newData = malloc(size);
        newSize = LZ4_compress_default((const char*)data, (char*)newData, (int)size, (int)size);
        
        if(newSize <= 0){
            std::cout << "compression error";
            return;
        }
        
        
        size_t packetCount = newSize/fragmentSize;
        
        if(size % fragmentSize != 0){
            packetCount++;
        }
        
        for (unsigned packetIndex = 0; packetIndex < packetCount; packetIndex++) {
            std::unique_ptr<connection::packet> pkg = std::make_unique<connection::packet>();
            
            pkg->byteOffset = fragmentSize * packetIndex;
            pkg->order = packetIndex+1;
            pkg->frameSize = (unsigned)size;
            pkg->stride = (unsigned)stride;
            
            if(packetIndex != packetCount - 1){
                pkg->byteCount = fragmentSize;
            }
            else{
                pkg->byteCount = (unsigned)newSize - pkg->byteOffset;
            }
            
            memcpy(pkg->bytes, (char *)newData + pkg->byteOffset, pkg->byteCount);
            
            if(sendData((void*)pkg.get(), sizeof(packet)) <= 0){
                std::cout << "couldn't send packet fragment";
                
            }
            else{
                std::cout << "sent packet";
            }
            
        }
    }
    
    ~connection(){
        this->destroySocket();
    }
};

extern std::unique_ptr<connection> con;
