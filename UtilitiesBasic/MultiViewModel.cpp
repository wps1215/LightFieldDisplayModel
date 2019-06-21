/*
* LightFieldDisplayModel - UtilitiesBasic - MultiViewModel
*
* Descriptor for the MultiView display model.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "MultiViewModel.h"
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



void MultiViewModel::Clear()
{
	name = std::string();
	num_cameras = 0;

	image_size_x = 0;
	image_size_y = 0;

	screen_size_x = 0.0f;
	screen_size_y = 0.0f;

	cameras_pos_x.clear();
	cameras_pos_y.clear();
	cameras_pos_z.clear();
}



bool MultiViewModel::Serialize( const std::string& file_path )
{
	std::fstream filestream;
	nlohmann::json json;

	bool success = true;

	try
	{
		json["name"] = name;
		json["num_cameras"] = num_cameras;

		json["image_size_x"] = image_size_x;
		json["image_size_y"] = image_size_y;

		json["screen_size_x"] = screen_size_x;
		json["screen_size_y"] = screen_size_y;

		json["cameras_pos_x"] = nlohmann::json( cameras_pos_x );
		json["cameras_pos_y"] = nlohmann::json( cameras_pos_y );
		json["cameras_pos_z"] = nlohmann::json( cameras_pos_z );

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



bool MultiViewModel::Deserialize( const std::string& file_path )
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
		num_cameras = json["num_cameras"].get<int>();

		image_size_x = json["image_size_x"].get<int>();
		image_size_y = json["image_size_y"].get<int>();

		screen_size_x = json["screen_size_x"].get<float>();
		screen_size_y = json["screen_size_y"].get<float>();

		parse_json_array<float>( cameras_pos_x, json["cameras_pos_x"] );
		parse_json_array<float>( cameras_pos_y, json["cameras_pos_y"] );
		parse_json_array<float>( cameras_pos_z, json["cameras_pos_z"] );
	}
	catch ( ... )
	{
		success = false;
	}

	filestream.close();

	return success;
}