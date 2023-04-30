#include <vector>
#include <complex>
#include <numbers>
#include <iostream>
#include <format>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <Eigen/Eigen>

std::complex<double> image_i = { 0,1 };

int main() {
	const std::string imagePath = "assets/image_low.jpg";
	std::vector<std::vector<Eigen::Vector3d>> data_raw;
	// load image
	{
		int width, height, comp;
		unsigned char* data = stbi_load(imagePath.c_str(), &width, &height, &comp, 0);
		std::cout << std::format("loaded iamge ({}x{}) comp:{}", width, height, comp) << std::endl;
		data_raw = std::vector<std::vector<Eigen::Vector3d>>(width,
			std::vector<Eigen::Vector3d>(height, Eigen::Vector3d(0, 0, 0))
		);
		for (int index = 0; index < width * height; ++index) {
			const int indexX = index % width;
			const int indexY = index / width;
			data_raw[indexX][indexY] = { (double)data[index * comp + 0], 0,0 };
		}
		stbi_image_free(data);
	}
	const int sizeX = data_raw.size();
	const int sizeY = data_raw[0].size();

	std::vector<std::vector<std::complex<double>>> data_converted(data_raw.size(), std::vector<std::complex<double>>(data_raw[0].size()));
	// fourier
	{
		std::cout << "compute fourier" << std::endl;
#pragma omp parallel for schedule(dynamic)
		for (int indexY = 0; indexY < sizeY; ++indexY) {
			std::cout << std::format("{} / {}", indexY, sizeY) << std::endl;
			for (int indexX = 0; indexX < sizeX; ++indexX) {
				std::complex<double> result = { 0,0 };

				for (int searchY = 0; searchY < sizeY; ++searchY) {
					for (int searchX = 0; searchX < sizeX; ++searchX) {
						double tmpY = 2.0 * std::numbers::pi * searchY * indexY / sizeY;
						double tmpX = 2.0 * std::numbers::pi * searchX * indexX / sizeX;
						result += data_raw[searchX][searchY][0] * std::exp(image_i * tmpY) * std::exp(image_i * tmpX) / (double)(sizeY * sizeX);
					}
				}
				data_converted[indexX][indexY] = result;
			}
		}

		std::cout << "output image(fourier)" << std::endl;
		std::vector<unsigned char> tmpImage(sizeX * sizeY * 3);
		double min = data_converted[0][0].real();
		double max = data_converted[0][0].real();
		for (int indexY = 0; indexY < sizeY; ++indexY) {
			for (int indexX = 0; indexX < sizeX; ++indexX) {
				const int index = indexY * sizeX * 3 + indexX * 3;
				auto v1 = data_converted[indexX][indexY].real();
				auto v2 = data_converted[indexX][indexY].imag();
				min = std::min(min, std::min(v1, v2));
				max = std::max(max, std::min(v1, v2));
			}
		}
		std::cout << "max :" << max << std::endl;
		for (int indexY = 0; indexY < sizeY; ++indexY) {
			for (int indexX = 0; indexX < sizeX; ++indexX) {
				const int index = indexY * sizeX * 3 + indexX * 3;
				tmpImage[index + 0] = 255 * data_converted[indexX][indexY].real() / max;
				tmpImage[index + 1] = 255 * data_converted[indexX][indexY].imag() / max;
				tmpImage[index + 2] = 0;
			}
		}
		stbi_write_bmp("fourier.bmp", sizeX, sizeY, 3, tmpImage.data());
	}
	std::vector<std::vector<double>> data_resotre(data_raw.size(), std::vector<double>(data_raw[0].size()));
	// inv fourier
	{
		std::cout << "compute fourier" << std::endl;
#pragma omp parallel for schedule(dynamic)
		for (int indexY = 0; indexY < sizeY; ++indexY) {
			std::cout << std::format("{} / {}", indexY, sizeY) << std::endl;
			for (int indexX = 0; indexX < sizeX; ++indexX) {
				double result = 0;
				for (int searchY = 0; searchY < sizeY; ++searchY) {
					for (int searchX = 0; searchX < sizeX; ++searchX) {
						double tmpY = 2.0 * std::numbers::pi * searchY * indexY / sizeY;
						double tmpX = 2.0 * std::numbers::pi * searchX * indexX / sizeX;
						auto result_com = data_converted[searchX][searchY] * std::exp(-image_i * tmpY) * std::exp(-image_i * tmpX);
						result += result_com.real();
					}
				}
				data_resotre[indexX][indexY] = result;
			}
		}
	}

	std::vector<unsigned char> out_image(sizeX * sizeY * 3, 0);
	{
		std::cout << "output image" << std::endl;
		for (int indexY = 0; indexY < sizeY; ++indexY) {
			for (int indexX = 0; indexX < sizeX; ++indexX) {
				const int index = indexY * sizeX * 3 + indexX * 3;
				out_image[index + 0] = data_resotre[indexX][indexY];
				out_image[index + 1] = data_resotre[indexX][indexY];
				out_image[index + 2] = data_resotre[indexX][indexY];
			}
		}
		stbi_write_bmp("result.bmp", sizeX, sizeY, 3, out_image.data());
	}

	/*

	for (int indexY = 0; indexY < sizeY; ++indexY) {
		for (int indexX = 0; indexX < sizeX; ++indexX) {
			std::complex<double> result = { 0,0 };

			for (int searchY = 0; searchY < sizeY; ++searchY) {
				for (int searchX = 0; searchX < sizeX; ++searchX) {
					double tmpY = 2.0 * std::numbers::pi * searchY * indexY / sizeY;
					double tmpX = 2.0 * std::numbers::pi * searchX * indexX / sizeX;
					result += data_raw[searchY][searchX] * std::exp(image_i * tmpY) * std::exp(image_i * tmpX) / (double)(sizeY * sizeX);
				}
			}
			data_converted[indexY][indexX] = result;
		}
	}

	for (int indexY = 0; indexY < sizeY; ++indexY) {
		for (int indexX = 0; indexX < sizeX; ++indexX) {
			double result = 0;
			for (int searchY = 0; searchY < sizeY; ++searchY) {
				for (int searchX = 0; searchX < sizeX; ++searchX) {
					double tmpY = 2.0 * std::numbers::pi * searchY * indexY / sizeY;
					double tmpX = 2.0 * std::numbers::pi * searchX * indexX / sizeX;
					auto result_com = data_converted[searchY][searchX] * std::exp(-image_i * tmpY) * std::exp(-image_i * tmpX);
					result += result_com.real();
				}
			}
			data_resotre[indexY][indexX] = result;
		}
	}

	std::cout << "data converted" << std::endl;
	for (int indexY = 0; indexY < sizeY; ++indexY) {
		for (int indexX = 0; indexX < sizeX; ++indexX) {
			std::cout << std::format("({}, {}j) ", data_converted[indexY][indexX].real(), data_converted[indexY][indexX].imag());
		}
		std::cout << std::endl;
	}

	std::cout << "data restore" << std::endl;
	for (int indexY = 0; indexY < sizeY; ++indexY) {
		for (int indexX = 0; indexX < sizeX; ++indexX) {
			std::cout << std::format("{:.2f} ", data_resotre[indexY][indexX]);
		}
		std::cout << std::endl;
	}
	*/

	return 0;
}