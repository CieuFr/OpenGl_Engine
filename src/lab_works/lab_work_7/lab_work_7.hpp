#ifndef __LAB_WORK_7_HPP__
#define __LAB_WORK_7_HPP__

#include "../common/TrackBallCamera.hpp"
#include "../common/camera.hpp"
#include "../common/models/base_model.hpp"
#include "../common/models/triangle_mesh.hpp"
#include "../common/models/triangle_mesh_model.hpp"
#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include <vector>

namespace M3D_ISICG
{
	class LabWork7 : public BaseLabWork
	{
	  public:
		LabWork7() : BaseLabWork() {}
		~LabWork7();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;

		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

	  private:
		BaseCamera _camera;

		void _updateViewMatrix();
		void _updateProjectionMatrix();
		void _initCamera();

		// =================TP 4 ==================

		TriangleMeshModel _tmm;

		// =================FIN TP 4 =============

		// =================AO  ==================

		Vec3f  lightPos;
		Vec3f lightColor;
		unsigned int ssaoFBO, ssaoBlurFBO;
		unsigned int		   gPosition, gNormal, gAlbedo;
		 unsigned int gBuffer;
		std::vector<glm::vec3> ssaoKernel;
		 unsigned int			noiseTexture;
		unsigned int		   ssaoColorBuffer, ssaoColorBufferBlur;

		// =================FIN AO =============

		GLuint transformationMatrix;
		Mat4f  _transformationMatrix = MAT4F_ID;
		Mat4f  _matrixWtoV			 = MAT4F_ID;
		Mat4f  _matrixVtoC			 = MAT4F_ID;

		GLuint aTranslationX;
		GLuint luminosityUint;

		GLfloat _time					= 0;
		GLfloat _luminosity				= 1;
		bool	luminosityNeedsUpdating = false;
		float	_fovy;
		bool	fovyNeedsUpdating = false;

		bool trackballSwitch = false;

		bool  perspecOrtho;
		bool  perspecNeedsUpdating = false;
		float _cameraSpeed		   = 0.1f;
		float _cameraSensitivity   = 0.1f;

		// ================ GL data.
		GLuint aProgram = GL_INVALID_VALUE;
		GLuint aProgram2 = GL_INVALID_VALUE;
		GLuint aProgram3 = GL_INVALID_VALUE;
		GLuint aProgram4 = GL_INVALID_VALUE;


		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_1_HPP__
