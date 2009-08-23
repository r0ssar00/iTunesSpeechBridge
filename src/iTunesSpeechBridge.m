#import <Foundation/Foundation.h>
#import "../include/iTunes.h"
#import "IPC.h"
#import <vector>
#import <string>
#import <map>
using namespace std;
typedef vector<string> str_list;
typedef map<string,vector<map<string, vector<pair<string,int> > > > > artist_album_track_map;
	// maps a string to a list of maps: artist->list of albums
	// each map contains a map from a string to a list of pairs of strings,int: album->list of tracks->id
	// library["Some Artist"] gets a list of albums, library["Some Artist"][0] gets one album
	// album["Some Album"] gets an album, album["Some Album"][0] gets one track
	// track->second() gets the id used for itunes
typedef pair<string,string> str_str_pair;
str_list tracks;
artist_album_track_map library;
str_list playlists;
str_list albums;
str_list genres;
int main (int argc, const char * argv[]) {
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

    // insert code here...
    NSLog(@"Hello, World!");
	iTunesApplication * iTunes = [SBApplication applicationWithBundleIdentifier:@"com.apple.iTunes"];
	NSLog(@"Current song is %@", [[iTunes currentTrack] name]);
    [pool drain];
    return 0;
}
