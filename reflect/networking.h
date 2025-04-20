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
        unsigned compressedFrameSize;
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
        
        if(newSize % fragmentSize != 0){
            packetCount++;
        }
        
        for (unsigned packetIndex = 0; packetIndex < packetCount; packetIndex++) {
            connection::packet* pkg = new packet;
            
            pkg->byteOffset = fragmentSize * packetIndex;
            pkg->order = packetIndex+1;
            pkg->compressedFrameSize = (unsigned)newSize;
            pkg->frameSize = (unsigned)size;
            pkg->stride = (unsigned)stride;
            
            if(packetIndex != packetCount - 1){
                pkg->byteCount = fragmentSize;
            }
            else{
                pkg->byteCount = (unsigned)newSize - pkg->byteOffset;
            }
            
            memcpy(pkg->bytes, (char *)newData + pkg->byteOffset, pkg->byteCount);
            
            if(sendData((void*)pkg, sizeof(packet)) <= 0){
                std::cout << "couldn't send packet fragment";
            }
            else{
                std::cout << "sent packet\n";
            }
            
            delete pkg;
        }
        free(newData);
    }
    
    ~connection(){
        this->destroySocket();
    }
};

extern std::unique_ptr<connection> con;
