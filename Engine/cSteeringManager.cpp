#include "cSteeringManager.h"
#include "cDebugRenderer.h"

#include <iostream>

extern float generateRandomNumber( float min, float max );

extern float CIRCLE_DISTANCE;
extern float CIRCLE_RADIUS;
extern float ANGLE_CHANGE;

extern void addCircleToDebugRenderer( glm::vec3 position, float range, glm::vec3 color );
extern cDebugRenderer* g_pDebugRenderer;

static const float MAX_FORCE = 40.0f; // 5.4f; //10.0f;

float calculateAngle( glm::vec3 vel )
{
	float movAngle = 0.0f;

	// Calculate the moving angle using the velocity
	movAngle = atan2( vel.x, vel.z );

	// Convert it from radians to degrees
	movAngle = ( movAngle * 180 ) / glm::pi<float>();

	return movAngle;
}

void setAngle( glm::vec3 &vector, float value )
{
	float lenght = glm::length( vector );
	vector.x = glm::cos( value ) * lenght;
	vector.z = glm::sin( value ) * lenght;

	//vector.x = glm::cos( value );
	//vector.z = glm::sin( value );
}

cSteeringManager::cSteeringManager()
{
	currentTimeStep = 0.0;
}


cSteeringManager::~cSteeringManager()
{
}

void cSteeringManager::updateAll( double deltaTime )
{
	this->currentTimeStep = deltaTime;

	cGameObject* pTargetGO = findTarget( nullptr );

	for( int i = 0; i != ::g_vecGameObjects.size(); i++ )
	{
		cGameObject* pTheGO = ::g_vecGameObjects[i];
		if( pTheGO->type != CHARACTER ) continue;
		if( pTheGO->team != ENEMY ) continue;

		setBehaviour( pTheGO, pTargetGO );
		solveSteering( pTheGO, pTargetGO );
		update( pTheGO, deltaTime );
	}
}

void cSteeringManager::reset()
{
}

glm::vec3 limitVector( glm::vec3 currentVector, float maximum )
{
	float velTotal = abs(currentVector.x) + abs( currentVector.y ) + abs( currentVector.z );
	//if( velTotal > maximum )
	//{
		glm::vec3 newVector = glm::vec3( ( currentVector.x / velTotal ) * maximum,
										 ( currentVector.y / velTotal ) * maximum,
										 ( currentVector.z / velTotal ) * maximum );

		return newVector;
	//}
	//else
	//	return currentVector;
}

glm::vec3 scaleVector( glm::vec3 currentVector, float maximum )
{
	glm::vec3 newVector = glm::normalize( currentVector ) * maximum;
	return newVector;
}


void cSteeringManager::update( cGameObject* pTheGO, double deltaTime )
{
	glm::vec3 velocity = pTheGO->vel;
	glm::vec3 position = pTheGO->position;
	glm::vec3 accel = pTheGO->accel;

	accel = limitVector( accel, MAX_FORCE );
	
	velocity += accel;
	//velocity = limitVector( velocity, ( pTheGO->maxVel * deltaTime ) );
	velocity = limitVector( velocity, pTheGO->maxVel );

	position += velocity;

	pTheGO->vel = velocity;
	pTheGO->position = position;
	pTheGO->accel = glm::vec3( 0.0f );

	glm::vec3 tempVel = velocity * 1000.0f; // scaleVector( ( velocity + position ), 1.0f );
	tempVel += position;

	::g_pDebugRenderer->addLine( position, tempVel, glm::vec3( 1.0f, 0.0f, 0.0f ), false );

	//truncate( steering, MAX_FORCE );
	//steering.scaleBy( 1 / host.getMass() );

	//velocity.incrementBy( steering );
	//truncate( velocity, host.getMaxVelocity() );

	//position.incrementBy( velocity );



	//// Euler Integration
	////accel = limitVector( accel, MAX_FORCE );
	////float accTotal = accel.x + accel.y + accel.z;
	////if( accTotal > ( MAX_FORCE * 1.1f ) )
	////{
	////	int breakpoint = 1;
	////}

	//// New velocity is based on acceleration over time
	////glm::vec3 deltaVelocity = accel;
	//glm::vec3 deltaVelocity = ( ( float )deltaTime * accel );
	////	+ ( ( float )deltaTime * GRAVITY );

	//velocity += deltaVelocity;

	//// Make sure velocity is not greater than maximum
	//velocity = limitVector( velocity, ( pTheGO->maxVel * deltaTime ) );
	////velocity = limitVector( velocity, pTheGO->maxVel );

	//float velTotal = velocity.x + velocity.y + velocity.z;
	//if( velTotal > ( pTheGO->maxVel * 1.10 ) )
	//{
	//	int breakpoint = 1;
	//}

	//// New position is based on velocity over time
	//glm::vec3 deltaPosition = /*( float )deltaTime * */velocity;
	//position += deltaPosition;
	////position += velocity;	

	//pTheGO->vel = velocity;
	//pTheGO->position = position;
	////pTheGO->vel = glm::vec3( 0.0f );
	//pTheGO->accel = glm::vec3( 0.0f );

	//glm::vec3 tempVel = velocity; // +position;
	//tempVel = glm::normalize( tempVel ) * 2.0f;
	//tempVel += position;

	//::g_pDebugRenderer->addLine( position, tempVel, glm::vec3( 1.0f, 0.0f, 0.0f ), false );
	//
	////float rotation = calculateAngle( velocity );
	////rotation = glm::radians( rotation );

	////glm::vec3 rotateAngle = glm::vec3( 0.0f );
	////rotateAngle.y = rotation;

	////pTheGO->adjustQOrientationFormDeltaEuler( rotateAngle );

	return;
}

