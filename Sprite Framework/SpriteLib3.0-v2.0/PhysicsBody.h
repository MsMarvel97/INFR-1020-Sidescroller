#pragma once

#include <GL/glew.h>
#include <Box2D/Box2D.h>

#include "JSON.h"
#include "Vector.h"
#include "VertexManager.h"
#include "Transform.h"

enum class BodyType
{
	BOX,
	CIRCLE,

	NUM_TYPES
};

struct CollisionIDs
{
public:
	//Gets the different IDs
	static unsigned int Player();
	static unsigned int Environment();
	static unsigned int Enemy();
private:
	//Holds the different IDs
	static unsigned int m_playerID;
	static unsigned int m_environmentID;
	static unsigned int m_enemyID;
};


class PhysicsBody
{
public:
	PhysicsBody() { };
	//Constructs a circle collider
	PhysicsBody(b2Body* body, float radius, vec2 centerOffset, bool isDynamic);
	//Constructs a box collider
	PhysicsBody(b2Body* body, float width, float height, vec2 centerOffset, bool isDynamic);

	void DeleteBody();

	//Initializes body for drawing
	void InitBody();

	//Depreciated
	void DrawBody();

	//Update physics stuffs
	void Update(Transform* trans);

	//Apply a force to the physics body
	void ApplyForce(vec3 force);

	//Getters
	//Get the Box2D physics body
	b2Body* GetBody() const;
	//Get position of body
	b2Vec2 GetPosition() const;
	//Gets the current velocity
	vec3 GetVelocity() const;

	//Set the mass of the phyics body
	float GetMass() const;

	//Get the body type enum
	BodyType GetBodyType() const;
	//Gets the center offset for the body
	//*if the offset is 0,0, then all corners will be relative to the
	//center of the actual sprite
	vec2 GetCenterOffset() const;
	//Gets the corners of the body
	vec2 GetBottomLeft() const;
	vec2 GetBottomRight() const;
	vec2 GetTopLeft() const;
	vec2 GetTopRight() const;
	//Gets the width of the physics body
	float GetWidth() const;
	//Gets the height of the physics body
	float GetHeight() const;
	//Gets the radius of the physics body
	float GetRadius() const;

	//Get whether or not we are currently drawing our physics bodies
	static bool GetDraw();

	//Setters
	//Sets the pointer to the box2D body
	void SetBody(b2Body* body);
	//Set position (just sets the variable, doesn't actually set the position)
	void SetPosition(b2Vec2 bodyPos);
	//Sets the velocity of the phyiscs body
	void SetVelocity(vec3 velo);

	//Set the mass of the physics body
	void SetMass(float mass);

	//If other body types were ever implemented we could set it here 
	//(obviously different types wouldn't use the whole BL, BR, TL, TR
	void SetBodyType(BodyType type);
	//Sets the center offset for the body
	//*if the offset is 0,0, then all corners will be relative to the
	//center of the actual sprite	void SetCenterOffset(vec2 cent);
	void SetCenterOffset(vec2 cent);
	//Sets the corners of the body
	void SetBottomLeft(vec2 BL);
	void SetBottomRight(vec2 BR);
	void SetTopLeft(vec2 TL);
	void SetTopRight(vec2 TR);
	//Sets the width of the body
	void SetWidth(float width);
	//Sets the height of the body
	void SetHeight(float height);
	//Sets the radius of the body
	void SetRadius(float radius);
	//Set whether the bodies are being drawn
	static void SetDraw(bool drawBodies);

private:
	//The actual box2D body
	b2Body* m_body = nullptr;
	b2Vec2 m_position = b2Vec2(0.f, 0.f);

	//Stores the velocity
	vec3 m_velocity = vec3(0.f, 0.f, 0.f);

	//Body type
	BodyType m_bodyType = BodyType::CIRCLE;
	//How far from the center of the sprite is it
	vec2 m_centerOffset = vec2();
	//Each corner
	vec2 m_bottomLeft = vec2();
	vec2 m_bottomRight = vec2();
	vec2 m_topLeft = vec2();
	vec2 m_topRight = vec2();
	//Width and height of the body
	float m_width = 0.f;
	float m_height = 0.f;

	float m_radius = 0.f;
	
	//Do you draw the bodies?
	static bool m_drawBodies;

	//Physics body drawing stuff
	GLuint m_vao = GL_NONE;
	GLuint m_vboPos = GL_NONE;
};