
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include "TrackBallCamera.hpp"
#include <iostream>


namespace M3D_ISICG
{

	// Vec2f( center.x + radius * glm::cos( angle ), center.y + radius * glm::sin( angle ) ) );
	 void TrackBallCamera::moveRight( const float p_delta )
	{
		 _azimuthAngle += p_delta;
		 _azimuthAngle = fmod( _azimuthAngle, 2 * pi );
		updateTrackBallCameraAfterMove();
	
	}

	 void TrackBallCamera::moveUp( const float p_delta )
	{
		 _polarAngle += p_delta;
		 _polarAngle = fmod( _polarAngle, 2 * pi );
		updateTrackBallCameraAfterMove();

	}


	 void TrackBallCamera::moveFront( const float p_delta )
	{
		/* std::cout << p_delta << "\n";
		 updateTrackBallCameraAfterMove();
		 BaseCamera::moveFront(p_delta);*/
	}



	 void TrackBallCamera::rotate( const float p_yaw, const float p_pitch )
	{
	
	}

	 void TrackBallCamera::updateTrackBallCameraAfterMove() {

		 float sineAzimuth	  = sin( _azimuthAngle );
		 float		cosineAzimuth = cos( _azimuthAngle );
		 float	   sinePolar	   = sin( _polarAngle );
		 float		cosinePolar		= cos( _polarAngle );
		
		 std::cout << "radius : : " << _radius << "\n";
		 std::cout << "center : " << _center.x << " y : " << _center.y << " z : " << _center.z << "\n";

		 if ( _radius == 0 )
			 _radius = 1;

		 float x = _center.x + _radius * cosinePolar * cosineAzimuth;
		 float y = _center.y + _radius * sinePolar;
		 float z = _center.z + _radius * cosinePolar * sineAzimuth;


		 Vec3f xyz = Vec3f( x, y, z );

		 setPosition( xyz );
		 setLookAt( _center );

		 std::cout << "Azi : " << _azimuthAngle << "\n";
		 std::cout << "POL : " << _polarAngle << "\n";

		 std::cout << "POSITION : " << xyz.x << " y : " << xyz.y << " z : " << xyz.z
				   << "\n";

	 }
	 void TrackBallCamera::switchCamera( Vec3f position ) {
		 setPosition( position ); 
		 if ( _radius == 0 )
			 _radius = 1;
		 _center = Vec3f( position.x - _radius, position.y, position.z );
	 }




} // namespace M3D_ISICG
