/*
 *  IPC.h
 *  iTunesSpeechBridge
 *
 *  Created by Kevin Ross on 23/08/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */
#include <RCF/Idl.hpp>
#include <RCF/UnixLocalEndpoint.hpp>
#include <SF/vector.hpp>

RCF_BEGIN(I_ITSB, "I_ITSB")
	// void play_playlist(string name);
	RCF_METHOD_V1(void, play_playlist, std::string)
	// vector<string> get_playlists();
	RCF_METHOD_R0(std::vector<std::string>, get_playlists)
	// void play_artist(string artist);
	RCF_METHOD_V1(void, play_artist, std::string)
	// vector<string> get_artists();
	RCF_METHOD_R0(std::vector<std::string>, get_artists)
	// void play_genre(string genre);
	RCF_METHOD_V1(void, play_genre, std::string)
	// vector<string> get_genres();
	RCF_METHOD_R0(std::vector<std::string>, get_genres)
	// void play_album(string album);
	// void play_album_by_artist(string album, string artist);
	RCF_METHOD_V1(void, play_album, std::string)
	RCF_METHOD_V2(void, play_album_by_artist, std::string, std::string)
	// vector<string> get_albums();
	RCF_METHOD_R0(std::vector<std::string>, get_albums)
	// void play_track_by_artist(string track, string artist)
	// void play_track_by_artist_from_album(string track, string artist, string album)
	RCF_METHOD_V2(void, play_track_by_artist, std::string, std::string)
	RCF_METHOD_V3(void, play_track_by_artist_from_album, std::string, std::string, std::string)
	// vector<string> get_tracks();
	RCF_METHOD_R0(std::vector<std::string>, get_tracks)
	// vector<string> get_artists_for_track(string track);
	// vector<string> get_albums_for_artist(string artist);
	// vector<string> get_albums_for_artist_with_track(string artist, string track);
RCF_END(I_ITSB)