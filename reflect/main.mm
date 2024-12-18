//
//  main.m
//  sctest
//
//  Created by Dora Orak on 11.12.2024.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"
int main(int argc, const char * argv[]) {

    AppDelegate *delegate = [[AppDelegate alloc] init];
                NSApplication.sharedApplication.delegate = delegate;
        return NSApplicationMain(argc, argv);
}
