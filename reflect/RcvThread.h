//
//  RcvThread.mm
//  reflect
//
//  Created by Dora Orak on 16.12.2024.
//

 #import <Foundation/Foundation.h>
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
        
            
        
        if(_highestOrder == 0){
            _rbytes = calloc(packet->frameSize, 1);
            _highestOrder = packet->order;
        }
        
        if(packet->order > _highestOrder){
            _highestOrder = packet->order;
        }
        
        memcpy((char *)_rbytes + packet->byteOffset, packet->bytes, packet->byteCount);
        
        if(packet->byteOffset == packet->frameSize - packet->byteCount){//arc4random_uniform(5000) == 0
            
            dispatch_async(dispatch_get_main_queue(), ^{
                [[[(AppDelegate*)[NSApplication.sharedApplication delegate] window] contentViewController] updateMonitorWithData:_rbytes size:size];
            });
    
        }
        
    }

@end

    
