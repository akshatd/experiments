#include <cmath>
#include <iostream>
#include <fmt/format.h>
#include <Eigen/Dense>
#include <unsupported/Eigen/NumericalDiff>
#include <unsupported/Eigen/LevenbergMarquardt>

// both of these are from https://en.wikipedia.org/wiki/Gaussian_function
// repeating pow instead of repeating divde cos div is more expensive
double Gaussian1d(double x, double amplitude, double mean, double std) {
	return amplitude * std::exp(-std::pow(x - mean, 2) / (2 * std::pow(std, 2)));
}

// taking the 2 out to avoid an extra multiply in the denominator
double Gaussian2d(double x, double y, double amplitude, double mean_x, double mean_y, double sigma) {
	double arg = pow((x - mean_x) / sigma, 2) + pow((y - mean_y) / sigma, 2);
	return amplitude * exp(-arg / 2.0);
}

// Functors for Gaussian fitting
struct GaussianFunctor1d : Eigen::DenseFunctor<double> {
		const Eigen::Matrix<double, Eigen::Dynamic, 2> &data;

		GaussianFunctor1d(const Eigen::Matrix<double, Eigen::Dynamic, 2> &data_in)
				: Eigen::DenseFunctor<double>(3, static_cast<int>(data_in.rows())), data(data_in) {}

