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

	for (int i = 0; i <= 4; i++) //makes 4 backgrounds
	{
		float posx = i * 384;
		CreateBackground("SnowBG.png", 600, 384, 1.f, posx, 100.f, 0.f);
	}

	CreateSpriteEntity(false, "boxSprite.jpg", 20, 20, -30.f, 20.f, 2.f, 50.f, -20.f);
	CreateSpriteEntity(true, "boxSprite.jpg", 20, 20, -30.f, 20.f, 2.f, 156.f, 20.f);
	CreateSpriteEntity(true, "boxSprite.jpg", 20, 20, -30.f, 20.f, 2.f, 136.f, 0.f);
	CreateSpriteEntity(true, "boxSprite.jpg", 20, 20, -30.f, 20.f, 2.f, 176.f, 40.f);
	CreateSpriteEntity(true, "boxSprite.jpg", 20, 20, -30.f, 20.f, 2.f, 600.f, 70.f);
	CreateSpriteEntity(true, "boxSprite.jpg", 20, 20, -30.f, 20.f, 2.f, 660.f, 90.f);
	CreateSpriteEntity(true, "flag.png", 30, 60, -30.f, 20.f, 2.f, 1420.f, 170.f);
	CreateSpriteEntity(true, "platform.png", 316, 83, -30.f, 20.f, 2.f, 844.f, 100.f);
	CreateSpriteEntity(true, "platform.png", 316, 83, -30.f, 20.f, 2.f, 1260.f, 100.f);
	CreateSpriteEntity(true, "rockfloor.png", 372, 391, 0.f, -20.f, 1.f, 0.f, -220.f);
	CreateSpriteEntity(true, "rockfloor.png", 362, 391, 0.f, -20.f, 2.f, 362.f, -130.f);
	CreateSpriteEntity(true, "rockfloor.png", 372, 391, 0.f, -20.f, 2.f, -372.f, 100.f); //world barrier
	CreateSpriteEntity(true, "rockfloor.png", 372, 391, 0.f, -20.f, 2.f, 1600.f, 100.f); //world barrier
	CreateSpriteEntity(true, "lava.png", 1100, 160, 0.f, -20.f, 1.f, 950.f, -150.f);
	
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
