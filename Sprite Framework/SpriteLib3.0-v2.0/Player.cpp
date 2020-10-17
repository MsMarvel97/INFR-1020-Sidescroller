#include "Player.h"


Player::Player()
{
}

Player::Player(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite, AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	InitPlayer(fileName, animationJSON, width, height, sprite, controller, transform, hasPhys, body);
}

void Player::InitPlayer(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite, AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{

	//store references to the components
	m_sprite = sprite;
	m_animController = controller;
	m_transform = transform;
	m_hasPhysics = hasPhys;
	if (hasPhys)
	{
		m_physBody = body;
	}


	//initializes UVs
	m_animController->InitUVs(fileName);

	//loads the texture and sets width and height
	m_sprite->LoadSprite(fileName, width, height, true, m_animController);
	m_animController->SetVAO(m_sprite->GetVAO());
	m_animController->SetTextureSize(m_sprite->GetTextureSize());

	//loads in the animation JSON file
	nlohmann::json animations = File::LoadJSON(animationJSON);

	//Idle animations\\

	//Idle left
	m_animController->AddAnimation(animations["IdleLeft"].get<Animation>());

	//Idle Right
	m_animController->AddAnimation(animations["IdleRight"].get<Animation>());
#ifdef TOPDOWN
	//Idle Up
	m_animController->AddAnimation(animations["IdleUp"].get<Animation>());

	//Idle Down
	m_animController->AddAnimation(animations["IdleDown"].get<Animation>());
#endif


	//Walk Animations\\

	//Walk left
	m_animController->AddAnimation(animations["WalkLeft"].get<Animation>());

	//Walk right
	m_animController->AddAnimation(animations["WalkRight"].get<Animation>());

#ifdef TOPDOWN
	//Walk up
	m_animController->AddAnimation(animations["WalkUp"].get<Animation>());

	//Walk down
	m_animController->AddAnimation(animations["WalkDown"].get<Animation>());
#endif


	//Attack Animations\\

	//Attack left
	m_animController->AddAnimation(animations["AttackLeft"].get<Animation>());

	//Attack right
	m_animController->AddAnimation(animations["AttackRight"].get<Animation>());

#ifdef TOPDOWN
	//Attack up
	m_animController->AddAnimation(animations["AttackUp"].get<Animation>());

	//Attack down
	m_animController->AddAnimation(animations["AttackDown"].get<Animation>());
#endif

	//Set Default Animation
	m_animController->SetActiveAnim(IDLELEFT);
}

//updating player movement and animations
void Player::Update()
{
	//if the player isn't considered locked due to their current animation, this will update their movement
	if (!m_locked)
	{
		MovementUpdate();
	}
	//updating player sprite animations
	AnimationUpdate();
}

//this function gets the time elapsed since the action that called this function
float StopWatch(float startTime)
{
	float swNow = Timer::time - startTime;
	return swNow;
}

//updating player movement
void Player::MovementUpdate()
{
	//getting player's physicsBody
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	//creating and initializing a vector for velocity
	vec3 vel = vec3(0.f, 0.f, 0.f);
	//creating a float to store a value for accelerating
	float speed = 2.8f;
	//setting moving to false to show movement hasn't started yet
	m_moving = false;

	//pressing alt toggles between the two forms of movement. By default the movement toggle is set to physics-based movement
	if (Input::GetKeyDown(Key::Alt))
	{
		if (moveToggle == 0)
		{
			moveToggle = 1;
		}
		else
		{
			moveToggle = 0;
		}

	}

	//this is the default physics-based movement
	if (moveToggle == 0)
	{
		//checking that the body has physics before applying movement
		if (m_hasPhysics)
		{
			//if "Shift" is being held, speed will be scaled up to represent running
			if (Input::GetKey(Key::Shift))
			{
				speed *= 1.5f;
			}

			//if "A" is being held, player will move left with physics-based movement
			if (Input::GetKey(Key::A))
			{
				//creating a vector for acceleration
				vec3 accelerationA = vec3(-1.f, 0.f, 0.f);
				//creating a vector to store the new velocity for the player
				vec3 acceleratedA = vec3();

				//starting a stopwatch to time this movement for graphing purposes
				if (startTime == 0)
				{
					//setting the start time of the stopwatch to the current value of timer
					startTime = Timer::time;
				}

				//sending the start time of the stopwatch 
				float timer = StopWatch(startTime);

				//if the "Shift" key is being held and the player has not reached peak sprint velocity, this will continue to accelerate them
				if (Input::GetKey(Key::Shift) && m_physBody->GetVelocity().x > -90.f)
				{
					//determining the new velocity of the player by taking the current velocity and adding acceleration multiplied by the fixed
					//value of speed to it
					acceleratedA = player.GetVelocity() + accelerationA * speed;
					//setting player velocity to the accelerated velocity
					player.SetVelocity(acceleratedA);
				}

				//if the "Shift" key is being held and the player has reached the peak sprint velocity this will keep them at that speed
				if (Input::GetKey(Key::Shift) && m_physBody->GetVelocity().x <= -90.f)
				{
					//setting player velocity to max sprint speed
					player.SetVelocity(vec3(-90.f, 0.f, 0.f));
				}

				//if the player is not holding "Shift" and they have not reached the peak walk velocity, this will accelerate them
				if (m_physBody->GetVelocity().x > -60.f && !Input::GetKey(Key::Shift))
				{
					//determining the new velocity of the player by taking the current velocity and adding acceleration multiplied by the fixed
					//value of speed to it
					acceleratedA = player.GetVelocity() + accelerationA * speed;
					//setting player velocity to the accelerated velocity
					player.SetVelocity(acceleratedA);
				}

				//if the player is not holding "Shift" and they have reached the peak walk velocity, this will keep them moving at max walk speed
				if (m_physBody->GetVelocity().x <= -60.f && !Input::GetKey(Key::Shift))
				{
					//keeping the player moving at the maximum walking speed
					player.SetVelocity(vec3(-60.f, 0.f, 0.f));
				}

				//every ten frames (starting with frame 1) the time elapsed and player velocity will be output to the console window for graphing
				//this counter exists to prevent flooding the console window with frame by frame informatiom
				if (graphCounter == 0)
				{
					//outputting information to the console window so it can be graphed
					printf("%f,%f \n", timer, player.GetVelocity().x);
					//increasing the graphCounter
					graphCounter++;
				}

				//if this is a frame between 2 and 9 of the current interval, the counter will continue to be increased
				else if (graphCounter >= 1 && graphCounter < 10)
				{
					//increasing the graphCounter
					graphCounter += 1;
				}

				//if this is the last frame of an interval, the counter will be reset to 0
				else if (graphCounter == 10)
				{
					//setting graph counter to 0
					graphCounter = 0;
				}

				//setting the player to face left for animations
				m_facing = LEFT;
				//setting movement to true while player is moving
				m_moving = true;
			}

			//when the player releases the "A" key, this will decelerate them. It also checks that the player is not currently facing right to prevent it from
			//triggering during deceleration from movement to the right.
			else if (m_physBody->GetVelocity().x < 0.f && !m_facing == RIGHT)
			{
				//creating a vector to handle deceleration
				vec3 deceleration = vec3(1.f, 0.f, 0.f);

				//creating a vector to hold velocity
				vel = vel + vec3(1.f, 0.f, 0.f);

				vec3 decelerated /*= vec3(vel * speed + deceleration)*/;

				//updating the stopwatch
				float timer = StopWatch(startTime);

				//determining the new velocity of the player by taking the current velocity and adding acceleration multiplied by the time
				//between frames
				decelerated = m_physBody->GetVelocity() + vel * Timer::deltaTime;
				//setting player velocity to the new velocity
				m_physBody->SetVelocity(decelerated);

				//printing timer and player velocity information to console for graphing
				printf("%f,%f \n", timer, player.GetVelocity().x);

				//seting player facing to left for animations
				m_facing = LEFT;
				//setting moving to true to indicate the player is currently moving
				m_moving = true;
				/*boolMove = true;*/
			}

			//resetting the timer when the player hits "T"
			if (Input::GetKeyDown(Key::T))//Reset Timer
			{
				//setting startTime to 0
				startTime = 0;
			}

			//if "D" is being held, player will move right with physics-based movement
			if (Input::GetKey(Key::D))
			{
				//setting a vector for player acceleration
				vec3 accelerationD = vec3(1.f, 0.f, 0.f);
				//setting a vector to hold the new velocity of the player
				vec3 acceleratedD = vec3(0.f, 0.f, 0.f);

				//starting the stopwatch if startTime has not been set
				if (startTime == 0)
				{
					startTime = Timer::time;
				}

				//updating the stopwatch
				float timer = StopWatch(startTime);

				//if the "Shift" key is being held and the player has not reached peak sprint velocity, this will continue to accelerate them
				if (Input::GetKey(Key::Shift) && m_physBody->GetVelocity().x <= 90.f)
				{
					//determining the new velocity of the player by taking the current velocity and adding acceleration multiplied by the fixed
					//value of speed to it
					acceleratedD = m_physBody->GetVelocity() + accelerationD * speed;
					//setting player velocity to the new velocity
					m_physBody->SetVelocity(acceleratedD);
				}

				//if the "Shift" key is being held and the player has reached the peak sprint velocity this will keep them at that speed
				if (Input::GetKey(Key::Shift) && m_physBody->GetVelocity().x >= 90.f)
				{
					//setting player velocity to max sprint speed
					player.SetVelocity(vec3(90.f, 0.f, 0.f));
				}

				//if the player is not holding "Shift" and they have not reached the peak walk velocity, this will accelerate them					
				if (m_physBody->GetVelocity().x <= 60.f && !Input::GetKey(Key::Shift))
				{
					//determining the new velocity of the player by taking the current velocity and adding acceleration multiplied by the fixed
					//value of speed to it
					acceleratedD = m_physBody->GetVelocity() + accelerationD * speed;
					//setting player velocity to the new velocity
					m_physBody->SetVelocity(acceleratedD);
				}

				//if the "Shift" key is not being held and the player has reached the peak walk velocity this will keep them at that speed
				if (m_physBody->GetVelocity().x >= 60.f && !Input::GetKey(Key::Shift))
				{
					//setting player velocity to max walk speed
					player.SetVelocity(vec3(60.f, 0.f, 0.f));
				}

				//every ten frames (starting with frame 1) the time elapsed and player velocity will be output to the console window for graphing
				//this counter exists to prevent flooding the console window with frame by frame informatiom
				if (graphCounter == 0)
				{
					//printing timer and player velocity information to console for graphing
					printf("%f,%f \n", timer, player.GetVelocity().x);
					//increasing graphCounter by 1
					graphCounter++;
				}

				//if this is a frame between 2 and 9 of the current interval, the counter will continue to be increased
				if (graphCounter >= 1 && graphCounter < 10)
				{
					//increasing graphCounter by 1
					graphCounter++;
				}

				//if this is the last frame of an interval, the counter will be reset to 0
				if (graphCounter == 10)
				{
					//resetting graph counter
					graphCounter = 0;
				}

				//setting player facing to right for animations
				m_facing = RIGHT;
				//setting moving to true to indicate the player is currently moving
				m_moving = true;
			}

			//when the player releases the "D" key, this will decelerate them. It also checks that the player is not currently facing left to prevent it from
			//triggering during deceleration from movement to the left.
			else if (m_physBody->GetVelocity().x > 0.f && !m_facing == LEFT)
			{
				//setting a vector for deceleration
				vec3 deceleration = vec3(-1.f, 0.f, 0.f);
				//setting a vector for velocity
				vel = vel + vec3(-1.f, 0.f, 0.f);
				//setting a vector to hold the new player velocity
				vec3 decelerated /*= vec3(vel * speed + deceleration)*/;

				//starting a stopwatch if startTime is at 0
				if (startTime == 0)
				{
					//setting a new value for startTime
					startTime = Timer::time;
				}

				//updating the stopwatch
				float timer = StopWatch(startTime);
				//calculating the new velocity for the player by taking the current velocity and adding velocity multiplied by delta time to it
				decelerated = m_physBody->GetVelocity() + vel * Timer::deltaTime;
				//setting player velocity to the new value
				m_physBody->SetVelocity(decelerated);
				//printing timer and player velocity information to the console for graphing purposes
				printf("%f,%f \n", timer, player.GetVelocity().x);

				//setting player facing to right for animations
				m_facing = RIGHT;
				//setting movement to true while player is moving
				m_moving = true;
			}

			//checking if player can jump when they hit spacebar
			if (Input::GetKeyDown(Key::Space))
			{
				//if player is not currently jumping, this will be changed to true and a timer for the jump will begin
				if (timeToJump == false)
				{
					timeToJump = true;
					jumpStart = Timer::time;
				}
			}

			//if player is jumping, this will govern the physics of the jump
			if (timeToJump)
			{
				//updating timer for the jump
				jumpTime = Timer::time - jumpStart;

				//if less than 0.25 seconds have elapsed since the start of the jump then the player will accelerate upwards
				if (jumpTime < 0.25)
				{
					//calculating increased velocity on the positive y-axis using the player's current velocity and an acceleration vector
					vec3 vel = m_physBody->GetVelocity() + vec3(0.f, 10.f, 0.f);
					//setting player velocity to the new value
					m_physBody->SetVelocity(vel);
				}
				//if more than 0.3 seconds have elapsed since the start of the jump, the player will be locked from further jumping
				if (jumpTime > 0.3)
				{
					timeToJump = false;
				}
			}
		}
	}

	//this is the toggle for direct movement controls
	if (moveToggle == 1)
	{
		//if "A" is being held, direct movement will begin going left
		if (Input::GetKey(Key::A))
		{
			if (startTime == 0)
			{
				startTime = Timer::time;
			}

			//updating the stopwatch
			float timer = StopWatch(startTime);

			//if displacement is greater than or equal to 0 and greater than -40, velocity increases
			if (displacement.x >= 0.f && displacement.x > -40.f)
			{
				//increasing displacement by adding a velocity vector to it
				displacement = displacement + vec3(-1.5f, 0.f, 0.f);
				//setting new velocity to current velocity + displacement
				imThere = m_physBody->GetVelocity() + displacement;
				//setting the new velocity to the new velocity
				m_physBody->SetVelocity(imThere);
				/*displaced = imThere.x - currentVel.x;*/

				//printing timer and current velocity to the console for graphing purposes
				printf("%f, %f\n", timer, m_physBody->GetVelocity().x);
				//currentVel = currentVel + imThere;
			}

			//if displacement is less than or equal to -40 and greater than or equal to -80, velocity will increase after a brief lapse
			if (displacement.x <= -40.f && displacement.x > -80.f)
			{
				//setting a new stopwatch start time
				move2Time = Timer::time;

				//updating the current value of the stopwatch
				letsAGo = StopWatch(move2Time);

				//if less than 0.3 seconds have passed since this level of velocity was reached, the current velocity is kept
				if (letsAGo < 0.3)
				{
					m_physBody->SetVelocity(displacement);
				}

				//after the first 0.3 seconds, the velocity will increase to a new threshold
				else
				{
					//adding a new value for acceleration so the growth towards the next threshold will not be affected by the currently elevated velocity
					//and increasing that value by itself multiplied by deltaTime
					displacing = displacing + displacing * Timer::deltaTime;
					//increasing the rate of acceleration by this new value
					displacement = displacement + displacing;
					//setting new velocity to current velocity + displacement
					imThere = m_physBody->GetVelocity() + displacement;
					//setting velocity of player to new velocity
					m_physBody->SetVelocity(displacement);
				}
				//printing time and player velocity information to the console for graphing purposes
				printf("%f, %f\n", timer, m_physBody->GetVelocity().x);
			}

			//if displacement is less than or equal to -80 it will level off as this is the max speed for this type of movement
			if (displacement.x <= -80)
			{
				//setting displacement to the max speed
				displacement = vec3(-80.f, 0.f, 0.f);
				//setting velocity to displacement value
				m_physBody->SetVelocity(displacement);
				//printing time and player velocity information to the console for graphing purposes
				printf("%f, %f\n", timer, m_physBody->GetVelocity().x);
			}

			//setting player facing to left for animation purposes
			m_facing = LEFT;
			//setting moving to true while player is moving
			m_moving = true;
		}

		//if "D" is being held, direct movement will begin going right
		if (Input::GetKey(Key::D))
		{

			//starting the stopwatch if startTime has not been set
			if (startTime == 0)
			{
				startTime = Timer::time;
			}

			//updating the stopwatch
			float timer = StopWatch(startTime);

			//if displacement is less than or equal to 0 and less than 40, velocity increases
			if (displacement.x >= 0.f && displacement.x < 40.f)
			{
				//increasing displacement by adding a velocity vector to it
				displacement = displacement + vec3(1.5f, 0.f, 0.f);
				//setting new velocity to current velocity + displacement
				imThere = m_physBody->GetVelocity() + displacement;
				//setting the new velocity to the new velocity
				m_physBody->SetVelocity(imThere);
				/*displaced = imThere.x - currentVel.x;*/

				//printing timer and current velocity to the console for graphing purposes
				printf("%f, %f\n", timer, m_physBody->GetVelocity().x);
				//currentVel = currentVel + imThere;
			}

			//if displacement is greater than or equal to 40 and less than 80, velocity increases after a short lapse
			if (displacement.x >= 40.f && displacement.x < 80.f)
			{
				//setting a new stopwatch start time
				move2Time = Timer::time;

				//updating the current value of the stopwatch
				letsAGo = StopWatch(move2Time);

				//if less than 0.3 seconds have passed since this level of velocity was reached, the current velocity is kept
				if (letsAGo < 0.3)
				{
					m_physBody->SetVelocity(displacement);
				}

				//after the first 0.3 seconds, the velocity will increase to a new threshold
				else
				{
					//adding a new value for acceleration so the growth towards the next threshold will not be affected by the currently elevated velocity
					//and increasing that value by itself multiplied by deltaTime
					displacing = displacing + displacing * Timer::deltaTime;
					//increasing the rate of acceleration by this new value
					displacement = displacement + displacing;
					//setting new velocity to current velocity + displacement
					imThere = m_physBody->GetVelocity() + displacement;
					//setting velocity of player to new velocity
					m_physBody->SetVelocity(displacement);

					//printing time and player velocity information to the console for graphing purposes
					printf("%f, %f\n", timer, m_physBody->GetVelocity().x);
				}

				//if displacement is greater than or equal to 80 it will level off as this is the max speed for this type of movement
				if (displacement.x >= 80)
				{
					//setting displacement to the max speed
					displacement = vec3(80.f, 0.f, 0.f);
					//setting velocity to displacement value
					m_physBody->SetVelocity(displacement);
					//printing time and player velocity information to the console for graphing purposes
					printf("%f, %f\n", timer, m_physBody->GetVelocity().x);
				}

				//setting player facing to right for animation purposes
				m_facing = RIGHT;
				//setting moving to true while player is moving
				m_moving = true;
			}

		}
	}
	//checking to see if the player has made it to the end of the level
	if (player.GetPosition().x >= 1380.f)
	{
		//if player has reached the end of the level the console will close
		exit(0);
	}
}

void Player::AnimationUpdate()
{
	int activeAnimation = 0;

	if (m_moving)
	{
		//puts it into the WALK category
		activeAnimation = WALK;
	}
	else if (m_attacking)
	{
		activeAnimation = ATTACK;

		//check if attack animation is done
		if (m_animController->GetAnimation(m_animController->GetActiveAnim()).GetAnimationDone())
		{
			//will auto set to idle
			m_locked = false;
			m_attacking = false;
			//resets the attack animation
			m_animController->GetAnimation(m_animController->GetActiveAnim()).Reset();

			activeAnimation = IDLE;
		}
	}
	else
	{
		activeAnimation = IDLE;
	}
	SetActiveAnimation(activeAnimation + (int)m_facing);
}


void Player::SetActiveAnimation(int anim)
{
	m_animController->SetActiveAnim(anim);
}


