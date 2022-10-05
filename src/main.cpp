#include "application.hpp"
#include <iostream>

int main( int argc, char * argv[] )
{
	try
	{
		const std::string appTitle = "M1 ISICG - Moteur3D";
		const int		  width	   = 1280;
		const int		  height   = 720;

		M3D_ISICG::Application app( appTitle, width, height );

		return app.run();
	}
	catch ( const std::exception & e )
	{
		std::cerr << "Exception caught: " << std::endl << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}
