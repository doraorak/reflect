//
//  ViewController.m
//  sctest
//
//  Created by Dora Orak on 11.12.2024.
//

#import "ViewController.h"
#define w 1512
#define h 982

@implementation ViewController

-(void) updateMonitorWithData:(void*) data size:(size_t) size{
    NSDictionary *pixelBufferAttributes = @{
            (id)kCVPixelBufferPixelFormatTypeKey : @(kCVPixelFormatType_64RGBAHalf),
            (id)kCVPixelBufferCGImageCompatibilityKey: @(CFBooleanGetValue(kCFBooleanTrue)),
            (id)kCVPixelBufferWidthKey : @(1512),
            (id)kCVPixelBufferHeightKey : @(982),
        };
        
        //memset(data, -999999, size);
    /*
    NSData* ndata = [(NSString*)CFBridgingRelease(data) dataUsingEncoding:NSUTF8StringEncoding];
    void* buffer = malloc([ndata length]);
    [ndata getBytes:buffer length:[ndata length]];
    */
    
        CVPixelBufferRef pixelBuffer = NULL;
        CVPixelBufferCreateWithBytes(
            kCFAllocatorDefault,
                                     w*1,
                                     h*1,
                                     kCVPixelFormatType_64RGBAHalf,//1380411457,//kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange,
                                     (CVPixelBufferRef)data,//buffer
                                     12160,//12160,w*4,
            NULL,
            NULL,
            (__bridge CFDictionaryRef)pixelBufferAttributes,
            &pixelBuffer
        );
    
    
    if(pixelBuffer != NULL){
        CGImageRef cgImage = [[CIContext context] createCGImage:[CIImage imageWithCVPixelBuffer:pixelBuffer] fromRect:NSMakeRect(0, 0, 1512, 982)];
        NSImage* image = [[NSImage alloc] initWithCGImage:cgImage size:NSMakeSize(w, h)];
        CGImageRelease(cgImage);
        
        NSImageView* imageView = [[NSImageView alloc] initWithFrame:NSMakeRect(0, 0, 1512, 982)];
        imageView.image = image;
        imageView.hidden = NO;
        
        self.monitorWindow.contentView = imageView;
        [self.monitorWindow.contentView setNeedsDisplay:YES];
    }
    /*
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        NSImageView* imageView = [[NSImageView alloc] initWithFrame:NSMakeRect(0, 0, 1512, 982)];
        imageView.image = image;
        imageView.hidden = NO;
        
        self.monitorWindow.contentView = imageView;
        [self.monitorWindow.contentView setNeedsDisplay:YES];
    }); */
}