		int operator()(const Eigen::VectorXd &params, Eigen::VectorXd &fvec) const {
			double amplitude_est = params(0);
			double mean_est      = params(1);
			double std_est       = params(2);
			// fmt::print("Estimates: amp={:.3f}, mean={:.3f}, std={:.3f}\n", amplitude_est, mean_est, std_est);
			for (int i = 0; i < values(); ++i) {
				double out_fit = Gaussian1d(data(i, 0), amplitude_est, mean_est, std_est);
				fvec(i)        = out_fit - data(i, 1);
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
			// fmt::print(
			// "Estimates: amp={:.3f}, mean={:.3f},{:.3f}, std={:.3f}\n", amplitude_est, mean_est_x, mean_est_y, std_est);
			for (int i = 0; i < values(); ++i) {
				double out_fit = Gaussian2d(data(i, 0), data(i, 1), amplitude_est, mean_est_x, mean_est_y, std_est);
				fvec(i)        = out_fit - data(i, 2);
			}
			return 0;
		}
		// df() that implements the jacobian matrix will be added by NumericalDiff
};

struct FittingResult {
		Eigen::LevenbergMarquardtSpace::Status status;
		double                                 amplitude_fit;
		double                                 mean_x_fit;
		double                                 mean_y_fit;
		double                                 std_fit;
		int                                    k_iters;
		int                                    k_f_evals;
		int                                    k_j_evals;
		double                                 norm_vector;
		double                                 norm_grad_error;
};

FittingResult FitGaussian1d(Eigen::Matrix<double, Eigen::Dynamic, 2> &data) {
	// Initial guess for Gaussian parameters
	Eigen::VectorXd guess(3);
	int             row_max;
	guess(0) = data.col(1).maxCoeff(&row_max); // amplitude_guess
	guess(1) = data(row_max, 0);               // mean_guess
	guess(2) = 1;                              // std_guess
	fmt::print("Guess: {}, {}, {}\n", guess(0), guess(1), guess(2));

	// Add the df() method using NumericalDiff
	GaussianFunctor1d                                                  functor(data);
	Eigen::NumericalDiff<GaussianFunctor1d>                            numDiff(functor);
	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<GaussianFunctor1d>> optimizer(numDiff);

	// Fit the function
	Eigen::LevenbergMarquardtSpace::Status status = optimizer.minimize(guess);
	return {status,           guess(0),         guess(1),          guess(1),         guess(2), optimizer.iterations(),
	        optimizer.nfev(), optimizer.njev(), optimizer.fnorm(), optimizer.gnorm()};
}

FittingResult FitGaussian2d(Eigen::Matrix<double, Eigen::Dynamic, 3> &data) {
	// Initial guess for Gaussian parameters
	Eigen::VectorXd guess(4);
	int             row_max;
	guess(0) = data.col(2).maxCoeff(&row_max); // amplitude_guess
	guess(1) = data(row_max, 0);               // mean_x_guess
	guess(2) = data(row_max, 1);               // mean_y_guess
	guess(3) = 1;                              // std_guess
	fmt::print("Guess: {}, {}, {}, {}\n", guess(0), guess(1), guess(2), guess(3));

	// Add the df() method using NumericalDiff
	GaussianFunctor2d                                                  functor(data);
	Eigen::NumericalDiff<GaussianFunctor2d>                            numDiff(functor);
	Eigen::LevenbergMarquardt<Eigen::NumericalDiff<GaussianFunctor2d>> optimizer(numDiff);

	// Fit the function
	Eigen::LevenbergMarquardtSpace::Status status = optimizer.minimize(guess);
	return {status,           guess(0),         guess(1),          guess(2),         guess(3), optimizer.iterations(),
	        optimizer.nfev(), optimizer.njev(), optimizer.fnorm(), optimizer.gnorm()};
}

int main() {
	fmt::print("1D gaussian fitting\n");
	// Generate sample data
	constexpr int                            data_size = 100;
	Eigen::Matrix<double, Eigen::Dynamic, 2> data_1d(data_size, 2);

	double amplitude_est = 1.0;
	double mean_est      = 50.0;
	double std_est       = 10.0;

	for (int i = 0; i < data_size; ++i) {
		data_1d(i, 0) = i;
		data_1d(i, 1) = Gaussian1d(data_1d(i, 0), amplitude_est, mean_est, std_est) +
		                0.05 * (static_cast<double>(rand()) / (double)RAND_MAX);
	}
	FittingResult result_1d = FitGaussian1d(data_1d);
	fmt::print(
		"Status: {}\namp: {}\nmean_x: {}\nmean_y: {}\nstd: {}\niters: {}\nf evals: {}\nj evals: {}\nnorm vector: {}\nnorm "
		"grad error: {}\n",
		result_1d.status, result_1d.amplitude_fit, result_1d.mean_x_fit, result_1d.mean_y_fit, result_1d.std_fit,
		result_1d.k_iters, result_1d.k_f_evals, result_1d.k_j_evals, result_1d.norm_vector, result_1d.norm_grad_error);

	fmt::print("\n2D gaussian fitting\n");
	// Generate sample data
	constexpr int width  = 20;
	constexpr int height = 20;

	Eigen::Matrix<double, Eigen::Dynamic, 3> data_2d(width * height, 3);

	constexpr double A     = 10.0;
	constexpr double x0    = width / 2.5;
	constexpr double y0    = height / 2.0;
	constexpr double sigma = 5.0;

	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			int index         = (h * height) + w;
			data_2d(index, 0) = h;
			data_2d(index, 1) = w;
			data_2d(index, 2) = Gaussian2d(data_2d(index, 0), data_2d(index, 1), A, x0, y0, sigma) +
			                    0.05 * (static_cast<double>(rand()) / (double)RAND_MAX);
		}
	}

	FittingResult result_2d = FitGaussian2d(data_2d);
	fmt::print(
		"Status: {}\namp: {}\nmean_x: {}\nmean_y: {}\nstd: {}\niters: {}\nf evals: {}\nj evals: {}\nnorm vector: {}\nnorm "
		"grad error: {}\n",
		result_2d.status, result_2d.amplitude_fit, result_2d.mean_x_fit, result_2d.mean_y_fit, result_2d.std_fit,
		result_2d.k_iters, result_2d.k_f_evals, result_2d.k_j_evals, result_2d.norm_vector, result_2d.norm_grad_error);

	return 0;
}