cGameObject * cSteeringManager::findTarget( cGameObject* pTheGO )
{
	for( int i = 0; i != ::g_vecGameObjects.size(); i++ )
	{
		cGameObject* pTargetGO = ::g_vecGameObjects[i];
		if( pTargetGO->type != CHARACTER ) continue;
		if( pTargetGO->team == PLAYER )
		{
			return pTargetGO;
		}		
	}
	return nullptr;
}

void cSteeringManager::setBehaviour( cGameObject* pTheGO, cGameObject* pTargetGO )
{
	bool bPlayerIsFacingMe = pTheGO->isFacingMe( pTargetGO->getDirectionVector(), pTargetGO->position );
	
	bool bIsPlayerInRange = false;
	float distanceFromThePlayer = glm::distance( pTheGO->position, pTargetGO->position );

	if( distanceFromThePlayer <= pTheGO->range ) bIsPlayerInRange = true;

	if( bIsPlayerInRange )
	{
		if( bPlayerIsFacingMe )	pTheGO->behaviour = eEnemyBehaviour::FLEE;
		else					pTheGO->behaviour = eEnemyBehaviour::SEEK;
	}
	else
	{
		pTheGO->behaviour = eEnemyBehaviour::IDLE;
	}

//eEnemyBehaviour::WANDER
//eEnemyBehaviour::SEEK
//eEnemyBehaviour::FLEE
//eEnemyBehaviour::PURSUE
//eEnemyBehaviour::ARRIVE
//eEnemyBehaviour::APPROACH
//eEnemyBehaviour::EVADE
//eEnemyBehaviour::IDLE
//eEnemyBehaviour::NA
}

void cSteeringManager::solveSteering( cGameObject* pTheGO, cGameObject* pTargetGO )
{
	float slowingRadius = 0.2f;

	pTheGO->accel += wander( pTheGO );

	switch( pTheGO->behaviour )
	{
		case WANDER :
			pTheGO->accel += wander( pTheGO );
			break;

		case SEEK:
			pTheGO->accel += wander( pTheGO );
			pTheGO->accel += seek( pTheGO, pTargetGO->position, slowingRadius );
			break;

		case FLEE:
			pTheGO->accel += wander( pTheGO );
			pTheGO->accel += flee( pTheGO, pTargetGO->position );
			break;

		case PURSUE :
			//pTheGO->accel += pursuit( pTheGO, pTargetGO, slowingRadius );
			break;

		case ARRIVE :

			break;

		case APPROACH :

			break;

		case EVADE :
			//pTheGO->accel += evade( pTheGO, pTargetGO );
			break;

		case IDLE:
			pTheGO->accel += wander( pTheGO );
			break;

		default:
			break;
	}
}

