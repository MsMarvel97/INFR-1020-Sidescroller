#include "Player.h"
#include <iostream>
#include <fstream>
using namespace std;


int timeCounter = 0;
int counterD = 0;
float startTime = 0;
float letsAGo = 0;
float jumpTime = 0;
float jumpStart = 0;
bool timeToJump = false;
float swStart = 0;
float swNow = 0;
float graphCounter = 0;
vec3 vel = vec3(0.f, 0.f, 0.f);
vec3 displacement = vec3();

Player::Player()
{
}

Player::Player(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite, 
					AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	InitPlayer(fileName, animationJSON, width, height, sprite, controller, transform, hasPhys, body);
}

void Player::InitPlayer(std::string& fileName, std::string& animationJSON, int width, int height, Sprite* sprite,
	AnimationController* controller, Transform* transform, bool hasPhys, PhysicsBody* body)
{
	//Store references to the components
	m_sprite = sprite;
	m_animController = controller;
	m_transform = transform;
	m_hasPhysics = hasPhys;

	if (hasPhys)
	{
		m_physBody = body;
	}

	//Initialize UVs
	m_animController->InitUVs(fileName);

	//Loads the texture and sets width and height
	m_sprite->LoadSprite(fileName, width, height, true, m_animController);
	m_animController->SetVAO(m_sprite->GetVAO());
	m_animController->SetTextureSize(m_sprite->GetTextureSize());

	//Loads in the animations json file
	nlohmann::json animations = File::LoadJSON(animationJSON);

	//IDLE ANIMATIONS

	//Idle Left
	m_animController->AddAnimation(animations["IdleLeft"].get<Animation>());
	//Idle Right
	m_animController->AddAnimation(animations["IdleRight"].get<Animation>());

	//MOVEMENT ANIMATIONS

	//Walk Left
	m_animController->AddAnimation(animations["WalkLeft"].get<Animation>());
	//Walk Right
	m_animController->AddAnimation(animations["WalkRight"].get<Animation>());
	//Run Left
	m_animController->AddAnimation(animations["RunLeft"].get<Animation>());
	//Run Right
	m_animController->AddAnimation(animations["RunRight"].get<Animation>());

	//JUMP ANIMATIONS

	//Jump Left
	m_animController->AddAnimation(animations["JumpLeft"].get<Animation>());
	//Jump Right
	m_animController->AddAnimation(animations["JumpRight"].get<Animation>());

	//ATTACK ANIMATIONS

	//Attack Left
	m_animController->AddAnimation(animations["AttackLeft"].get<Animation>());
	//Attack Right
	m_animController->AddAnimation(animations["AttackRight"].get<Animation>());

	//PUSH ANIMATIONS

	//Push Left
	m_animController->AddAnimation(animations["PushLeft"].get<Animation>());
	//Push Right
	m_animController->AddAnimation(animations["PushRight"].get<Animation>());

	//DEATH ANIMATIONS

	//Death Left
	m_animController->AddAnimation(animations["DeathLeft"].get<Animation>());
	//Death Right
	m_animController->AddAnimation(animations["DeathRight"].get<Animation>());

	//Set Default Animation
	m_animController->SetActiveAnim(IDLERIGHT);
}

void Player::Update()
{
	if (!m_locked)
	{
		MovementUpdate();
	}

	AnimationUpdate();
}

