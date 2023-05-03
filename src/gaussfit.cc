#include <cmath>
#include <iostream>
#include <fmt/format.h>
#include <Eigen/Dense>
#include <unsupported/Eigen/NumericalDiff>
#include <unsupported/Eigen/LevenbergMarquardt>

constexpr int kSize = 100;

// both of these are from https://en.wikipedia.org/wiki/Gaussian_function
// repeating pow instead of repeating divde cos div is more expensive
double gaussian(double x, double amplitude, double mean, double std) {
	return amplitude * std::exp(-std::pow(x - mean, 2) / (2 * std::pow(std, 2)));
}

// taking the 2 out to avoid an extra multiply in the denominator
double gaussian2d(double x, double y, double amplitude, double mean_x, double mean_y, double sigma) {
	double arg = pow((x - mean_x) / sigma, 2) + pow((y - mean_y) / sigma, 2);
	return amplitude * exp(-arg / 2.0);
}

// Functors for Gaussian fitting
struct GaussianFunctor : Eigen::DenseFunctor<double> {
		const Eigen::VectorXd &x_data;
		const Eigen::VectorXd &y_data;

		GaussianFunctor(const Eigen::VectorXd &x_data_in, const Eigen::VectorXd &y_data_in)
				: Eigen::DenseFunctor<double>(3, static_cast<int>(y_data_in.size())), x_data(x_data_in), y_data(y_data_in) {}

		int operator()(const Eigen::VectorXd &params, Eigen::VectorXd &fvec) const {
			double amplitude_est = params(0);
			double mean_est      = params(1);
			double std_est       = params(2);
			fmt::print("Estimates: amp={:.3f}, mean={:.3f}, std={:.3f}\n", amplitude_est, mean_est, std_est);
			for (int i = 0; i < values(); ++i) {
				double out_fit = gaussian(x_data(i), amplitude_est, mean_est, std_est);
				fvec(i)        = out_fit - y_data(i);
			}
			return 0;
		}
		// df() that implements the jacobian matrix will be added by NumericalDiff
};

struct GaussianFunctor2d : Eigen::DenseFunctor<double> {
		const Eigen::Matrix<double, Eigen::Dynamic, 3> &data;

		GaussianFunctor2d(const Eigen::Matrix<double, Eigen::Dynamic, 3> &data_in)
				: Eigen::DenseFunctor<double>(4, static_cast<int>(data_in.rows())), data(data_in) {}

		int operator()(const Eigen::VectorXd &params, Eigen::VectorXd &fvec) const {
			double amplitude_est = params(0);
			double mean_est_x    = params(1);
			double mean_est_y    = params(2);
			double std_est       = params(3);
			fmt::print(
				"Estimates: amp={:.3f}, mean={:.3f},{:.3f}, std={:.3f}\n", amplitude_est, mean_est_x, mean_est_y, std_est);
			for (int i = 0; i < values(); ++i) {
				double out_fit = gaussian2d(data(i, 0), data(i, 1), amplitude_est, mean_est_x, mean_est_y, std_est);
				fvec(i)        = out_fit - data(i, 2);
			}
			return 0;
		}
		// df() that implements the jacobian matrix will be added by NumericalDiff
};

int main() {
	// 1d gaussian
	fmt::print("trying 1d gaussian fitting\n");
	// Generate sample data
	Eigen::VectorXd x_data(kSize);
	Eigen::VectorXd y_data(kSize);

	double amplitude_est = 1.0;
	double mean_est      = 50.0;
	double std_est       = 10.0;

	for (int i = 0; i < kSize; ++i) {
		x_data(i) = i;
		y_data(i) =
			gaussian(x_data(i), amplitude_est, mean_est, std_est) + 0.05 * (static_cast<double>(rand()) / (double)RAND_MAX);
	}

	// Initial guess for Gaussian parameters
	Eigen::VectorXd initial_params(3);
	initial_params << amplitude_est + 5, mean_est + 30, std_est + 20;

	// Add the df() method to GaussianFunctor using NumericalDiff
	GaussianFunctor                                                  functor(x_data, y_data);
	Eigen::NumericalDiff<GaussianFunctor>                            numDiff(functor);
	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<GaussianFunctor>> optimizer(numDiff);

	// Fit the function
	Eigen::LevenbergMarquardtSpace::Status status = optimizer.minimize(initial_params);

	// Output the fitted parameters
	std::cout << "Fitted parameters:" << std::endl;
	std::cout << "Status: " << status << '\n';
	std::cout << "Iterations: " << optimizer.iterations() << '\n';
	std::cout << "Function evaluations: " << optimizer.nfev() << '\n';
	std::cout << "ftol: " << optimizer.ftol() << '\n';
	std::cout << "xtol: " << optimizer.xtol() << '\n';
	std::cout << "gtol: " << optimizer.gtol() << '\n';
	std::cout << "amplitude_est: " << initial_params(0) << std::endl;
	std::cout << "mean_est: " << initial_params(1) << std::endl;
	std::cout << "std_est: " << initial_params(2) << std::endl;

	// 2d gaussian
	fmt::print("\ntrying 2d gaussian fitting\n");
	// Generate sample data
	constexpr int width  = 20;
	constexpr int height = 20;

	Eigen::Matrix<double, Eigen::Dynamic, 3> data(width * height, 3);

	constexpr double A     = 10.0;
	constexpr double x0    = width / 2.5;
	constexpr double y0    = height / 2.0;
	constexpr double sigma = 5.0;

	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			int index      = (h * height) + w;
			data(index, 0) = h;
			data(index, 1) = w;
			data(index, 2) = gaussian2d(data(index, 0), data(index, 1), A, x0, y0, sigma) +
			                 0.05 * (static_cast<double>(rand()) / (double)RAND_MAX);
			// fmt::print("index: {}, x: {}, y:{}, value:{}\n", index, data(index, 0), data(index, 1), data(index, 2));
			;
		}
	}

	// std::cout << "data matrix is: " << data << '\n';

	// Initial guess for Gaussian parameters
	Eigen::VectorXd initial_params_2d(4);
	initial_params_2d << A + 5, x0 + 5, y0 - 4, sigma - 3;

	// Add the df() method to GaussianFunctor using NumericalDiff
	GaussianFunctor2d                                                  functor2d(data);
	Eigen::NumericalDiff<GaussianFunctor2d>                            numDiff2d(functor2d);
	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<GaussianFunctor2d>> optimizer2d(numDiff2d);

	// Fit the function
	Eigen::LevenbergMarquardtSpace::Status status2d = optimizer2d.minimize(initial_params_2d);

	// Output the fitted parameters
	std::cout << "Fitted parameters:" << std::endl;
	std::cout << "Status: " << status2d << '\n';
	std::cout << "Iterations: " << optimizer2d.iterations() << '\n';
	std::cout << "Function evaluations: " << optimizer2d.nfev() << '\n';
	std::cout << "ftol: " << optimizer2d.ftol() << '\n';
	std::cout << "xtol: " << optimizer2d.xtol() << '\n';
	std::cout << "gtol: " << optimizer2d.gtol() << '\n';
	std::cout << "amplitude_est: " << initial_params_2d(0) << std::endl;
	std::cout << "mean_est_x: " << initial_params_2d(1) << std::endl;
	std::cout << "mean_est_y: " << initial_params_2d(2) << std::endl;
	std::cout << "std_est: " << initial_params_2d(3) << std::endl;

	return 0;
}
