#ifndef __LAB_WORK_8_HPP__
#define __LAB_WORK_8_HPP__

#include "../common/TrackBallCamera.hpp"
#include "../common/camera.hpp"
#include "../common/models/base_model.hpp"
#include "../common/models/triangle_mesh.hpp"
#include "../common/models/triangle_mesh_model.hpp"
#include "GL/gl3w.h"
#include "common/base_lab_work.hpp"
#include "define.hpp"
#include "victor_toolkit/basic_object_drawer.hpp"
#include "victor_toolkit/program_wrapper.hpp"
#include <random>
#include <vector>

namespace M3D_ISICG
{
	class LabWork8 : public BaseLabWork
	{
	  public:
		LabWork8() : BaseLabWork() {}
		~LabWork8();

		bool init() override;
		void animate( const float p_deltaTime ) override;
		void render() override;
		void handleEvents( const SDL_Event & p_event ) override;
		void displayUI() override;

		// TP6
		void initGBuffer();
		// PARAM GBUFFER2
		GLuint positionTexture;
		GLuint normalTexture;
		GLuint ambientTexture;
		GLuint diffuseTexture;
		GLuint specularTexture;
		GLuint depthTexture;

		bool initLightingPassProgram();
		bool initAOPasses();

		void renderLightingPass();
		void renderAOPasses();

		void computeAO();


		BasicObjectDrawer drawer;

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

		// =================TP 4 ==================

		TriangleMeshModel _tmm;

		// =================FIN TP 4 =============

		// =================TP 6 ==================

		GLuint _fboId;
		GLuint _gBufferTextures[ 6 ];

		GLenum _drawBuffers[ 5 ] = { GL_COLOR_ATTACHMENT0,
									 GL_COLOR_ATTACHMENT1,
									 GL_COLOR_ATTACHMENT2,
									 GL_COLOR_ATTACHMENT3,
									 GL_COLOR_ATTACHMENT4 };

		const char * _listBox[ 5 ] = { "Attachment0", "Attachment1", "Attachment2", "Attachment3", "Attachment4" };
		int			 _listBoxSelectedValue = 0;

		GLuint quadVAO = 0;
		GLuint quadVBO;
		GLuint quadVBO2;
		GLuint quadEBO;

		bool lightPassEnabled = true;

		// =================FIN TP 6 =============


		// SSAO

		Vec3f lightColor = glm::vec3( 0.2, 0.2, 0.7 );
		GLuint									ssaoFBO;
		GLuint									ssaoBlurFBO;
		std::vector<glm::vec3>					ssaoKernel;
		GLuint									noiseTexture;
		GLuint									ssaoColorBuffer, ssaoColorBufferBlur;
		std::vector<glm::vec3>					ssaoNoise;

		// FIN SSAO


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
		GLuint		   aProgram				= GL_INVALID_VALUE;
		GLuint		   _lightingPassProgram = GL_INVALID_VALUE;
		ProgramWrapper programWrapper;
		ProgramWrapper programWrapper2;
		ProgramWrapper programWrapper3;
		ProgramWrapper programWrapper4;

		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_1_HPP__
