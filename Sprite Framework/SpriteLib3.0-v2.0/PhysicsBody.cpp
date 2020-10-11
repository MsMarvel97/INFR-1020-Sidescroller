#include "PhysicsBody.h"


bool PhysicsBody::m_drawBodies = false;

PhysicsBody::PhysicsBody(b2Body * body, float radius, vec2 centerOffset, bool isDynamic)
{
	//Bodies don't reference a shape by themselves
	//they need a shape that has been linked to a fixture
	//in order to have a shape
	b2CircleShape tempShape;
	tempShape.m_p = b2Vec2(float32(centerOffset.x), float32(centerOffset.y));
	tempShape.m_radius = radius;

	//Creates the actual fixture (aka, shape, mass, etc);
	b2FixtureDef tempFixture;
	tempFixture.shape = &tempShape;
	tempFixture.density = 1.f;
	tempFixture.friction = 0.3f;

	m_body = body;
	m_body->CreateFixture(&tempFixture);

	m_body = body;
	m_bodyType = BodyType::CIRCLE;

	//Stores the position;
	m_position = m_body->GetPosition();

	m_width = radius * 2.f;
	m_height = radius * 2.f;

	m_centerOffset = centerOffset;
	m_bottomLeft = vec2(centerOffset.x - (radius), centerOffset.y - (radius));
	m_bottomRight = vec2(centerOffset.x + (radius), centerOffset.y - (radius));
	m_topLeft = vec2(centerOffset.x - (radius), centerOffset.y + (radius));
	m_topRight = vec2(centerOffset.x + (radius), centerOffset.y + (radius));

	InitBody();
}

PhysicsBody::PhysicsBody(b2Body * body, float width, float height, vec2 centerOffset, bool isDynamic)
{
	//Bodies don't reference a shape by themselves
	//they need a shape that has been linked to a fixture
	//in order to have a shape
	b2PolygonShape tempShape;
	tempShape.SetAsBox(float32(width / 2.f), float32(height / 2.f),
		b2Vec2(float32(centerOffset.x), float32(centerOffset.y)), float32(0.f));

	//Creates the actual fixture (aka, shape, mass, etc);
	b2FixtureDef tempFixture;
	tempFixture.shape = &tempShape;
	tempFixture.density = 1.f;
	tempFixture.friction = 0.3f;

	m_body = body;
	m_body->CreateFixture(&tempFixture);

	m_body = body;
	m_bodyType = BodyType::BOX;

	m_width = width;
	m_height = height;

	m_centerOffset = centerOffset;
	m_bottomLeft = vec2(centerOffset.x - (width / 2.f), centerOffset.y - (height / 2.f));
	m_bottomRight = vec2(centerOffset.x + (width / 2.f), centerOffset.y - (height / 2.f));
	m_topLeft = vec2(centerOffset.x - (width / 2.f), centerOffset.y + (height / 2.f));
	m_topRight = vec2(centerOffset.x + (width / 2.f), centerOffset.y + (height / 2.f));

	InitBody();
}

void PhysicsBody::DeleteBody()
{
	if (m_body != nullptr)
	{
		m_body->GetWorld()->DestroyBody(m_body);
	}
}

void PhysicsBody::InitBody()
{
	m_vao = VertexManager::CreateVAO();
	glBindVertexArray(m_vao);

	//Enable slot 0
	glEnableVertexAttribArray(0);	//Vertices

	//VBO positions
	m_vboPos = VertexManager::GetPlaneVertVBO();

	//Pushes away the warnings
#pragma warning(push)
#pragma warning(disable : 4312)
//Bind position buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vboPos);
	//Sets the attribute pointer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<void*>(0));
#pragma warning(pop)

	//Unbinds the buffers
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);
}

void PhysicsBody::DrawBody()
{
	//Bind vao
	glBindVertexArray(m_vao);
	//Draw the points
	glDrawArrays(GL_TRIANGLES, 0, 6);
	//Unbind vao
	glBindVertexArray(GL_NONE);
}

