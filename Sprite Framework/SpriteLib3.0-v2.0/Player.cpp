#include "Player.h"
int timeCounter = 0;
int counterD = 0;
using namespace std;
float startTime = 0;
float jumpTime = 0;
float jumpStart = 0;
bool timeToJump = false;
float swStart = 0;
float swNow = 0;
vec3 vel = vec3(0.f, 0.f, 0.f);

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
	float speed = 3.f;
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
				speed *= 2.f;
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

					if (m_physBody->GetVelocity().x > -60.f)
					{
						acceleratedA = m_physBody->GetVelocity() + accelerationA * speed;
						m_physBody->SetVelocity(acceleratedA);
					}

					printf("%f m/s || %f \n", player.GetPosition().x, timer);

					m_facing = LEFT;
					m_moving = true;
					boolMove = true;
				}

				else if (m_physBody->GetVelocity().x < 0.f)
				{
					vec3 deceleration = vec3(1.f, 0.f, 0.f);
					vel = vel + vec3(1.f, 0.f, 0.f);
					vec3 decelerated = vec3(vel * speed + deceleration);

					if (startTime == 0)
					{
						startTime = Timer::time;
					}

					float timer = StopWatch(startTime);

					decelerated = m_physBody->GetVelocity() + vel * Timer::deltaTime;
					m_physBody->SetVelocity(decelerated);
					printf("%fm/s || %fs \n", m_physBody->GetVelocity().x, timer);

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

					if (m_physBody->GetVelocity().x <= 60.f)
					{
						acceleratedD = m_physBody->GetVelocity() + accelerationD * speed;
						m_physBody->SetVelocity(acceleratedD);
					}


					printf("%f m/s || %f \n", m_physBody->GetVelocity().x, timer);
					m_facing = RIGHT;
					m_moving = true;
					boolMove = true;
				}

				else if (m_physBody->GetVelocity().x > 0.f)
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
					printf("%fm/s || %fs \n", m_physBody->GetVelocity().x, timer);

					m_facing = RIGHT;
					m_moving = true;
					boolMove = true;
				}
			//}

			if (Input::GetKeyDown(Key::Space))
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
			


			/*if (Input::GetKeyDown(Key::Space))
			{
				if (vel.y <= 0)
				{
					jump = true;
				}
			}

			if (jump) {
				if (vel.y > 0) {
					sTime = Timer::currentClock;
					isJumping = true;
				}
				else {
					float dTime = Timer::currentClock;
					speed = 5.8;
					if (Input::GetKey(Key::Shift)) {
						speed *= 1.1;
					}
					vel = vel + vec3(0.f, 4.5f, 0.f);
					if ((dTime - sTime) >= 200) {
						isJumping = false;
						jump = false;
					}
				}*/

			/*}*/

		}
	}

	if(moveToggle == 1)
	{
		float speed = 1000.f;

		if (Input::GetKey(Key::A))
		{
			float timeTester = speed * Timer::deltaTime;
			float oldX = m_transform->GetPositionX() - timeTester;

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

	
