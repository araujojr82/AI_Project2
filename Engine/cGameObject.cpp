#include "cGameObject.h"

// Start the unique IDs at 1. Why not?
/*static*/ unsigned int cGameObject::m_nextUniqueID = 1;

cGameObject::cGameObject()
{
	this->scale = 1.0f;	// (not zero)
	this->position = glm::vec3(0.0f);
	this->prevPosition = glm::vec3( NULL );
	this->overwrtiteQOrientationFormEuler( glm::vec3( 0.0f, 0.0f, 0.0f ) );

	this->vel = glm::vec3( 0.0f );
	this->accel = glm::vec3( 0.0f );

	// If you aren't sure what the 4th value should be,
	// make it a 1.0f ("alpha" or transparecy)
	this->diffuseColour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	//Assume everything is simulated 
	this->bIsUpdatedInPhysics = true; //??? 
	this->radius = 0.0f;	// Is this the best value??? Who knows?
	this->mass = 0.0f;
	this->inverseMass = 0.0f;
	this->typeOfObject = UNKNOWN;
	
	this->bIsLight = false;
	this->bIsWireFrame = false;
	
	this->myLight = NULL;
	
	// Set all texture blend values to 0.0f (meaning NO texture)
	for( int index = 0; index != NUMTEXTURES; index++ )
	{
		this->textureBlend[index] = 0.0f;
	}

	// Assign unque ID, the increment for next created object
	// (Note: if you write your own copy constructor, be sure to COPY this
	//	value, rather than generate a new one - i.e. call the c'tor again)
	this->m_UniqueID = cGameObject::m_nextUniqueID++;

	//this->pDebugRenderer = NULL;

	//this->bIsSkyBoxObject = false;

	this->type = eTypeOfGO::OTHER;
	this->team = eTeam::NONE;
	this->enemyType = eEnemyType::UNAVAIABLE;
	this->behaviour = eEnemyBehaviour::NA;
	this->range = NULL;
	this->health = NULL;
	this->maxVel = NULL;
	this->wanderAngle = 0.0f;

	return;
}

cGameObject::~cGameObject()
{
	return;
}

void cGameObject::overwrtiteQOrientationFormEuler( glm::vec3 eulerAxisOrientation )
{
	this->prevOrientation = this->qOrientation;
	// Calcualte the quaternion represnetaiton of this Euler axis
	// NOTE: We are OVERWRITING this..
	this->qOrientation = glm::quat( eulerAxisOrientation );

	return;
}

void cGameObject::adjustQOrientationFormDeltaEuler( glm::vec3 eulerAxisOrientChange )
{
	// How do we combine two matrices?
	// That's also how we combine quaternions...

	// So we want to "add" this change in oriention
	glm::quat qRotationChange = glm::quat( eulerAxisOrientChange );

	// Mulitply it by the current orientation;
	this->qOrientation = this->qOrientation * qRotationChange;

	return;
}

glm::vec3 cGameObject::getDirectionVector()
{
	glm::vec3 directionVector;
	directionVector = glm::eulerAngles( this->qOrientation );

	directionVector = glm::normalize( directionVector );
	return directionVector;
}

bool cGameObject::isFacingMe( glm::vec3 targetDirection, glm::vec3 targetPosition )
{
	// dotProduct( normalize( B - A ), normalize( directionFacingOfA ) )
	float facing = glm::dot( glm::normalize( this->position - targetPosition ), glm::normalize( targetDirection ) );

	if( facing < 0 )	// It's not facing, looking to opposite direction
	{
		return false;
	}
	else				// It's in the 180 degrees direction
	{
		if( facing >= 0.5f ) // It's in a 90 degrees cone
			return true;
		else
			return false;
	}
}