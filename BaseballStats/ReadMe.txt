========================================================================
    CONSOLE APPLICATION : BaseballStats Project Overview
========================================================================

AppWizard has created this BaseballStats application for you.

This file contains a summary of what you will find in each of the files that
make up your BaseballStats application.


BaseballStats.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

BaseballStats.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

BaseballStats.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named BaseballStats.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////

Implementation Notes:


//I fixed a few files that had HR/#, by deleting the /#. But, that is actually valid.
//It will print a warning as not recognized, but not cause an error

//Fixed 2/12/18 File 948 1983SEA.EVA
//Need to handle CSH(13E2)(UR).3-H;1-2 in the special caught stealing section.
//Could not handle adjacent ()()

//Issue Fixed 2/11/18: Need to handle this play from 1997FLO.EVN
//play, 1, 0, lockk001, 22, FCBB1, POCS2(134); CSH(42) / DP
//Cannot handle picked off caught stealing and caught stealing in the same row.
//Combined handling of caught stealing, picked off, and picked off caught stealing
//int he same section of Event()

//Issue on 8/3/17, fixed in GameState::updateBaserunners by comparind the positions that
//made the out to the positions that made the error
// Stuck on this play: play,4,1,lambj001,01,CX,5/P5F/NDP/SF.3-H;2XH(26)(E5/TH);1-2
// in 2015ARI.EVN. Two outs are made on the play, but I an not counting the out E5
// because I'm saying it's an error and not an out.  I think there was a fly ball to 3rd, 
// he threw home (badly, an error) so the runner from 2nd had a chance to score but 
// was out at the plate.  So, after 2XH if there is an error, but also an additional play
// it will turn the play back into an out


//
// Complicated Issues Encountered
//
//1. DH moves to field, so pitcher needs to be deleted from batting position 0
//   Case in the top of the 8th here: http://www.baseball-reference.com/boxes/OAK/OAK201604070.shtml
//
//2. Trailing / with no modifier following in play.  This is an error in the log, but I deal with it
//   See play,4,1,beltb001,11,SBX,7/F/ in 2016 Giants
//
//3. Single ? for count when unknown, should be ?? (see play,6,0,koenm101,?,,9 in 1927WS1.EVA)
//
//4. Nothing for count when unknown, should be ?? (See play,1,0,adams101,,,43 in 1930BRO.EVN)
//
//5. Missing version, 1943PHA.EVA, just set to -1
//
//6. Comma inside quotes, I deleted comment IN THE FILES 1944SLN.EVN, 1949NY1.EVN
//
// 7. 'b' instead of 1 or 0 for inning, 1963KC1.EVA, play,5,b,gardb101,??,,NP (b changed to 0 in file)