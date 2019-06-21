/*
* LightFieldDisplayModel - UtilitiesBasic - MultiViewModel
*
* Descriptor for the MultiView display model.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#ifndef UTILITIESBASIC_MULTIVIEWMODEL_H
#define UTILITIESBASIC_MULTIVIEWMODEL_H

#include <string>
#include <vector>

// Assumptions:
//  * cameras are sorted by x-coordinate (increasing);
//  * all cameras are in front of screen (i.e., z>0).

struct MultiViewModel
{
	std::string name;
	int num_cameras = 0;

	int image_size_x = 0; // In pixels.
	int image_size_y = 0; // In pixels.

	float screen_size_x = 0.0f; // In millimeters.
	float screen_size_y = 0.0f; // In millimeters.

	std::vector<float> cameras_pos_x; // In millimeters.
	std::vector<float> cameras_pos_y; // In millimeters.
	std::vector<float> cameras_pos_z; // In millimeters.

	void Clear();
	bool Serialize( const std::string& file_path );
	bool Deserialize( const std::string& file_path );
};

#endif // UTILITIESBASIC_MULTIVIEWMODEL_H