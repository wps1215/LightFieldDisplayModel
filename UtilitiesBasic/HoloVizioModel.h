/*
* LightFieldDisplayModel - UtilitiesBasic - HoloVizioModel
*
* Descriptor for the HoloVizio display model.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#ifndef UTILITIESBASIC_HOLOVIZIOMODEL_H
#define UTILITIESBASIC_HOLOVIZIOMODEL_H

#include <string>
#include <vector>

// Assumptions:
//  * projectors are sorted by x-coordinate (increasing);
//  * all projectors are behind the screen (i.e., z<0).

struct HoloVizioModel
{
	std::string name;
	int num_projectors = 0;

	int image_size_x = 0; // In pixels.
	int image_size_y = 0; // In pixels.

	float observer_distance = 0.0f; // In millimeters.
	float screen_size_x = 0.0f; // In millimeters.
	float screen_size_y = 0.0f; // In millimeters.

	float angular_scattering = 0.0f; // In radians.

	std::vector<float> projectors_pos_x;
	std::vector<float> projectors_pos_y;
	std::vector<float> projectors_pos_z;

	void Clear();
	bool Serialize( const std::string& file_path );
	bool Deserialize( const std::string& file_path );
};

#endif // UTILITIESBASIC_HOLOVIZIOMODEL_H