#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <assert.h>

#define SIGMA  2.0
#define MEAN   7.0
#define N      300
#define NSIGMA 10.0
#define LOW_BORDER (MEAN - NSIGMA * SIGMA)
#define UP_BORDER  (MEAN + NSIGMA * SIGMA)

static double normal(double x, double mean, double sigma) {
	const double s1 = -((x - mean) * (x - mean)) / (2.0 * sigma * sigma);
	const double s2 = sigma * sqrt(2.0 * M_PI);
	return exp(s1) / s2;
}

static double integ(double a, double b, int n, double (*f)(double)) {
    static const double weights[] = {
		16067.0 / 299376.0, 16067.0 / 149688.0, 26575.0 / 74844.0, -16175.0 / 99792.0, 5675.0 / 6237.0, -4825.0 / 5544.0, 17807.0 / 12474.0
    };
	assert(n > 0 && (n % 10 == 0));
	const double h = (b - a) / (double)n;
	double sum = 0.0;
	for (int i = 0; i <= n; i++) {
        int ind = 0;
		if (i > 0 && i < n)
		{
			ind = i % 10;
			if (ind > 5) ind = 10 - ind;
			ind += 1;
		}
		const double x_i = a + (double)i * h;
		sum += f(x_i) * weights[ind];
	}
	return 5.0 * h * sum;
}

static double wrap_normal(double x) {return normal(x, MEAN, SIGMA);}
static double wrap_mean(double x) {return x * wrap_normal(x);}
static double wrap_var(double x) {return (x - MEAN) * (x - MEAN) * wrap_normal(x);}

static const char* wrap_check_integ(double (*f)(double), double exp) {
	const double y = integ(LOW_BORDER, UP_BORDER, N, f);
	const double delta = y - exp;
	static char buf[64];
	snprintf(buf, sizeof(buf), (fabs(delta) < 1e-12)? "OK [delta=%g]":"ERR [delta=%g]" , delta);
	return buf;
}

int main(void)
{
	printf(
		"Welcome to Laboratory work 1, (2026.10.28) by Pavel314\n"
		"Goal: verify several invariants of the normal distribution density\n"
		"Parameters: mean = %g, sigma = %g, a = %g, b = %g\n"
		"================================================================\n",
		MEAN, SIGMA, LOW_BORDER, UP_BORDER
	);

	printf("Let's verify the these invariants:\n");
	printf("1. Integral[a, b] of rho(x) dx = 1.0... %s\n",  wrap_check_integ(wrap_normal, 1.0));
	printf("2. Integral[a, b] of x * rho(x) dx = %g... %s\n", MEAN, wrap_check_integ(wrap_mean, MEAN));
	printf("3. Integral[a, b] of (x - mean)^2 * rho(x) dx = %g... %s\n", SIGMA * SIGMA, wrap_check_integ(wrap_var, SIGMA * SIGMA));
	return 0;
}