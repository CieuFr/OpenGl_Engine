#include "lab_work_manager.hpp"
#include "imgui.h"
#include "lab_work_1/lab_work_1.hpp"

namespace M3D_ISICG
{
	LabWorkManager::LabWorkManager()
	{
		_current = new LabWork1();
		_type	 = TYPE::LAB_WORK_1;
	}

	void LabWorkManager::drawMenu()
	{
		// Here you can add other lab works to the menu.
		if ( ImGui::MenuItem( "Lab work 1" ) )
		{
			if ( _type != TYPE::LAB_WORK_1 ) // Change only if needed.
			{
				// Keep window size.
				const int w = _current->getWindowWidth();
				const int h = _current->getWindowHeight();
				delete _current;			 // Delete old lab work.
				_current = new LabWork1();	 // Create new lab work.
				_type	 = TYPE::LAB_WORK_1; // Update type.
				_current->resize( w, h );	 // Update window size.
				_current->init();			 // Don't forget to call init().
			}
		}
	}
} // namespace M3D_ISICG
