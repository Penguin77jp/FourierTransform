#pragma once
#include "DFT.h"

template<typename T>
DFT<T>::DFT(const std::vector<T>& in_data) {
	const int size_data = in_data.size();
	m_fourier_data.reserve(size_data);
	for (int k = 0; k < size_data; ++k) {
		Eigen::Matrix<T, 1, 2> el_fourier_data = { T(0), T(0) };
		for (int n = 0; n < size_data; ++n) {
			const double theta = 2.0 * std::numbers::pi * k * n / size_data;
			el_fourier_data[0] += in_data[n] * std::cos(theta);
			el_fourier_data[1] += -1.0 * in_data[n] * std::sin(theta);
		}
		m_fourier_data.push_back(el_fourier_data);
	}
}

template<typename T>
void DFT<T>::InvDFT(std::vector<T>& out_data) const {
	const int size_data = m_fourier_data.size();
	out_data = std::vector<T>(size_data);
	for (int n = 0; n < size_data; ++n) {
		double result_real = 0.0;
		double result_image = 0.0;
		for (int fourier_index = 0; fourier_index < size_data; ++fourier_index) {
			const double theta = 2.0 * std::numbers::pi * fourier_index * n / size_data;
			const T f_real = m_fourier_data[fourier_index][0];
			const T f_image = m_fourier_data[fourier_index][1];
			result_real += f_real * std::cos(theta) - f_image * std::sin(theta);
			result_image += f_image * std::cos(theta) + f_real * std::sin(theta);
		}
		result_real /= size_data;
		result_image /= size_data;
		out_data[n] = result_real;
	}
}

template<typename T>
DFT2D::DFT2D(const Util::DBL_VECTOR<T>& in_data) {
	const Eigen::Vector2i size_data = Util::GetVectorSize(in_data);
	m_fourier_data = Util::DBL_VECTOR_INIT<Eigen::Vector2d>(size_data);
	for (int fourier_index0 = 0; fourier_index0 < size_data[0]; ++fourier_index0) {
		std::cout << std::format("{}/{}\n", fourier_index0, size_data[0]);
#pragma omp parallel for
		for (int fourier_index1 = 0; fourier_index1 < size_data[1]; ++fourier_index1) {
			Eigen::Vector2d el_fourier_data = { 0,0 };
			for (int data_index0 = 0; data_index0 < size_data[0]; ++data_index0) {
				for (int data_index1 = 0; data_index1 < size_data[1]; ++data_index1) {
					const double theta = 2.0 * std::numbers::pi * (1.0 * fourier_index0 * data_index0 / size_data[0] + 1.0 * fourier_index1 * data_index1 / size_data[1]);
					el_fourier_data[0] += in_data[data_index0][data_index1] * std::cos(theta);
					el_fourier_data[1] += -1.0 * in_data[data_index0][data_index1] * std::sin(theta);
				}
			}
			m_fourier_data[fourier_index0][fourier_index1] = el_fourier_data;
		}
	}
}

template<typename T>
void DFT2D::InvDFT(Util::DBL_VECTOR<T>& out_data) const {
	const Eigen::Vector2i size_data = Util::GetVectorSize(m_fourier_data);
	out_data = Util::DBL_VECTOR_INIT<T>(size_data);
	for (int data_index0 = 0; data_index0 < size_data[0]; ++data_index0) {
		std::cout << std::format("{}/{}\n", data_index0, size_data[0]);
#pragma omp parallel for
		for (int data_index1 = 0; data_index1 < size_data[1]; ++data_index1) {
			double result_real = 0.0;
			double result_image = 0.0;
			const double fourier_index_max = 1.0;
			for (int fourier_index0 = 0; fourier_index0 < size_data[0] * fourier_index_max; ++fourier_index0) {
				for (int fourier_index1 = 0; fourier_index1 < size_data[1] * fourier_index_max; ++fourier_index1) {
					const double theta = 2.0 * std::numbers::pi * (1.0 * fourier_index0 * data_index0 / size_data[0] + 1.0 * fourier_index1 * data_index1 / size_data[1]);
					const double f_real = m_fourier_data[fourier_index0][fourier_index1][0];
					const double f_image = m_fourier_data[fourier_index0][fourier_index1][1];
					result_real += f_real * std::cos(theta) - f_image * std::sin(theta);
					result_image += f_image * std::cos(theta) + f_real * std::sin(theta);
				}
			}
			result_real /= (size_data[0] * size_data[1]);
			result_image /= (size_data[0] * size_data[1]);
			out_data[data_index0][data_index1] = T(result_real);
		}
	}
}

inline void DFT2D::ComputeFourierItensity(Util::DBL_VECTOR<double>& out_vec, const bool center_shift) const {
	const Eigen::Vector2i data_size = Util::GetVectorSize(m_fourier_data);
	out_vec = Util::DBL_VECTOR_INIT<double>(data_size);
	for (int uIndex = 0; uIndex < data_size[0]; ++uIndex) {
		for (int vIndex = 0; vIndex < data_size[1]; ++vIndex) {
			const Eigen::Vector2d& val = m_fourier_data[uIndex][vIndex];
			const double val_norm = val.norm();
			if (center_shift) {
				const int out_image_uindex = (uIndex + data_size[0] / 2) % data_size[0];
				const int out_image_vindex = (vIndex + data_size[1] / 2) % data_size[1];
				out_vec[out_image_uindex][out_image_vindex] = val_norm;
			}
			else {
				out_vec[uIndex][vIndex] = val_norm;
			}
		}
	}
}