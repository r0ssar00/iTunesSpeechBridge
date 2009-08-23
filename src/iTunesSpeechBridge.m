#import <Foundation/Foundation.h>
#import "../include/iTunes.h"
#define RCF_USE_BOOST_ASIO

#import "RCF/RCF.hpp"

int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    NSLog(@"Hello, World!");
	iTunesApplication * iTunes = [SBApplication applicationWithBundleIdentifier:@"com.apple.iTunes"];
	NSLog(@"Current song is %@", [[iTunes currentTrack] name]);
    [pool drain];
    return 0;
}
