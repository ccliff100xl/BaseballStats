#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "Play.h"

using namespace std;

Play::Play(string line_raw_) : _line_raw(line_raw_) 
{
	//Parse line and add to members (need to delete defaults in header)
	vector<string> line_parsed;
	boost::split(line_parsed, line_raw_, boost::is_any_of(","));
	_inning = stoi(line_parsed[1]);
	_team = (TeamType) stoi(line_parsed[2]);
	_player_id = line_parsed[3];
	//If unknown, this is ??
	if (!(boost::equal(line_parsed[4], "??"))){
		const string count_string = line_parsed[4];
		_count[0] = count_string.at(0) - '0';
		_count[1] = count_string.at(1) - '0';
	} 
	//May be empty, OK
	_pitch_details = line_parsed[5];
	//Store raw play details string
	_event_raw = line_parsed[6];
}

//EXPLANATION OF PLAY LINE FROM : http://www.retrosheet.org/datause.txt 4/15/16
//play      The play records contain the events of the game.Each
//play record has 7 fields.
//
//1. The first field is the inning.
//
//2. The second field is either 0 (for visiting team) or 1
//(for home team).
//
//3. The third field is the Retrosheet ID code.
//
//4. The fourth field is the count on the batter when this
//particular event(play) occurred.Most Retrosheet
//games do not have this information, and in such cases,
//"??" appears in this field.
//
//5. The fifth field is of variable length and contains all
//pitches to this batter in this plate appearance.The
//standard pitches are : C for called strike, S for
//swinging strike, B for ball, F for foul ball.In
//addition, pickoff throws are indicated by the number of
//the base the throw went to.For example, "1" means the
//pitcher made a throw to first, "2" a throw to second,
//etc.If the base number is preceded by a "+" sign, the
//pickoff throw was made by the catcher.Some of the less
//common pitch codes are L : foul bunt, M : missed bunt,
//Q : swinging strike on a pitchout, R : foul ball on a pitchout,
//	I : intentional ball, P : pitchout, H : hit by pitch, K : strike of
//	unknown type, U : unkown or missing pitch.Most Retrosheet
//	games do not have pitch data and consequnetly this field
//	is blank for such games.
//
//	There is occasionally more than one event for each
//	plate appearance, such as stolen bases, wild pitches,
//	and balks in which the same batter remains at the
//	plate.On these occasions the pitch sequence is
//	interrupted by a period, and there is another play
//	record for the resumption of the batter's plate  
//	appearance.
//
//	6. The sixth field describes the play which occurred.
//	This field is variable in length and has three main
//	portions which follow the Retrosheet scoring
//	system.The scoring procedure description also
//	contains a diagram that explains clearly how each area
//	of the playing field is designated.  (The diagram is
//		available under Docs, but most Retrosheet game
//		accounts do not contain detailed location codes.)
//
//	a.The first portion is a description of the basic
//	play, following standard baseball scoring
//	notation.For example, a fly ball to center field
//	is "8", a ground ball to second is "43", etc.
//	Base hits are abbreviated with a letter(S for
//		singles, D for doubles, T for triples, H for home
//		runs) and (usually)a number identifying the
//	fielder who played the ball.Therefore "S7" is a
//	single fielded by the left fielder.
//
//	b.The second portion is a modifier of the first part
//	and is separated from it with a forward slash,
//	"/".In fact, there may be more than one second
//	portion.Typical examples are hit locations.For
//	example, "D8/78" indicates a double fielded by the
//	center fielder on a ball hit to left center.
//	Other possible second portion modifiers are "SH"
//	for sacrifice hits, GDP for grounding into double
//		plays, etc.
//
//		c.The third portion describes the advancement of any
//		runners, separated from the earlier parts by a
//		period.For example, "S9/L9S.2-H;1-3"  should be
//		read as : single fielded by the right fielder, line
//		drive  to short right field.The runner on 2nd
//		scored(advanced to home), and the runner on first
//		advanced to third.Note that any advances after
//		the first are separated by semicolons.