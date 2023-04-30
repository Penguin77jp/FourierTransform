#include <vector>
#include <complex>
#include <numbers>
#include <iostream>
#include <format>

std::complex<double> image_i = { 0,1 };

int main() {
	std::vector<std::vector<double>> data_raw = { { 0,1,2,1,0 }, {0,2,4,2,0}, { 0,1,2,1,0 } };
	std::vector<std::vector<std::complex<double>>> data_converted(data_raw.size(), std::vector<std::complex<double>>(data_raw[0].size()));
	std::vector<std::vector<double>> data_resotre(data_raw.size(), std::vector<double>(data_raw[0].size()));
	const int sizeY = data_raw.size();
	const int sizeX = data_raw[0].size();

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

	return 0;
}