//void Player::MovementUpdate()
//{
//	auto& player = ECS::GetComponent<Player>(MainEntities::MainPlayer());
//	m_moving = false;
//
//	if (m_hasPhysics)
//	{
//		float speed = 550.0f;
//		vec3 vel = m_physBody->GetVelocity();
//
//		if (Input::GetKey(Key::Shift))
//		{
//			speed *= 2.f;
//		}
//
//		if (Input::GetKey(Key::A))
//		{
//
//			if (vel.x > 0)
//			{
//				vel.x = 0.f;
//			}
//			auto timeTester = Timer::deltaTime;
//			vel = vel + vec3(-1.f, 0.f, 0.f) * speed * timeTester;
//			//printf("%f value\n", vel.x);
//			m_facing = LEFT;
//			m_moving = true;
//		}
//
//
//		if (Input::GetKey(Key::D))
//		{
//			if (vel.x < 0)
//			{
//				vel.x = 0.f;
//			}
//			auto timeTester = Timer::deltaTime;
//			vel = vel + vec3(1.f, 0.f, 0.f) * speed * timeTester;
//			//printf("%f value\n", vel.x);
//			m_facing = RIGHT;
//			m_moving = true;
//		}
//
//		else
//		{
//			//vel = vec3();
//			counterD = 0;
//		}
//
//
//
//	/*	if (Input::GetKey(Key::W))
//		{
//			printf("Hello from W");
//			vel = vel + vec3(0.f, 1.f, 0.f);
//			m_moving = true;
//		}*/
//
//		
//		m_physBody->SetVelocity(vel);
//		printf("%.2f m/s :: %.3fs\n", vel.x, Timer::time);
//		//if (Input::GetKeyUp(Key::A))
//		//{
//		//	printf("%d m/s \n", vel * speed);
//		//}
//	}
//
//
//	else
//	{
//		//regular movement
//		float speed = 3000.f;
//
//		if (Input::GetKey(Key::W))
//		{
//			printf("Hello from W2");
//			m_transform->SetPositionY(m_transform->GetPositionY() + (speed * Timer::deltaTime));
//			//m_facing = UP;
//			m_moving = true;
//		}
//		
//		//if (Input::GetKey(Key::S))
//		//{
//		//	m_transform->SetPositionY(m_transform->GetPositionY() - (speed * Timer::deltaTime));
//		//	//m_facing = DOWN;
//		//	m_moving = true;
//		//}
//
//		if (Input::GetKey(Key::A))
//		{
//			m_transform->SetPositionX(m_transform->GetPositionX() - (speed * Timer::deltaTime));
//			m_facing = LEFT;
//			m_moving = true;
//		}
//
//		if (Input::GetKey(Key::D))
//		{
//			m_transform->SetPositionX(m_transform->GetPositionX() + (speed * Timer::deltaTime));
//			m_facing = RIGHT;
//			m_moving = true;
//		}
//	}
//
//	if (Input::GetKeyDown(Key::Space))
//	{
//		m_moving = false;
//
//		if (m_hasPhysics)
//		{
//			m_physBody->SetVelocity(vec3());
//		}
//
//		m_attacking = true;
//		m_locked = true;
//	}
//}


float StopWatch(float startTime)
{
	float swNow = Timer::time - startTime;
	return swNow;
}

