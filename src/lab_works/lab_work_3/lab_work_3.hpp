#ifndef __LAB_WORK_3_HPP__
#define __LAB_WORK_3_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>
#include "../common/camera.hpp"

namespace M3D_ISICG
{
	class LabWork3 : public BaseLabWork
	{
		// MESH
		struct Mesh
		{
			std::vector<Vec3f>	  vectorPositions;
			std::vector<Vec3f>		  vectorColors;
			std::vector<unsigned int> vectorIndices;
			Mat4f					  transformationMatrice;
			GLuint					  VBOVertices = GL_INVALID_VALUE;
			GLuint					  VBOColors	  = GL_INVALID_VALUE;
			GLuint					  VAO		  = GL_INVALID_VALUE;
			GLuint					  EBO		  = GL_INVALID_VALUE;
		};

	  public:
		LabWork3() : BaseLabWork() {}
		~LabWork3();
		

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;
		

	  private:

		Camera _camera;

		Mesh _createCube();
		void _updateViewMatrix();
		void _updateProjectionMatrix();
		void _initCamera();
		// ================ Scene data.
		// ================

		Mesh _cube;
		GLuint matrixLtoW;
		GLuint matrixWtoVGluint;
		GLuint matrixVtoCGluint;
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
