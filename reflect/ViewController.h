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

-(void) updateMonitorWithData:(void*) data size:(size_t) size;
@end

