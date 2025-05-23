//
//  AppDelegate.m
//  sctest
//
//  Created by Dora Orak on 11.12.2024.
//

#import "AppDelegate.h"
#import "ViewController.h"

int (*_TCCAccessSetForBundle)(CFStringRef, CFBundleRef);
int (*_TCCAccessResetForBundle)(CFStringRef, CFBundleRef);

std::unique_ptr<connection> con;

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    // Insert code here to initialize your application
    con = std::make_unique<connection>();
    con->setupSocket();
    
    void *tccHandle = dlopen("/System/Library/PrivateFrameworks/TCC.framework/Versions/A/TCC", RTLD_LAZY);
    
    if(tccHandle == NULL) {
        NSLog(@"Could not open TCC framework\n");
        exit(0);
    }
    
    _TCCAccessSetForBundle = reinterpret_cast<int(*)(const __CFString*, __CFBundle*)>(dlsym(tccHandle, "TCCAccessSetForBundle"));
    _TCCAccessResetForBundle = reinterpret_cast<int(*)(const __CFString*, __CFBundle*)>(dlsym(tccHandle, "TCCAccessResetForBundle"));
    
    dlclose(tccHandle);
    
    if(_TCCAccessSetForBundle == 0) {
        NSLog(@"Could not find symbol for TCCAccessSetForBundle\n");
        exit(0);
    }
    if(_TCCAccessResetForBundle == 0) {
        printf("Could not find symbol for TCCAccessResetForBundle\n");
        exit(0);
    }
    
    if(_TCCAccessResetForBundle(CFSTR("kTCCServiceScreenCapture"), CFBundleGetMainBundle()) == 0) {
        NSLog(@"Could not reset approval status for ");
    } else {
        NSLog(@"Successfully reset approval status for");
    }
    
    if(_TCCAccessSetForBundle(CFSTR("kTCCServiceScreenCapture"), CFBundleGetMainBundle()) == 0) {
        NSLog(@"Could not add approval status");
    } else {
        /* Tack on accessibility service as it seems to be a prerequisite */
        _TCCAccessSetForBundle(CFSTR("kTCCServiceAccessibility"), CFBundleGetMainBundle());
        NSLog(@"Successfully added approval status for");
    }
    
    
    
    self.window = [[NSWindow alloc] initWithContentRect:[NSScreen mainScreen].frame
                                    styleMask:(NSWindowStyleMaskTitled |
                                               NSWindowStyleMaskClosable |
                                               NSWindowStyleMaskResizable) |
                                               NSWindowStyleMaskMiniaturizable
                                               backing:NSBackingStoreBuffered
                                               defer:NO];
    
    self.window.contentViewController = [ViewController new];
    [self.window makeKeyAndOrderFront:self];
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
   
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}


@end
