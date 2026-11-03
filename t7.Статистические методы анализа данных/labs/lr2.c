#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#define MEAN_X   3.0
#define SIGMA_X  2.0

#define MEAN_Y   5.0
#define SIGMA_Y  1.0

#define N_X      200
#define N_Y      200

#define NSIGMA_X 10.0
#define LOW_BORDER_X (MEAN_X - NSIGMA_X * SIGMA_X)
#define UP_BORDER_X  (MEAN_X + NSIGMA_X * SIGMA_X)
#define NSIGMA_Y 10.0
#define LOW_BORDER_Y (MEAN_Y - NSIGMA_Y * SIGMA_Y)
#define UP_BORDER_Y  (MEAN_Y + NSIGMA_Y * SIGMA_Y)

static double normal(double x, double mean, double sigma) {
	const double s1 = -((x - mean) * (x - mean)) / (2.0 * sigma * sigma);
	const double s2 = sigma * sqrt(2.0 * M_PI);
	return exp(s1) / s2;
}

static double normal_2d(double x, double y, double mean_x, double sigma_x, double mean_y, double sigma_y) {
	return normal(x, mean_x, sigma_x) * normal(y, mean_y, sigma_y);
}

static double integ_get_weight(int i, int n) {
    static const double weights[] = {
        16067.0 / 299376.0, 16067.0 / 149688.0, 26575.0 / 74844.0, -16175.0 / 99792.0, 5675.0 / 6237.0, -4825.0 / 5544.0, 17807.0 / 12474.0
    };
    int ind = 0;
    if (i > 0 && i < n) {
        ind = i % 10;
        if (ind > 5) ind = 10 - ind;
        ind += 1;
    }
    return weights[ind];
}

static double integ_2d(double ax, double bx, int nx, double ay, double by, int ny, double (*f)(double, double)) {
    assert(nx > 0 && (nx % 10 == 0));
    assert(ny > 0 && (ny % 10 == 0));
    const double hx = (bx - ax) / (double)nx;
    const double hy = (by - ay) / (double)ny;
    double sum = 0.0;
    for (int i = 0; i <= nx; i++)
        for (int j = 0; j <= ny; j++)
            sum += f(ax + (double)i * hx, ay + (double)j * hy) * integ_get_weight(i, nx) * integ_get_weight(j, ny);
    return 25.0 * hx * hy * sum;
}

static double wrap_normal_2d(double x, double y) {return normal_2d(x, y, MEAN_X, SIGMA_X, MEAN_Y, SIGMA_Y);}
static double wrap_mean_2d(double x, double y) {return x * y * wrap_normal_2d(x, y);}
static double wrap_var_2d(double x, double y) {return ((x - MEAN_X) * (x - MEAN_X)) * ((y - MEAN_Y) * (y - MEAN_Y)) * wrap_normal_2d(x, y);}

static const double wrap_delta_integ_2d(double (*f)(double, double), double ans) {
	return integ_2d(LOW_BORDER_X, UP_BORDER_X, N_X, LOW_BORDER_Y, UP_BORDER_Y, N_Y, f) - ans;
}

int main(void)
{
	printf(
		"Welcome to Laboratory work 2, (2026.10.28) by Pavel314\n"
		"Goal: verify several properties of the 2D normal distribution\n"
		"Parameters: mean_x = %g, sigma_x = %g, a_x = %g, b_x = %g\n"
		"            mean_y = %g, sigma_y = %g, a_y = %g, b_y = %g\n"
		"================================================================\n",
		MEAN_X, SIGMA_X, LOW_BORDER_X, UP_BORDER_X,
		MEAN_Y, SIGMA_Y, LOW_BORDER_Y, UP_BORDER_Y
	);
	const double normal_ans = 1.0;
	const double mean_ans = MEAN_X * MEAN_Y;
	const double var_ans = SIGMA_X * SIGMA_X * SIGMA_Y * SIGMA_Y;

	printf("1. Integral[a_x,b_x] Integral[a_y,b_y] of rho(x,y) dydx = %g\n--->eps=%g\n", normal_ans, wrap_delta_integ_2d(wrap_normal_2d, normal_ans));
	printf("2. Integral[a_x,b_x] Integral[a_y,b_y] of x*y*rho(x,y) dydx = %g\n--->eps=%g\n", mean_ans, wrap_delta_integ_2d(wrap_mean_2d, mean_ans));
	printf("3. Integral[a_x,b_x] Integral[a_y,b_y] of (x-mean_x)^2 * (y-mean_y)^2 * rho(x,y) dydx = %g\n--->eps=%g\n", var_ans, wrap_delta_integ_2d(wrap_var_2d, var_ans));
	return 0;
}