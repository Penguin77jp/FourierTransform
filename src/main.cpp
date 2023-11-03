#include <iostream>
#include <format>
#include <string>
#include <fstream>
#include "Util.h"
#include "DFT.h"

int main(int arc, char* argv[]) {
	if (false) {
		std::vector<double> in_data;
		{
			const std::string csv_path = argv[1];
			in_data = Util::CSV::ReadCSV<double>(csv_path);
		}
		DFT<double> dft(in_data);
		for (int i = 0; i < dft.m_fourier_data.size(); ++i) {
			//std::cout << std::format("dft.m_fourier_data[{}] = {},{}", i, dft.m_fourier_data[i][0], dft.m_fourier_data[i][1]) << std::endl;
		}

		std::vector<double> result;
		dft.InvDFT(result);
		Util::CSV::WriteCSV<double>("result.csv", result);
	}

	if (true) {
		const std::string image_path = argv[1];
		int width, height, channels;
		unsigned char* image_data = stbi_load(image_path.c_str(), &width, &height, &channels, 0);
		if (image_data == nullptr) {
			std::cout << "Failed to load image." << std::endl;
			return -1;
		}
		std::cout << std::format("width = {}, height = {}, channels = {}", width, height, channels) << std::endl;

		Util::DBL_VECTOR<unsigned char> in_data;
		in_data = Util::DBL_VECTOR_INIT<unsigned char>({ width, height });
		for (int uIndex = 0; uIndex < width; ++uIndex) {
			for (int vIndex = 0; vIndex < height; ++vIndex) {
				in_data[uIndex][vIndex] = image_data[channels * width * vIndex + channels * uIndex];
			}
		}
		{
			std::vector<unsigned char> result_image;
			result_image.reserve(width * height);
			for (int i = 0; i < width * height; ++i) {
				const int uIndex = i % width;
				const int vIndex = i / width;
				result_image.push_back(in_data[uIndex][vIndex]);
			}
			stbi_write_png("input_gray.png", width, height, 1, result_image.data(), 0);
		}
		stbi_image_free(image_data);
		DFT2D dft2d(in_data);
		for (int fourier_idnex0 = 0; fourier_idnex0 < dft2d.m_fourier_data.size(); ++fourier_idnex0) {
			for (int fourier_idnex1 = 0; fourier_idnex1 < dft2d.m_fourier_data[0].size(); ++fourier_idnex1) {
				std::cout << std::format("[{}][{}] = real:{},image{}\n",
					fourier_idnex0,
					fourier_idnex1,
					dft2d.m_fourier_data[fourier_idnex0][fourier_idnex1][0],
					dft2d.m_fourier_data[fourier_idnex0][fourier_idnex1][1]
				);
			}
		}
		{
			// output fourier intensity as image
			Util::DBL_VECTOR<double> intensity;
			dft2d.ComputeFourierItensity(intensity);
			Util::Image::SaveFromVector("fourier_intensity.png", intensity);
		}

		Util::DBL_VECTOR<unsigned char> result;
		dft2d.InvDFT(result);
		std::vector<unsigned char> result_image;
		result_image.reserve(width * height);
		for (int i = 0; i < width * height; ++i) {
			const int uIndex = i % width;
			const int vIndex = i / width;
			result_image.push_back( unsigned char(result[uIndex][vIndex]) );
		}
		stbi_write_png("result.png", width, height, 1, result_image.data(), 0);
	}

	return 0;
}