//
//  ViewController.h
//  sctest
//
//  Created by Dora Orak on 11.12.2024.
//

#import <Cocoa/Cocoa.h>
#import <ScreenCaptureKit/ScreenCaptureKit.h>
#import <IOSurface/IOSurfaceObjC.h>
#import <CoreImage/CoreImage.h>
#import "networking.h"

@interface ViewController : NSViewController <SCStreamDelegate, SCStreamOutput>

@property (nonatomic, strong) SCStream *stream;
@property (nonatomic, strong) NSWindow *monitorWindow;
/*
@property (nonatomic, strong) IOSurface *surface;
@property (nonatomic) CVPixelBufferRef pbuf;
@property (nonatomic) size_t pbufW;
@property (nonatomic) size_t pbufH;
*/

-(void) updateMonitorWithData:(void*) data size:(size_t) size;
@end

