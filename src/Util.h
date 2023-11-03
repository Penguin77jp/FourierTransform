#pragma once
#include <fstream>
#include <vector>
#include <iostream>
#include <format>
#include <Eigen/Eigen>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <numeric>

namespace Util {
	template <typename T>
	using DBL_VECTOR = std::vector<std::vector<T>>;
	template <typename T>
	DBL_VECTOR<T> DBL_VECTOR_INIT(const Eigen::Vector2i& size, const T dafultValue = T()) {
		return DBL_VECTOR<T>(size[0], std::vector<T>(size[1], dafultValue));
	}
	template <typename T>
	Eigen::Vector2i GetVectorSize(const DBL_VECTOR<T>& in) {
		return Eigen::Vector2i(in.size(), in[0].size());
	}

	namespace CSV {
		template <typename T>
		void WriteCSV(const std::string& path, const std::vector<T>& data) {
			std::ofstream output_file(path);
			for (int i = 0; i < data.size(); ++i) {
				output_file << data[i] << std::endl;
			}
		}

		template <typename T>
		std::vector<T> ReadCSV(const std::string& path) {
			std::ifstream input_file(path);
			if (input_file.fail()) {
				std::cerr << std::format("Failed to open file: {}", path) << std::endl;
				return std::vector<T>();
			}
			std::string str_line;
			std::vector<T> result_data;
			while (std::getline(input_file, str_line)) {
				const double val = std::stod(str_line);
				result_data.push_back(val);
			}
			return result_data;
		}
	}

	namespace Image {
		template <typename T>
		void SaveFromVector(const std::string& path, const DBL_VECTOR<T>& data) {
			double max = -std::numeric_limits<double>::max();
			const Eigen::Vector2i data_size = GetVectorSize(data);
			for (int uIndex = 0; uIndex < data_size[0]; ++uIndex) {
				for (int vIndex = 0; vIndex < data_size[1]; ++vIndex) {
					max = std::max(data[uIndex][vIndex], max);
				}
			}

			std::vector<unsigned char> image_data(data_size[0] * data_size[1], 0);
			for (int uIndex = 0; uIndex < data_size[0]; ++uIndex) {
				for (int vIndex = 0; vIndex < data_size[1]; ++vIndex) {
					image_data[(data_size[1] - vIndex - 1) * data_size[0] + uIndex] = (unsigned char)(255.0 * data[uIndex][vIndex] / max);
				}
			}
			stbi_write_png(path.c_str(), data_size[0], data_size[1], 1, image_data.data(), 0);
		}
	}
}