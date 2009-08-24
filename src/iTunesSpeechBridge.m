#import <Foundation/Foundation.h>
#import "../include/iTunes.h"
#import "IPC.h"
#import <boost/tuple/tuple.hpp>
#import <vector>
#import <string>
#import <map>
#import <iostream>
#import <algorithm>
using namespace std;
using namespace boost;
typedef vector<string> str_list;
typedef vector<tuple<string, string, string> > track_list; // contains name, id, genre
typedef map<string, map<string, track_list> > artist_album_track_map;
// maps a string to a list of maps: artist->list of albums
// each map contains a map from a string to a list of pairs of strings,int: album->list of tracks->id
// library["Some Artist"] gets an artist
// library["Some Artist"]["Some Album"] gets an album, album["Some Album"][0] gets one track
// track->second() gets the id used for itunes
typedef map<string, str_list> track_artist_map;
typedef map<string, map<string, str_list> > track_artist_album_map;
typedef map<string, tuple<string, string, string> >id_track_artist_album_map;
typedef map<string, vector<string> > genre_id_map;
str_list tracks;
str_list artists;
NSMutableArray *it_tracks;
artist_album_track_map library;
track_artist_map artists_for_track;
track_artist_album_map artists_for_track_with_album;
id_track_artist_album_map id_for_track_with_artist_album;
genre_id_map ids_for_genre;
str_list playlists;
str_list albums;
str_list genres;
void sort_vector(str_list &list_) {
	sort(list_.begin(), list_.end());
	str_list::iterator new_end_pos = unique( list_.begin(), list_.end());
	list_.erase( new_end_pos, list_.end());
}
int main(int argc, const char *argv[]) {
	NSAutoreleasePool *pool = [[ NSAutoreleasePool alloc ] init ];

	// insert code here...
	NSLog(@ "Hello, World!");
	iTunesApplication *iTunes = [ SBApplication applicationWithBundleIdentifier:@ "com.apple.iTunes" ];
	tuple<string, string, string> track;
	iTunesPlaylist *it_music = [[[[ iTunes sources ] objectWithName:@ "Library" ] playlists ] objectWithName:@ "Music" ];
	string artist_s, album_s, track_s, id_s, genre_s;
	for (iTunesTrack *itt in[ it_music tracks ]) {
		artist_s = [[ itt artist ] UTF8String ];
		album_s = [[ itt album ] UTF8String ];
		track_s = [[ itt name ] UTF8String ];
		genre_s = [[ itt genre ] UTF8String ];
		id_s = [[ itt persistentID ] UTF8String ];
		track = tuple<string, string, string>(track_s, id_s, genre_s);
		library[ artist_s ][ album_s ].push_back(track);  // maintain artist-album-track relation
		artists_for_track[ track_s ].push_back(artist_s);  // make it easy to find artists that have a given track
		artists_for_track_with_album[ track_s ][ artist_s ].push_back(album_s); // find artists for track with an album
		id_for_track_with_artist_album[ id_s ] = tuple<string, string, string>(track_s, artist_s, album_s); // find track details given id
		ids_for_genre[ genre_s ].push_back(id_s); // find tracks by id for a genre
		sort_vector(artists_for_track[ track_s ]);
		sort_vector(artists_for_track_with_album[ track_s ][ artist_s ]);
		sort_vector(ids_for_genre[ genre_s ]);
		tracks.push_back(track_s);
		albums.push_back(album_s);
		artists.push_back(artist_s);
		genres.push_back(genre_s);
	}
	sort_vector(tracks);
	sort_vector(albums);
	sort_vector(artists);
	sort_vector(genres);
	// all data acquired, respond to requests
	[ pool drain ];
	return 0;
}
