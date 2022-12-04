#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>
#include "../common/camera.hpp"
#include "../common/TrackBallCamera.hpp"
#include "utils/random.hpp"
#include "victor_toolkit/program_wrapper.hpp"




namespace M3D_ISICG
{

	class Mesh
	{
	  public: 
		
		Mesh() {};
		~Mesh() {};
	  
		std::vector<Vec3f>		  vectorPositions;
		std::vector<Vec3f>		  vectorColors;
		std::vector<unsigned int> vectorIndices;
		Mat4f					  transformationMatrice = MAT4F_ID;
		GLuint					  VBOVertices			= GL_INVALID_VALUE;
		GLuint					  VBOColors				= GL_INVALID_VALUE;
		GLuint					  VAO					= GL_INVALID_VALUE;
		GLuint					  EBO					= GL_INVALID_VALUE;
	
	};

	class SceneGraph
	{	
	public:
		
		Mesh _createCube()
		{
			Mesh cube;
			cube.vectorPositions.push_back( Vec3f( 0.5f, 0.5f, -0.5f ) );
			cube.vectorPositions.push_back( Vec3f( 0.5f, -0.5f, -0.5f ) );
			cube.vectorPositions.push_back( Vec3f( -0.5f, -0.5f, -0.5f ) );
			cube.vectorPositions.push_back( Vec3f( -0.5f, 0.5f, -0.5f ) );
			cube.vectorPositions.push_back( Vec3f( 0.5f, 0.5f, 0.5f ) );
			cube.vectorPositions.push_back( Vec3f( 0.5f, -0.5f, 0.5f ) );
			cube.vectorPositions.push_back( Vec3f( -0.5f, -0.5f, 0.5f ) );
			cube.vectorPositions.push_back( Vec3f( -0.5f, 0.5f, 0.5f ) );
			cube.vectorColors.push_back( getRandomVec3f() );
			cube.vectorColors.push_back( getRandomVec3f() );
			cube.vectorColors.push_back( getRandomVec3f() );
			cube.vectorColors.push_back( getRandomVec3f() );
			cube.vectorColors.push_back( getRandomVec3f() );
			cube.vectorColors.push_back( getRandomVec3f() );
			cube.vectorColors.push_back( getRandomVec3f() );
			cube.vectorColors.push_back( getRandomVec3f() );

			for ( int i = 0; i < 4; i++ )
			{
				cube.vectorIndices.push_back( i );
				cube.vectorIndices.push_back( ( i + 1 ) % 4 );
				cube.vectorIndices.push_back( i + 4 );
				cube.vectorIndices.push_back( ( ( i + 1 ) % 4 ) + 4 );
				cube.vectorIndices.push_back( ( i + 1 ) % 4 );
				cube.vectorIndices.push_back( i + 4 );
			}

			// BAS
			cube.vectorIndices.push_back( 0 );
			cube.vectorIndices.push_back( 1 );
			cube.vectorIndices.push_back( 2 );
			cube.vectorIndices.push_back( 0 );
			cube.vectorIndices.push_back( 2 );
			cube.vectorIndices.push_back( 3 );

			// HAUT
			cube.vectorIndices.push_back( 4 );
			cube.vectorIndices.push_back( 5 );
			cube.vectorIndices.push_back( 6 );
			cube.vectorIndices.push_back( 4 );
			cube.vectorIndices.push_back( 6 );
			cube.vectorIndices.push_back( 7 );

			glCreateBuffers( 1, &cube.VBOVertices );
			glCreateBuffers( 1, &cube.EBO );
			glCreateBuffers( 1, &cube.VBOColors );

			glNamedBufferData( cube.VBOVertices,
							   cube.vectorPositions.size() * sizeof( Vec3f ),
							   cube.vectorPositions.data(),
							   GL_STATIC_DRAW );

			glNamedBufferData(
				cube.VBOColors, cube.vectorColors.size() * sizeof( Vec3f ), cube.vectorColors.data(), GL_STATIC_DRAW );

			glNamedBufferData(
				cube.EBO, cube.vectorIndices.size() * sizeof( int ), cube.vectorIndices.data(), GL_STATIC_DRAW );

			glCreateVertexArrays( 1, &cube.VAO );

			glEnableVertexArrayAttrib( cube.VAO, 0 );
			glVertexArrayAttribFormat( cube.VAO, 0, 3, GL_FLOAT, GL_FALSE, 0 );
			glVertexArrayVertexBuffer( cube.VAO, 0, cube.VBOVertices, 0, sizeof( Vec3f ) );
			glVertexArrayAttribBinding( cube.VAO, 0, 0 );

			glEnableVertexArrayAttrib( cube.VAO, 1 );
			glVertexArrayAttribFormat( cube.VAO, 1, 3, GL_FLOAT, GL_TRUE, 0 );
			glVertexArrayVertexBuffer( cube.VAO, 1, cube.VBOColors, 0, sizeof( Vec3f ) );
			glVertexArrayAttribBinding( cube.VAO, 1, 1 );

			glVertexArrayElementBuffer( cube.VAO, cube.EBO );

			// cube.transformationMatrice = Mat4f( 1.0f );
			cube.transformationMatrice = glm::scale( cube.transformationMatrice, Vec3f( 0.8f, 0.8f, 0.8f ) );

			return cube;
		}
		
	 
		SceneGraph() { parent = NULL; };