void Player::MovementUpdate()
{
	auto& player = ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer());
	vec3 vel = vec3(0.f, 0.f, 0.f);
	float speed = 2.8f;
	m_moving = false;
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

	if (moveToggle == 0)
	{
		if (m_hasPhysics)
		{

			if (Input::GetKey(Key::Shift))
			{
				speed *= 1.5f;
			}


			if (Input::GetKey(Key::A))
			{
				vec3 accelerationA = vec3(-1.f, 0.f, 0.f);
				vec3 acceleratedA = vec3();

				if (startTime == 0)
				{
					startTime = Timer::time;
				}

				float timer = StopWatch(startTime);

				if (Input::GetKey(Key::Shift) && m_physBody->GetVelocity().x > -90.f)
				{
					acceleratedA = player.GetVelocity() + accelerationA * speed;
					player.SetVelocity(acceleratedA);
				}

				if (m_physBody->GetVelocity().x > -60.f)
				{
					acceleratedA = player.GetVelocity() + accelerationA * speed;
					player.SetVelocity(acceleratedA);
				}

				if (m_physBody->GetVelocity().x <= -60.f)
				{
					player.SetVelocity(vec3(-60.f, 0.f, 0.f));
				}

				if (graphCounter == 0)
				{
					printf("%f,%f \n", timer, player.GetVelocity().x);
					graphCounter++;
				}

				else if (graphCounter >= 1 && graphCounter < 10)
				{
					graphCounter += 1;
				}

				else if (graphCounter == 10)
				{
					graphCounter = 0;
				}
				m_facing = LEFT;
				m_moving = true;
				boolMove = true;
			}

			else if (m_physBody->GetVelocity().x < 0.f && !m_facing == RIGHT)
			{
				vec3 deceleration = vec3(1.f, 0.f, 0.f);
				float decelerating = 1.f;
				vel = vel + vec3(1.f, 0.f, 0.f);
				vec3 decelerated = vec3(vel * speed + deceleration);

				if (startTime == 0)
				{
					startTime = Timer::time;
				}

				float timer = StopWatch(startTime);

				decelerated = m_physBody->GetVelocity() + vel * Timer::deltaTime;
				m_physBody->SetVelocity(decelerated);

				printf("%f,%f \n", timer, player.GetVelocity().x);

				m_facing = LEFT;
				m_moving = true;
				boolMove = true;
			}


			if (Input::GetKeyDown(Key::T))//Reset Timer
			{
				startTime = 0;
			}

			//if (Input::GetKeyDown(Key::D))//Move Right (Physics)
			//{
			if (Input::GetKey(Key::D))
			{
				vec3 accelerationD = vec3(1.f, 0.f, 0.f);
				vec3 acceleratedD = vec3();
				if (startTime == 0)
				{
					startTime = Timer::time;
				}

				float timer = StopWatch(startTime);

				if (Input::GetKey(Key::Shift) && m_physBody->GetVelocity().x <= 90.f)
				{
					acceleratedD = m_physBody->GetVelocity() + accelerationD * speed;
					m_physBody->SetVelocity(acceleratedD);
				}

				if (Input::GetKey(Key::Shift) && m_physBody->GetVelocity().x >= 90.f)
				{
					player.SetVelocity(vec3(90.f, 0.f, 0.f));
				}

				if (m_physBody->GetVelocity().x <= 60.f && !Input::GetKey(Key::Shift))
				{
					acceleratedD = m_physBody->GetVelocity() + accelerationD * speed;
					m_physBody->SetVelocity(acceleratedD);
				}

				if (m_physBody->GetVelocity().x >= 60.f && !Input::GetKey(Key::Shift))
				{
					player.SetVelocity(vec3(60.f, 0.f, 0.f));
				}


				if (graphCounter == 0)
				{
					printf("%f,%f \n", timer, player.GetVelocity().x);
					graphCounter++;
				}
				if (graphCounter >= 1 && graphCounter < 10)
				{
					graphCounter += 1;
				}
				if (graphCounter == 10)
				{
					graphCounter = 0;
				}

				m_facing = RIGHT;
				m_moving = true;
				boolMove = true;
			}

			else if (m_physBody->GetVelocity().x > 0.f && !m_facing == LEFT)
			{

				vec3 deceleration = vec3(-1.f, 0.f, 0.f);
				vel = vel + vec3(-1.f, 0.f, 0.f);
				vec3 decelerated = vec3(vel * speed + deceleration);

				if (startTime == 0)
				{
					startTime = Timer::time;
				}

				float timer = StopWatch(startTime);
				decelerated = m_physBody->GetVelocity() + vel * Timer::deltaTime;
				m_physBody->SetVelocity(decelerated);
				printf("%f,%f \n", timer, player.GetVelocity().x);

				m_facing = RIGHT;
				m_moving = true;
				boolMove = true;
			}
			//}

			if (Input::GetKeyDown(Key::W))
			{
				if (timeToJump == false)
				{
					timeToJump = true;
					jumpStart = Timer::time;
				}
			}


			if (timeToJump)
			{
				jumpTime = Timer::time - jumpStart;
				if (jumpTime < 0.25)
				{
					vec3 vel = m_physBody->GetVelocity() + vec3(0.f, 10.f, 0.f);
					m_physBody->SetVelocity(vel);
				}
				if (jumpTime > 0.3)
				{
					timeToJump = false;
				}
			}
		}
	}

	if (moveToggle == 1)
	{
		float speed = 10.f;

		if (Input::GetKey(Key::A))
		{
			float timeTester = speed * Timer::deltaTime;
			float displaced = 0;
			float oldX = m_transform->GetPositionX() - timeTester;
			vec3 imThere = vec3(0.f, 0.f, 0.f);
			vec3 imHere = vec3(0.f, 0.f, 0.f);


			if (displacement.x >= 0.f && displacement.x < 40.f)
			{
				//displacement.x += 10.f;
				//imHere = (vec3(player.GetPosition());
				//imThere = imHere + displacement;
				//Transform::SetPosition(imThere);
				////player.SetPosition(imThere);
				////player.Update();
				//displaced = imThere.x - imHere.x;
				//printf("%f here || %f there || %f displaced\n", imHere.x, imThere.x, displaced);

				displacement.x += 10.f;
				imHere.x = player.GetPosition().x;
				imThere = imHere + displacement;
				/*Transform::SetPosition(imThere);*/
				displaced = imThere.x - imHere.x;
				printf("%f here || %f there || %f displaced\n", imHere.x, imThere.x, displaced);
			}

			if (displacement.x >= 40.f && displacement.x < 80.f)
			{
				displacement.x += displacement.x * timeTester;
				//imThere += imHere + displacement;
				//player.SetPosition(imThere);
				/*Transform::SetPosition(imThere);*/
			}

			if (displacement.x >= 90)
			{
				displacement.x = 90.f;
				imThere = imHere + displacement;
				/*player.SetPosition(imThere);*/
			}





			/*	displacement = (m_physBody->GetVelocity().x * Timer::deltaTime) + (0.5 * acceleration * Timer::deltaTime)pow 2 */


				//if ()
				//{

				//}
				//
				//m_transform->SetPositionX();
			m_facing = LEFT;
			m_moving = true;
		}

		if (Input::GetKey(Key::D))
		{
			auto timeTester = speed * Timer::deltaTime;
			vec3 velo = m_physBody->GetVelocity();




			m_transform->SetPositionX(m_transform->GetPositionX() + (timeTester));
			m_facing = RIGHT;
			m_moving = true;
		}
		m_physBody->SetVelocity(vel * speed);
	}
}

void Player::AnimationUpdate()
{
	int activeAnimation = 0;

	if (m_moving)
	{
		if (Input::GetKey(Key::Shift))
		{
			activeAnimation = RUN;
		}
		else if (Input::GetKey(Key::W))
		{
			activeAnimation = JUMP;
		}
		else
		{
			activeAnimation = WALK;
		}
	}
	else if (Input::GetKey(Key::Space))
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


