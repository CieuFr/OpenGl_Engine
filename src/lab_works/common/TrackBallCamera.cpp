
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "TrackBallCamera.hpp"
#include <iostream>


namespace M3D_ISICG
{
	//Vec2f( center.x + radius * glm::cos( angle ), center.y + radius * glm::sin( angle ) ) );
	 void TrackBallCamera::moveRight( const float p_delta )
	{
		 _azimuthAngle += p_delta;
		updateTrackBallCameraAfterMove();
	
	}

	 void TrackBallCamera::moveUp( const float p_delta )
	{ 
		_polarAngle += p_delta;
		updateTrackBallCameraAfterMove();

	}


	 void TrackBallCamera::moveFront( const float p_delta )
	{
		 BaseCamera::moveFront(p_delta);
		 updateTrackBallCameraAfterMove();
	}



	 void TrackBallCamera::rotate( const float p_yaw, const float p_pitch )
	{
	
	}

	 void TrackBallCamera::updateTrackBallCameraAfterMove() {

		 float sineAzimuth	  = sin( _azimuthAngle );
		 float		cosineAzimuth = cos( _azimuthAngle );
		 float	   sinePolar	   = sin( _polarAngle );
		 float		cosinePolar		= cos( _polarAngle );

		 Vec2f vecRadius = _position - _center;
		 float radius	 = pow( vecRadius.x * vecRadius.x + vecRadius.y * vecRadius.y, 0.5f );
		
		 float		x = _center.x + radius * cosinePolar * cosineAzimuth;
		 float		y = _center.y + radius * sinePolar;
		 float		z = _center.z + radius * cosinePolar * sineAzimuth;

		 Vec3f xyz = Vec3f( x, y, z );

		 setPosition( xyz );
		 setLookAt( Vec3f( 0.f, 0.f, 0.f ) );

	 }



} // namespace M3D_ISICG
