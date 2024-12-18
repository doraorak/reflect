//
//  LogThread.mm
//  reflect
//
//  Created by Dora Orak on 16.12.2024.
//



/*
#import <Foundation/Foundation.h>
#include <vector>
#include <map>
#import "networking.h"


@interface NSViewController (vc)
-(void) setSurface:(IOSurface*)arg1;
-(IOSurface*) surface;
-(void) updateMonitorWithData:(void*) arg1 size:(size_t) arg2;
@end

@interface NSApplication (category)

-(AppDelegate*) delegate;

@end

@interface RcvThread : NSThread

-(void) main;

@end

@implementation RcvThread
-(void) main{
    
    while(true){
        static std::vector<char> buf;
        static unsigned previousOrder = 0;
        
            void* rdata = malloc(1024*1024*50 * sizeof(char));
            con->receiveData(rdata);
            //std::cout << "recieved size: " << ((connection::packet*)rdata)->byteCount <<"\n";
        if(((connection::packet*)rdata)->order > previousOrder){
            if (((connection::packet*)rdata)->byteCount < sizeof(((connection::packet*)rdata)->bytes)) {//sizeof(((connection::packet*)rdata)->bytes)
                
                //Last fragment of a sent data
                
                buf.insert(buf.end(),((connection::packet*)rdata)->bytes,((connection::packet*)rdata)->bytes + ((connection::packet*)rdata)->byteCount);
                
                free(rdata);
                rdata = NULL;
                
                dispatch_sync(dispatch_get_main_queue(), ^{
                    [[[(AppDelegate*)[NSApplication.sharedApplication delegate] window] contentViewController] updateMonitorWithData:buf.data() size:buf.size()];
                });
                
                std::cout << "last bufsize: " << buf.size() << "\n";
                buf.clear();
                buf.shrink_to_fit();
            }
            else{
                
                buf.insert(buf.end(),((connection::packet*)rdata)->bytes,((connection::packet*)rdata)->bytes + ((connection::packet*)rdata)->byteCount);
                
                free(rdata);
                rdata = NULL;
            }
        }
    }
}
@end

*/














 #import <Foundation/Foundation.h>
 #include <stdlib.h>
 #import "networking.h"


 @interface NSViewController (vc)
 -(void) setSurface:(IOSurface*)arg1;
 -(IOSurface*) surface;
 -(void) updateMonitorWithData:(void*) arg1 size:(size_t) arg2;
 @end

 @interface NSApplication (category)

 -(AppDelegate*) delegate;

 @end

 @interface RcvThread : NSThread
 -(void) main;
 
 -(void) processPacket:(connection::packet*)pkt size:(size_t) size;
 
@property (nonatomic, readonly) void* rbytes;
@property (nonatomic, readonly) unsigned highestOrder;

 @end
 
@implementation RcvThread


-(void) main{
    connection::packet pkt;
    while(true){
        size_t size = con->receiveData(&pkt);
        
        if (size < 1){
            std::cout << "error rcv";
            
        }
        else{
            [self processPacket:&pkt size:size];
        }
    }
}


-(void) processPacket:(connection::packet*) packet size:(size_t) size
    {
        std::cout << _highestOrder << "\n";
        
            if(packet->order < _highestOrder){
                //old packet
                return;
            }
        
        if(_highestOrder == 0){
            _rbytes = calloc(packet->frameSize, 1);
            _highestOrder = packet->order;
        }
        
        if(packet->order > _highestOrder){
            _highestOrder = packet->order;
        }
        
        memcpy((char *)_rbytes + packet->byteOffset, packet->bytes, packet->byteCount);
        
        if(packet->byteOffset == packet->frameSize - packet->byteCount){
            dispatch_async(dispatch_get_main_queue(), ^{
                [[[(AppDelegate*)[NSApplication.sharedApplication delegate] window] contentViewController] updateMonitorWithData:_rbytes size:size];
            });
            free(_rbytes);
            _highestOrder = 0;
            std::cout << "last ?" << "\n";
        }
        
    }

@end

    
