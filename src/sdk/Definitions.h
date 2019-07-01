#pragma once

#define MULTIPLAYER_BACKUP 300

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.
#define M_PHI		1.61803398874989484820 // golden ratio

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

#define TICK_INTERVAL			(globalVars.intervalPerTick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )


// random Source engine things that don't need their own file.

#define SIGNONSTATE_NONE		0	// no state yet, about to connect
#define SIGNONSTATE_CHALLENGE	1	// client challenging server, all OOB packets
#define SIGNONSTATE_CONNECTED	2	// client is connected to server, netchans ready
#define SIGNONSTATE_NEW			3	// just got serverinfo and string tables
#define SIGNONSTATE_PRESPAWN	4	// received signon buffers
#define SIGNONSTATE_SPAWN		5	// ready to receive entity packets
#define SIGNONSTATE_FULL		6	// we are fully connected, first non-delta packet received
#define SIGNONSTATE_CHANGELEVEL	7	// server is changing level, please wait
#define SIGNONSTATE_INGAMEAPEX  8   // In Game Apex ( Note: The Lobby is literally a server )


#define IN_ATTACK		(1 << 0) // 1
#define IN_JUMP			(1 << 1) // 2
#define IN_DUCK			(1 << 2) // 4
#define IN_FORWARD		(1 << 3) // 8
#define IN_BACK			(1 << 4) // 16
#define IN_USE			(1 << 5) // 32
#define IN_CANCEL		(1 << 6) // 64
#define IN_LEFT			(1 << 7) // 128
#define IN_RIGHT		(1 << 8) // 256
#define IN_MOVELEFT		(1 << 9) // 512
#define IN_MOVERIGHT	(1 << 10) // 1024
#define IN_UNKNOWN		(1 << 11) // 2048
#define IN_RELOAD		(1 << 12) // 4096
#define IN_SWAPGUN		(1 << 13) // 8k // Used with IN_USE to pick stuff up
#define IN_UNKNOWN2		(1 << 14) // 16k
#define IN_SPRINT		(1 << 15) // 32k
#define IN_ZOOM 		(1 << 16) // 64k // Right click
/*
#define IN_SPEED		(1 << 17) // 128k	// Player is holding the speed key
#define IN_WALK			(1 << 18)	// Player holding walk key
#define IN_ZOOM			(1 << 19)	// Zoom key for HUD zoom
#define IN_WEAPON1		(1 << 20)	// weapon defines these bits
#define IN_WEAPON2		(1 << 21)	// weapon defines these bits
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)	// grenade 1
#define IN_GRENADE2		(1 << 24)	// grenade 2
*/

#define FL_ONGROUND				(1<<0)	// At rest / on the ground
/*
#define FL_DUCKING				(1<<1)	// Player flag -- Player is fully crouched
#define FL_WATERJUMP			(1<<2)	// player jumping out of water
#define FL_ONTRAIN				(1<<3)	// Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_INRAIN				(1<<4)	// Indicates the entity is standing in rain
#define FL_FROZEN				(1<<5)	// Player is frozen for 3rd person camera
#define FL_ATCONTROLS			(1<<6)	// Player can't move, but keeps key inputs for controlling another entity
#define FL_CLIENT				(1<<7)	// Is a player
#define FL_FAKECLIENT			(1<<8)	// Fake client, simulated server side; don't send network messages to them
// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define FL_INWATER				(1<<9)	// In water
*/