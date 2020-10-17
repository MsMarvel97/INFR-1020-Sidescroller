#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "BackEnd.h"
#include <iostream>
#include <fstream>
using namespace std;


enum AnimEnums
{
	IDLELEFT,
	IDLERIGHT,

	//Only in Top down
#ifdef TOPDOWN
	IDLEUP,
	IDLEDOWN,
#endif

	WALKLEFT,
	WALKRIGHT,

	//Only in Top down
#ifdef TOPDOWN
	WALKUP,
	WALKDOWN,
#endif

	ATTACKLEFT,
	ATTACKRIGHT,

	//Only in Top down
#ifdef TOPDOWN
	ATTACKUP,
	ATTACKDOWN
#endif
};

enum AnimTypes
{
#ifdef TOPDOWN
	IDLE = 0,
	WALK = 4,
	ATTACK = 8
#endif
#ifndef TOPDOWN
	IDLE = 0,
	WALK = 2,
	ATTACK = 4
#endif
};

enum AnimDir
{
	LEFT,
	RIGHT,
	//Only in Top Down
#ifdef TOPDOWN
	UP,
	DOWN
#endif
};

class Player
{
public:
	Player();
	Player(std::string& fileName, std::string& animationJSON, int width, int height,
		Sprite* sprite, AnimationController* controller, Transform* transform, bool hasPhys = false, PhysicsBody* body = nullptr);

	void InitPlayer(std::string& fileName, std::string& animationJSON, int width, int height,
		Sprite* sprite, AnimationController* controller, Transform* transform, bool hasPhys = false, PhysicsBody* body = nullptr);

	void Update();
	void MovementUpdate();
	void AnimationUpdate();

private:
	void SetActiveAnimation(int anim);

	//Basically, any animation OTHER than moving will not have a cancel, and we'll be checking whether or not that animation is done
	bool m_moving = false;
	//Are you currently attacking?????
	bool m_attacking = false;
	//Have we locked the player from moving during this animation?
	bool m_locked = false;

	//checks if player is moving
	bool boolMove = false;
	//flag to check if player is allowed to jump
	bool isJumping = false;
	//float to track how long player has been jumping
	float jumpTime = 0;
	//flag to determine type of movement function player will be using
	int moveToggle = 0;

	//float to track the time on Timer::time at the moment the stopwatch starts
	float startTime = 0;
	//float to track the current time on the stopwatch
	float swNow = 0;
	//float to track the start of the stopwatch function for the second move function
	float move2Time = 0;
	//float for tracking 
	float letsAGo = 0;

	//timer to start during the jump
	float jumpStart = 0;
	//boolean to track whether players can jump
	bool timeToJump = false;

	//float to track graphing intervals
	float graphCounter = 0;


	//vector to hold velocity
	vec3 vel = vec3(0.f, 0.f, 0.f);
	//vector to hold the second version of acceleration for direct movement controls
	vec3 displacing = vec3(1.0, 0.f, 0.f);
	//vector to hold displacement for direct movement controls
	vec3 displacement = vec3(0.f, 0.f, 0.f);
	//vector to hold new velocity for direct movement controls
	vec3 imThere = vec3(0.f, 0.f, 0.f);


	//A reference to our sprite
	Sprite* m_sprite = nullptr;
	//A reference to our animation controller
	AnimationController* m_animController = nullptr;
	//A reference to our player transform
	Transform* m_transform = nullptr;

	//Physics importance
	//A reference to our physics body
	PhysicsBody* m_physBody = nullptr;
	//Does this player have physics?
	bool m_hasPhysics = true;

	//Default animation direction (feel free to change this to suit your game. If you're making a side-scroller, left or right would be better
	AnimDir m_facing = LEFT;
};

#endif // !__PLAYER_H__