glm::vec3 cSteeringManager::seek( cGameObject* pTheGO, glm::vec3 targetDestination, float slowingRadius )
{
	glm::vec3 force;

	glm::vec3 targetVec = targetDestination - pTheGO->position;
	float distance = glm::length( targetVec );
	targetVec = glm::normalize( targetVec );

	if( distance <= slowingRadius )
	{
		float limit = pTheGO->maxVel * distance / slowingRadius;
		force  = scaleVector( targetVec, limit );
	}
	else
	{
		force = scaleVector( targetVec, pTheGO->maxVel );
	}

	force = force - pTheGO->vel;

	std::cout << "Seek force: "
		<< force.x << ", "
		<< force.y << ", "
		<< force.z << std::endl;

	return force;
}

glm::vec3 cSteeringManager::flee( cGameObject* pTheGO, glm::vec3 targetDestination )
{
	glm::vec3 force;

	glm::vec3 targetVec = pTheGO->position - targetDestination;
	targetVec = glm::normalize( targetVec );

	force = scaleVector( targetVec, pTheGO->maxVel );

	force = force - pTheGO->vel;

	return force;
}

glm::vec3 cSteeringManager::wander( cGameObject* pTheGO )
{
	glm::vec3 wanderForce;
	glm::vec3 circleCenter;
	glm::vec3 displacement;

	if( pTheGO->vel == glm::vec3( 0.0f ) )
	{
		pTheGO->vel = glm::normalize( glm::vec3( generateRandomNumber( -1, 1 ),
												 0.0f,
												 generateRandomNumber( -1, 1 ) ) );
	}

	circleCenter = pTheGO->vel;
	glm::normalize( circleCenter );

	circleCenter = scaleVector( circleCenter, CIRCLE_DISTANCE );

	displacement = glm::vec3( 0, 0, -1 );
	displacement = scaleVector( displacement, CIRCLE_RADIUS );

	float wanderAngle = pTheGO->wanderAngle;
	setAngle( displacement, wanderAngle );

	float newAngle = generateRandomNumber( 0, 0.99f ) * ANGLE_CHANGE - ANGLE_CHANGE * 0.5f;
	pTheGO->wanderAngle = wanderAngle + newAngle;

	wanderForce = circleCenter + displacement;

	wanderForce = scaleVector( wanderForce, pTheGO->maxVel );

	glm::vec3 displacementPos = displacement + circleCenter + pTheGO->position;
	glm::vec3 circleCenterPos = circleCenter + pTheGO->position;

	//::g_pDebugRenderer->addLine( pTheGO->position, ( circleCenter + pTheGO->position ), glm::vec3( 1.0f, 1.0f, 0.0f ), false );
	addCircleToDebugRenderer( circleCenterPos, CIRCLE_RADIUS, glm::vec3( 1.0f, 1.0f, 0.0f ) );
	::g_pDebugRenderer->addLine( circleCenterPos, displacementPos, glm::vec3( 0.0f, 1.0f, 1.0f ), false );
	::g_pDebugRenderer->addLine( pTheGO->position, displacementPos, glm::vec3( 0.0f, 1.0f, 0.0f ), false );

	return wanderForce;
}

glm::vec3 cSteeringManager::evade( cGameObject* pTheGO, cGameObject* pTargetGO )
{
	//glm::vec3 force;

	glm::vec3 targetVec = pTargetGO->position - pTheGO->position;
	float distance = glm::length( targetVec );	

	float updatesNeeded = distance / pTheGO->maxVel;

	glm::vec3 targetVel = pTargetGO->vel;
	
	targetVel = scaleVector( targetVel, updatesNeeded );

	glm::vec3 targetFuturePosition = pTargetGO->position + targetVel;

	return flee( pTheGO, targetFuturePosition );
}

glm::vec3 cSteeringManager::pursuit( cGameObject* pTheGO, cGameObject* pTargetGO, float slowingRadius )
{	
	glm::vec3 targetVec = pTargetGO->position - pTheGO->position;
	float distance = glm::length( targetVec );

	float updatesNeeded = distance / pTheGO->maxVel;

	glm::vec3 targetVel = glm::vec3( 0.0f );
	if( pTargetGO->vel != glm::vec3( 0.0f ) )
		targetVel = scaleVector( pTargetGO->vel, updatesNeeded );

	glm::vec3 targetFuturePosition = pTargetGO->position + targetVel;

	return seek( pTheGO, targetFuturePosition, slowingRadius );
}