void PhysicsBody::Update(Transform * trans)
{
	//Stores the position;
	m_position = m_body->GetPosition();

	//Sets the transform position to the position of the physics body
	trans->SetPosition(vec3(m_body->GetPosition().x, m_body->GetPosition().y, trans->GetPosition().z));
	trans->SetRotationAngleZ(Degrees(m_body->GetAngle()));
}

void PhysicsBody::ApplyForce(vec3 force)
{
	m_body->ApplyForce(b2Vec2(float32(force.x), float32(force.y)),
						b2Vec2(float32(m_body->GetPosition().x), float32(m_body->GetPosition().y)),
						 true);

}

b2Body * PhysicsBody::GetBody() const
{
	return m_body;
}

b2Vec2 PhysicsBody::GetPosition() const
{
	return m_position;
}

vec3 PhysicsBody::GetVelocity() const
{
	//Returns current velocity
	b2Vec2 vel = m_body->GetLinearVelocity();
	vec3 temp = vec3(vel.x, vel.y, 0.f);
	
	return temp;
}

float PhysicsBody::GetMass() const
{
	b2MassData massData;
	m_body->GetMassData(&massData);
	return massData.mass;
}


BodyType PhysicsBody::GetBodyType() const
{
	//What kinda physics body is this?
	return m_bodyType;
}

vec2 PhysicsBody::GetCenterOffset() const
{
	//return offset from the center of the sprite
	return m_centerOffset;
}

vec2 PhysicsBody::GetBottomLeft() const
{
	//return bottom left corner
	return m_bottomLeft;
}

vec2 PhysicsBody::GetBottomRight() const
{
	//return bottom right corner
	return m_bottomRight;
}

vec2 PhysicsBody::GetTopLeft() const
{
	//return top left corner
	return m_topLeft;
}

vec2 PhysicsBody::GetTopRight() const
{
	//return top right corner
	return m_topRight;
}

float PhysicsBody::GetWidth() const
{
	//return body width
	return m_width;
}

float PhysicsBody::GetHeight() const
{
	//return body height
	return m_height;
}

float PhysicsBody::GetRadius() const
{
	return m_radius;
}

bool PhysicsBody::GetDraw()
{
	return m_drawBodies;
}

void PhysicsBody::SetBody(b2Body * body)
{
	m_body = body;
}

void PhysicsBody::SetPosition(b2Vec2 bodyPos)
{
	m_position = bodyPos;
}

void PhysicsBody::SetVelocity(vec3 velo)
{
	b2Vec2 vel = b2Vec2(velo.x, velo.y);

	m_body->SetLinearVelocity(vel);
}

void PhysicsBody::SetMass(float mass)
{
	b2MassData massData;
	m_body->GetMassData(&massData);
	massData.mass = mass;

	m_body->SetMassData(&massData);
}

void PhysicsBody::SetBodyType(BodyType type)
{
	//Set the type of body this is
	m_bodyType = type;
}

void PhysicsBody::SetCenterOffset(vec2 cent)
{
	//Set the center offset
	m_centerOffset = cent;
}

void PhysicsBody::SetBottomLeft(vec2 BL)
{
	//Set the bottom left corner
	m_bottomLeft = BL;
}

void PhysicsBody::SetBottomRight(vec2 BR)
{
	//Set the bottom right corner
	m_bottomRight = BR;
}

void PhysicsBody::SetTopLeft(vec2 TL)
{
	//Set the top left corner
	m_topLeft = TL;
}

void PhysicsBody::SetTopRight(vec2 TR)
{
	//Set the top right corner
	m_topRight = TR;
}

void PhysicsBody::SetWidth(float width)
{
	//Set the physics body width
	m_width = width;
}

void PhysicsBody::SetHeight(float height)
{
	//Sets the physics body height
	m_height = height;
}

void PhysicsBody::SetRadius(float radius)
{
	m_radius = radius;
}

void PhysicsBody::SetDraw(bool drawBodies)
{
	m_drawBodies = drawBodies;
}

