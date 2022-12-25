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
#include "victor_toolkit/basic_object_drawer.hpp"
#include "victor_toolkit/program_wrapper.hpp"
#include <random>
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
		bool initShadowMap();
		bool initDepthMap();
		bool initSkyBox();




		void renderLightingPass();
		void renderAOPass();
		void renderBlurPass();
		void renderGeometryPass();
		void renderDepthMapPass();
		void renderPrintDepthMap();



		void computeAO();
		void computerShadowMap();


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

		GLuint _gBufferFBO;
		GLuint _gBufferTextures[ 6 ];

		GLenum _aoDrawBuffer[ 1 ] = { GL_COLOR_ATTACHMENT0 };

		GLenum _depthMapDrawBuffer[ 1 ] = { GL_NONE };


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

		Vec3f				   lightColor = glm::vec3( 0.2, 0.2, 0.7 );
		GLuint				   ssaoFBO;
		GLuint				   ssaoBlurFBO; 
		std::vector<glm::vec3> ssaoKernel;
		GLuint				   noiseTexture;
		GLuint				   ssaoOutputTexture, blurOutputTexture;
		std::vector<glm::vec3> ssaoNoise;
		bool				   printDepth = false;

		int	  kernelSize = 64;
		float radius	 = 0.5;
		float bias		 = 0.025;
		int	  power		 = 1;

		// FIN SSAO

		//===========SHADOW MAP ============

		unsigned int SHADOW_WIDTH = 760, SHADOW_HEIGHT = 760;
		GLuint depthMapTexture;
		GLuint _shadowMapFBO;
		GLuint _depthMapFBO;
		float  lightX		   = -2.0f;
		float  lightY = 4.0f;
		float  lightZ		   = -1.0f;
		float		 orthoCoefficient = 5;

		Mat4f lightProjection = MAT4F_ID;
		float near_plane = 1.0f, far_plane = 7.5f;
		Mat4f lightView = MAT4F_ID;
		Mat4f lightSpaceMatrix = MAT4F_ID;

		//=========== FIN SHADOW MAP ============

		//=============SKYBOX ============
		
		GLuint skyboxTexture; 
		//==============================

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
		ProgramWrapper _programGeometryPass;
		ProgramWrapper _programLightingPass;
		ProgramWrapper _programSSAO;
		ProgramWrapper _programSSAOBlur;
		ProgramWrapper _programDepthMap;
		ProgramWrapper _programPrintDepthMap;



		// ================

		// ================ Settings.
		Vec4f _bgColor = Vec4f( 0.8f, 0.8f, 0.8f, 1.f ); // Background color
		// ================

		static const std::string _shaderFolder;
	};
} // namespace M3D_ISICG

#endif // __LAB_WORK_1_HPP__
