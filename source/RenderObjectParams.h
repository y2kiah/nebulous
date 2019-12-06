/* ----==== RENDEROBJECTPARAMS.H ====---- */

#pragma once

#include "Math Code/Vector3f.h"


///// STRUCTURES /////

class RenderObjectParams {
	friend class RenderObjectOwner;

	private:
		///// VARIABLES /////

		unsigned int	roID;				// RenderObject ID

	public:		

		// Render engine variables		
		unsigned int	indexBufferID;		// index of the indexBuffer to use
		int				shaderProgramID;	// shaders also are API specific inherited from an abstract base class
		bool			useDefaultFog;		// could also store individual fog settings here
		bool			envReflection;		// object requires environmental reflection - could be in material instead but maybe not
		bool			frustumCull;
		int				priority;
			// store bounding volume for frustum cull

		// Positioning variables
		Vector3f		worldPos;	// store transform matrix instead?
		Vector3f		angle;		// euler angles
			// store coordinate space
		Vector3f		scale;

		// Material variables
		// store color
		unsigned short	textureID[16];		// stage 0 - 15 textures	instead of all this, create materials and store material ID
		// store lighting type (diffuse, specular, etc)

		///// FUNCTIONS /////

		unsigned int getRoID() const { return roID; }

		void setupRender(int _indexBufferID, int _shaderProgramID, bool _useDefaultFog, bool _envReflection, bool _frustumCull) {
			indexBufferID = _indexBufferID;
			shaderProgramID = _shaderProgramID;
			useDefaultFog = _useDefaultFog;
			envReflection = _envReflection;
			frustumCull = _frustumCull;
		}

		void setTransform(const Vector3f &_p, const Vector3f &_a, const Vector3f &_s) {
			worldPos = _p;
			angle = _a;
			scale = _s;
		}

		void setTextureID(int _stage, unsigned short _textureID) // convert to setMaterialID
		{
			_ASSERTE(_stage >= 0 && _stage < 16);
			textureID[_stage] = _textureID;
		}

		// Operators
		void operator= (const RenderObjectParams &rop)	{
			roID = rop.roID;
			indexBufferID = rop.indexBufferID;
			shaderProgramID = rop.shaderProgramID;
			useDefaultFog = rop.useDefaultFog;
			envReflection = rop.envReflection;
			frustumCull = rop.frustumCull;
			priority = rop.priority;
			worldPos = rop.worldPos;
			angle = rop.angle;			
			scale = rop.scale;
			memcpy_s(textureID, sizeof(ushort) * 16, rop.textureID, sizeof(ushort) * 16);
		}

		// Constructors
		explicit RenderObjectParams() :
			roID(0),
			indexBufferID(0),
			shaderProgramID(-1),
			useDefaultFog(false),
			envReflection(false),
			frustumCull(false),
			priority(0),
			worldPos(0.0f,0.0f,0.0f),
			angle(0.0f,0.0f,0.0f),
			scale(0.0f,0.0f,0.0f)
		{	
			memset(textureID, 0, sizeof(ushort) * 16);
		}
		
		explicit RenderObjectParams(const RenderObjectParams &rop) :
			roID(rop.roID),
			indexBufferID(rop.indexBufferID),
			shaderProgramID(rop.shaderProgramID),
			useDefaultFog(rop.useDefaultFog),
			envReflection(rop.envReflection),
			frustumCull(rop.frustumCull),
			priority(rop.priority),
			worldPos(rop.worldPos),
			angle(rop.angle),
			scale(rop.scale)
		{	
			memset(textureID, 0, sizeof(ushort) * 16);
		}

		explicit RenderObjectParams(int _roID, int _indexBufferID, int _shaderProgramID, bool _useDefaultFog, bool _envReflection, bool _frustumCull) :
			roID(_roID),
			indexBufferID(_indexBufferID),
			shaderProgramID(_shaderProgramID),
			useDefaultFog(_useDefaultFog),
			envReflection(_envReflection),
			frustumCull(_frustumCull),
			priority(0),
			worldPos(0.0f,0.0f,0.0f),
			angle(0.0f,0.0f,0.0f),
			scale(0.0f,0.0f,0.0f)
		{	
			memset(textureID, 0, sizeof(ushort) * 16);
		}
};