- (void)viewDidLoad {
    [super viewDidLoad];
    NSButton* button = [NSButton buttonWithTitle:@"start" target:self action: @selector(start:)];
    button.translatesAutoresizingMaskIntoConstraints = NO; // Enable Auto Layout
        
        // Add the button to the view
        [self.view addSubview:button];
        
        // Add constraints
        [NSLayoutConstraint activateConstraints:@[
            // Center the button horizontally in the view
            [button.centerXAnchor constraintEqualToAnchor:self.view.centerXAnchor],
            // Center the button vertically in the view
            [button.centerYAnchor constraintEqualToAnchor:self.view.centerYAnchor],
            // Set a fixed width
            [button.widthAnchor constraintEqualToConstant:100],
            // Set a fixed height
            [button.heightAnchor constraintEqualToConstant:50]
        ]];
    // Do any additional setup after loading the view.
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (void) stream:(SCStream *) stream
didStopWithError:(NSError *) error{
    
    NSLog(@"stop error:%@", error);
    
}

-(void) streamDidBecomeActive:(SCStream*) s {
    
    NSLog(@"active");
}

- (void) stream:(SCStream *) stream
didOutputSampleBuffer:(CMSampleBufferRef) sampleBuffer
         ofType:(SCStreamOutputType) type{
    
    //static int i = 0;
    
    if(!CMSampleBufferIsValid(sampleBuffer)){
        NSLog(@"sample invalid");
    }
    if(type == SCStreamOutputTypeScreen){
        
        CFArrayRef arr = CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, NO);
        CFDictionaryRef attachments = (CFDictionaryRef)CFArrayGetValueAtIndex(arr, 0);
        
        if(!attachments){
            NSLog(@"attachments not found");
        }
        else{
            
            NSNumber* statusNumber = (__bridge NSNumber*)static_cast<CFNumberRef>(CFDictionaryGetValue(attachments, CFSTR("SCStreamFrameStatus")));
            
            if ([statusNumber longValue] == SCFrameStatusComplete || [statusNumber longValue] == SCFrameStatusIdle){
                CVPixelBufferRef pbuf = CMSampleBufferGetImageBuffer(sampleBuffer);
                CVPixelBufferLockBaseAddress(pbuf, kCVPixelBufferLock_ReadOnly);
                
                /*
                self.pbuf = pbuf;
                self.pbufW = CVPixelBufferGetWidth(self.pbuf);
                self.pbufH = CVPixelBufferGetHeight(self.pbuf);
                self.surface = (__bridge IOSurface *)CVPixelBufferGetIOSurface(pbuf);

                

                uint32_t seed = 123;
                [self.surface lockWithOptions:kIOSurfaceLockReadOnly seed:&seed];
                
                void* sdata = [self.surface baseAddress];
                size_t sdataSize = [self.surface allocationSize];
                con->sendDataFragmented(sdata, sdataSize);
                */
                /*
               NSData* data = [NSData dataWithBytes:CVPixelBufferGetBaseAddress(pbuf) length:CVPixelBufferGetDataSize(pbuf)];

                NSString* str = [[NSString alloc] initWithData:data encoding: NSUTF8StringEncoding];
                
                con->sendDataFragmented((void*)CFBridgingRetain(str), [str length]);
                 */
                
                con->sendDataFragmented(CVPixelBufferGetBaseAddress(pbuf), CVPixelBufferGetDataSize(pbuf));
                //CVPixelBufferUnlockBaseAddress(pbuf, kCVPixelBufferLock_ReadOnly);

            }
            else{
                NSLog(@"WHATTT");
                NSLog(@"%ld",(long)CFDictionaryGetValue(attachments, CFSTR("SCStreamFrameStatus")));
            }
            
        }
        
    }
    
}
- (IBAction)start:(id)sender{
    
    __block SCContentFilter* filter;
    dispatch_semaphore_t sem = dispatch_semaphore_create(0);
    [SCShareableContent getShareableContentWithCompletionHandler: ^(SCShareableContent * shareableContent, NSError * err){
        if (err){
            NSLog(@"%@", err);
        }
        else{
             filter = [[SCContentFilter alloc] initWithDisplay: shareableContent.displays[0]
                                         excludingApplications: @[] exceptingWindows:@[]];
        }
        dispatch_semaphore_signal(sem);
    }];
    
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);

    //stream config
    SCStreamConfiguration* config = [SCStreamConfiguration new];//[SCStreamConfiguration streamConfigurationWithPreset: SCStreamConfigurationPresetCaptureHDRStreamLocalDisplay];
    config.width = w*1; //CGDisplayBounds(CGMainDisplayID()).size.width;
    config.height = h*1; //CGDisplayBounds(CGMainDisplayID()).size.height;
    config.queueDepth = 6;
    //config.pixelFormat = 1380411457;
    //config.pixelFormat = kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange;
    config.pixelFormat = kCVPixelFormatType_64RGBAHalf;
    
    self.stream = [[SCStream alloc] initWithFilter: filter configuration: config delegate:self];
    NSError* error = nil;
    
    [self.stream addStreamOutput: self type: SCStreamOutputTypeScreen sampleHandlerQueue: nil error: &error];
    if(error){
        NSLog(@"%@", error.localizedDescription);
    }
    
    [self.stream addStreamOutput: self type: SCStreamOutputTypeAudio sampleHandlerQueue: nil error: &error];
    if(error){
        NSLog(@"%@", error.localizedDescription);
    }
    [self.stream addStreamOutput: self type: SCStreamOutputTypeMicrophone sampleHandlerQueue: nil error: &error];
    if(error){
        NSLog(@"%@", error.localizedDescription);
    }
     
    [self.stream startCaptureWithCompletionHandler:^(NSError* err){
        if (err){
            NSLog(@"%@", err.localizedDescription);
        }
        NSLog(@"bad days are over");
        }];
    
        self.monitorWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(200, 200, 400, 300)
                                                          styleMask:(NSWindowStyleMaskTitled |
                                                                     NSWindowStyleMaskClosable |
                                                                     NSWindowStyleMaskResizable |
                                                                     NSWindowStyleMaskMiniaturizable)
                                                            backing:NSBackingStoreBuffered
                                                              defer:NO];
        [self.monitorWindow setTitle:@"Another Window"];
        //[self.monitorWindow setContentViewController:self];
        [self.monitorWindow makeKeyAndOrderFront:nil];
}

@end
    
