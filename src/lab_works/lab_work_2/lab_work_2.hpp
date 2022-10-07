#ifndef __LAB_WORK_2_HPP__
#define __LAB_WORK_2_HPP__

#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork2 : public BaseLabWork
	{
	  public:
		LabWork2() : BaseLabWork() {}
		~LabWork2();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		// ================ Scene data.
		// ================
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
