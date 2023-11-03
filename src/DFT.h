#pragma once
#include <vector>
#include <Eigen/Eigen>
#include <numbers>
#include <format>
#include <iostream>
#include "Util.h"

template <typename T>
class DFT {
public:
	DFT(const std::vector<T>& in_data);
	void InvDFT(std::vector<T>& out_data) const;
public:
	std::vector<Eigen::Matrix<T, 1, 2>> m_fourier_data;
};

class DFT2D {
public:
	template<typename T>
	DFT2D(const Util::DBL_VECTOR<T>& in_data);
	template<typename T>
	void InvDFT(Util::DBL_VECTOR<T>& out_data) const;
	void ComputeFourierItensity(Util::DBL_VECTOR<double>& out_vec, const bool center_shift = true) const;
public:
	Util::DBL_VECTOR<Eigen::Vector2d> m_fourier_data;
};

#include "DFT_impl.h"