		~SceneGraph()
		{
			for ( unsigned int i = 0; i < children.size(); ++i )
			{
				delete children[ i ];
			}
		};
		
		void			  SetMesh(  Mesh meshParam ) { mesh = meshParam; }
		Mesh GetMesh()  { return mesh; }

		void				 SetParent( SceneGraph * parentParam ) { parent = parentParam; }
		SceneGraph * GetParent()  { return parent; }

		void			  SetTransform( const glm::mat4 & matrix ) { transform = matrix; }
		glm::mat4 & GetTransform()  { return transform; }

		void		SetWorldTransform( const glm::mat4 & matrix ) { worldTransform = matrix; }
		glm::mat4 & GetWorldTransform() { return worldTransform; }

		
		void			  AddChild( SceneGraph * child )
		{
			children.push_back( child );
			child->parent = this;
		}
		void Update()
		{
			if ( parent )
			{
				worldTransform = parent->worldTransform * transform;
			}
			else
			{
				worldTransform = transform;
			}
			for ( std::vector<SceneGraph *>::iterator it = children.begin(); it != children.end(); ++it )
			{
				( *it )->Update();
			}
		}
		
	  private:
	  protected:
		Mesh					   mesh;
		std ::vector<SceneGraph *> children;
		SceneGraph *			   parent;
		glm::mat4				   worldTransform = MAT4F_ID;
		glm::mat4				   transform	  = MAT4F_ID;
		// glm::vec3	modelScale;
	};

	class LabWork3 : public BaseLabWork
	{
		
	public:

	 
		LabWork3() : BaseLabWork() {}
		~LabWork3();
		

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		

	  private:

		BaseCamera *	_camera = &_baseCamera;
		TrackBallCamera _trackBallCamera;
		BaseCamera		_baseCamera;

		bool trackBallEnabled		= false;
		bool trackBallNeedsUpdating = false;
		
		void _updateViewMatrix();
		void _updateProjectionMatrix();
		void _initCamera();
		// ================ Scene data.
		// ================

		Mesh _cube;
		Mesh	 _cube2;
		Mesh	 _cube3;
		
		ProgramWrapper programWrapper;

		SceneGraph _graphCube1;
		SceneGraph _graphCube2;
		SceneGraph _graphCube3;

		GLuint transformationMatrix;
		Mat4f  _transformationMatrix = MAT4F_ID;
		Mat4f  _matrixWtoV = MAT4F_ID;
		Mat4f  _matrixVtoC = MAT4F_ID;

		std::vector<Vec2f> triangleVertices;
		std::vector<Vec4f> triangleColors;
		std::vector<int>   eboPositions;
		GLuint			   aTranslationX;
		GLuint			   luminosityUint;
		

		GLfloat			   _time = 0;
		GLfloat			   _luminosity = 1;
		bool			   luminosityNeedsUpdating = false;
		float			   _fovy;
		bool			   fovyNeedsUpdating = false;

		bool trackballSwitch = false;

		bool perspecOrtho;
		bool perspecNeedsUpdating = false;
		float _cameraSpeed		   = 0.1f;
		float _cameraSensitivity   = 0.1f;

		// ================ GL data.
		GLuint aProgram = GL_INVALID_VALUE;
		GLuint VBO		= GL_INVALID_VALUE;
		GLuint VBO2		= GL_INVALID_VALUE;
		GLuint VAO		= GL_INVALID_VALUE;
		GLuint EBO		= GL_INVALID_VALUE;
		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;

		
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_1_HPP__
