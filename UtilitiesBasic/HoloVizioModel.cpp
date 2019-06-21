/*
* LightFieldDisplayModel - UtilitiesBasic - HoloVizioModel
*
* Descriptor for the HoloVizio display model.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "HoloVizioModel.h"
#include "json.hpp"

#include <fstream>
#include <iomanip>


template<typename BaseType>
void parse_json_array( std::vector<BaseType>& output, nlohmann::json& input )
{
	output.resize( input.size() );
	for ( int i = 0; i < input.size(); ++i )
	{
		output[i] = input[i].get<BaseType>();
	}
}



void HoloVizioModel::Clear()
{
	name = std::string();
	num_projectors = 0;

	image_size_x = 0;
	image_size_y = 0;

	observer_distance = 0.0f;
	screen_size_x = 0.0f;
	screen_size_y = 0.0f;

	angular_scattering = 0.0f;

	projectors_pos_x.clear();
	projectors_pos_y.clear();
	projectors_pos_z.clear();
}



bool HoloVizioModel::Serialize( const std::string& file_path )
{
	std::fstream filestream;
	nlohmann::json json;

	bool success = true;

	try
	{
		json["name"] = name;
		json["num_projectors"] = num_projectors;

		json["image_size_x"] = image_size_x;
		json["image_size_y"] = image_size_y;

		json["observer_distance"] = observer_distance;
		json["screen_size_x"] = screen_size_x;
		json["screen_size_y"] = screen_size_y;

		json["angular_scattering"] = angular_scattering;

		json["projectors_pos_x"] = nlohmann::json( projectors_pos_x );
		json["projectors_pos_y"] = nlohmann::json( projectors_pos_y );
		json["projectors_pos_z"] = nlohmann::json( projectors_pos_z );

		filestream.open( file_path, std::ofstream::out );
		filestream << std::setw( 4 ) << json << std::endl;
		filestream.close();
	}
	catch ( ... )
	{
		success = false;
	}

	filestream.close();

	return success;
}



bool HoloVizioModel::Deserialize( const std::string& file_path )
{
	std::fstream filestream;
	nlohmann::json json;

	bool success = true;

	try
	{
		filestream.open( file_path, std::ofstream::in );
		filestream >> json;
		filestream.close();

		Clear();

		name = json["name"].get<std::string>();
		num_projectors = json["num_projectors"].get<int>();

		image_size_x = json["image_size_x"].get<int>();
		image_size_y = json["image_size_y"].get<int>();

		observer_distance = json["observer_distance"].get<float>();
		screen_size_x = json["screen_size_x"].get<float>();
		screen_size_y = json["screen_size_y"].get<float>();

		angular_scattering = json["angular_scattering"].get<float>();

		parse_json_array<float>( projectors_pos_x, json["projectors_pos_x"] );
		parse_json_array<float>( projectors_pos_y, json["projectors_pos_y"] );
		parse_json_array<float>( projectors_pos_z, json["projectors_pos_z"] );
	}
	catch ( ... )
	{
		success = false;
	}

	filestream.close();

	return success;
}