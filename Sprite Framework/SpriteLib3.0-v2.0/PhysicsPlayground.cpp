#include "PhysicsPlayground.h"
#include "AnimationSpritePlayground.h"
#include "Utilities.h"

PhysicsPlayground::PhysicsPlayground(std::string name)
	: Scene(name)
{
	//no gravity this is a top down scene
	m_gravity = b2Vec2(0.f, -400.f);
	m_physicsWorld->SetGravity(m_gravity);
}

void PhysicsPlayground::InitScene(float windowWidth, float windowHeight)
{
	//Dynamically allocates the register
	m_sceneReg = new entt::registry;

	//Attach the register
	ECS::AttachRegister(m_sceneReg);

	//Sets up aspect ratio for the camera
	float aspectRatio = windowWidth / windowHeight;

	//Setup MainCamera Entity
	{
		//Scene::CreateCamera(m_sceneReg, vec4(-75f, 75.f, -75.f, 75f), -100.f, windowWidth, windowHeight, true, true);
		//Creates Camera Entity
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainCamera(entity, true);

		//Creates new orthographic camera
		ECS::AttachComponent<Camera>(entity);
		ECS::AttachComponent<HorizontalScroll>(entity);
		ECS::AttachComponent<VerticalScroll>(entity);

		vec4 temp = vec4(-75.f, 75.f, -75.f, 75.f);
		ECS::GetComponent<Camera>(entity).SetOrthoSize(temp);
		ECS::GetComponent<Camera>(entity).SetWindowSize(vec2(float(windowWidth), float(windowHeight)));
		ECS::GetComponent<Camera>(entity).Orthographic(aspectRatio, temp.x, temp.y, temp.z, temp.w, -100.f, 100.f);

		//Attaches the camera to vert and horz scrolls
		ECS::GetComponent<HorizontalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
		ECS::GetComponent<VerticalScroll>(entity).SetCam(&ECS::GetComponent<Camera>(entity));
	}

	//Setup background
	//Gonna loop this later
	{
		//Scene::CreateSprite(m_sceneReg, "boxSprite.jpg", 100, 60, 0.5f, vec3(0.f, 0.f, 0.f));

		//Creates entity
		auto entity = ECS::CreateEntity();

		//Adds Components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);

		//Setup the Components
		std::string fileName = "SnowBG.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 600, 384);
		ECS::GetComponent<Sprite>(entity).SetTransparency(1.f);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 20.f, 0.f));
	}

	//Setup Box Prop
	{
		//Creates Entity
		auto entity = ECS::CreateEntity();

		//Add Components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Setup the Components
		std::string fileName = "boxSprite.jpg";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 20, 20);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(-30.f, 20.f, 2.f));

		auto& tempSprite = ECS::GetComponent<Sprite>(entity);
		auto& tempPhysBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(50.f), float32(-20.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhysBody = PhysicsBody(tempBody, float(tempSprite.GetWidth() - shrinkX), float(tempSprite.GetHeight() - shrinkY), vec2(0.f, 0.f), false);
	}

	//Setup Floor
	{
		//Creates Entity
		auto entity = ECS::CreateEntity();

		//Add Components
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);

		//Setup the Components
		std::string fileName = "rockfloor.png";
		ECS::GetComponent<Sprite>(entity).LoadSprite(fileName, 372, 391);
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, -20.f, 2.f));

		auto& tempSprite = ECS::GetComponent<Sprite>(entity);
		auto& tempPhysBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;
		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_staticBody;
		tempDef.position.Set(float32(0.f), float32(-222.f)); //set position

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhysBody = PhysicsBody(tempBody, float(tempSprite.GetWidth() - shrinkX), float(tempSprite.GetHeight() - shrinkY), vec2(0.f, 0.f), false);

	}

	//Main Entity
	{
		auto entity = ECS::CreateEntity();
		ECS::SetIsMainPlayer(entity, true);

		//Adds Components
		ECS::AttachComponent<Player>(entity);
		ECS::AttachComponent<Sprite>(entity);
		ECS::AttachComponent<Transform>(entity);
		ECS::AttachComponent<PhysicsBody>(entity);
		ECS::AttachComponent<AnimationController>(entity);

		//Setup the Components
		std::string fileName = "spritesheet/PinkMonster_Spritesheet.png";
		std::string animations = "PinkMonster_Animations.json";
		ECS::GetComponent<Player>(entity).InitPlayer(fileName, animations, 32, 32, &ECS::GetComponent<Sprite>(entity),
			&ECS::GetComponent<AnimationController>(entity), &ECS::GetComponent<Transform>(entity), true, &ECS::GetComponent<PhysicsBody>(entity));
		ECS::GetComponent<Transform>(entity).SetPosition(vec3(0.f, 0.f, 100.f)); //layer 100, 0 in y

		//Physics
		auto& tempSprite = ECS::GetComponent<Sprite>(entity);
		auto& tempPhysBody = ECS::GetComponent<PhysicsBody>(entity);

		float shrinkX = 0.f;
		float shrinkY = 0.f;

		b2Body* tempBody;
		b2BodyDef tempDef;
		tempDef.type = b2_dynamicBody;
		tempDef.position.Set(float32(0.f), float32(30.f));

		tempBody = m_physicsWorld->CreateBody(&tempDef);

		tempPhysBody = PhysicsBody(tempBody, float(tempSprite.GetWidth() - shrinkX), float(tempSprite.GetHeight() - shrinkY), vec2(0.f, 0.f), false);

		ECS::GetComponent<PhysicsBody>(MainEntities::MainPlayer()).GetBody()->SetFixedRotation(true);
	}

	ECS::GetComponent<HorizontalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));
	ECS::GetComponent<VerticalScroll>(MainEntities::MainCamera()).SetFocus(&ECS::GetComponent<Transform>(MainEntities::MainPlayer()));

}

void PhysicsPlayground::Update()
{
	auto& player = ECS::GetComponent<Player>(MainEntities::MainPlayer());
	player.Update();
}


void PhysicsPlayground::KeyboardHold()
{
}

void PhysicsPlayground::KeyboardDown()
{
}

void PhysicsPlayground::KeyboardUp()
{
}
