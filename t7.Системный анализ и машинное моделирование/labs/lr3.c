#define _USE_MATH_DEFINES

#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
	const double* poly;
	int count;
	int steps;
	double step;
} run_config;

static const run_config RUN_CONFIGS[] = {
	{(double[]) { 1.0, 4.0, 16.0, 25.0, 13.0, 9.0 }, 6, 10000, 0.1},
	{(double[]) { 1.0, 4.0, 9.0, 16.0, 19.0, 13.0 }, 6, 10000, 0.1},
};
static const int RUN_CONFIGS_COUNT = (int)(sizeof(RUN_CONFIGS) / sizeof(RUN_CONFIGS[0]));


typedef struct {double real, imag;} cpx;
static double cpx_abs(const cpx z) {return hypot(z.real, z.imag);}
static double cpx_arg(const cpx z) {return atan2(z.imag, z.real);}
static cpx cpx_mul(const cpx a, const cpx b) {return (cpx){ a.real * b.real - a.imag * b.imag, a.real * b.imag + b.real * a.imag };}
static cpx cpx_conj(const cpx z) {return (cpx) {z.real, -z.imag };}
static double cpx_phase_diff(const cpx z1, const cpx z2) {return cpx_arg(cpx_mul(z2, cpx_conj(z1)));}

static cpx cpx_calc_polyd(const cpx s, const double poly[], int count) {
	assert(poly && count > 0);
	cpx res = { poly[0], 0.0 };
	for (int i = 1; i < count; ++i) {
		res = cpx_mul(res, s);
		res.real += poly[i];
	}
	return res;
}

typedef struct {
	double total_angle;
	double required_angle;
	double passed_quadrants;
} michailov_info;

static michailov_info calc_michailov_stability(const double poly[], int count, int steps, double step) {
	assert(poly && count > 0 && steps > 0 && step > 0.0);
	double w = 0.0, ang = 0.0;
	cpx cur = cpx_calc_polyd((cpx){0,w}, poly, count);
	for (int s = 0; s < steps; ++s) {
		w += step;
		const cpx next = cpx_calc_polyd((cpx){0,w}, poly, count);
		ang += cpx_phase_diff(cur, next);
		cur = next;
	}
	return (michailov_info) {.total_angle = ang, .required_angle = (double)(count - 1) * M_PI * 0.5, .passed_quadrants = ang / (M_PI * 0.5) };
}

int main(void)
{
	printf(
		"Welcome to Laboratory work 3, (2026.11.01) by Pavel314\n"
		"Goal: check ODE stability via the Michailov criterion for a given characteristic polynomial.\n"
		"================================================================\n"
	);


	for (int i = 0; i < RUN_CONFIGS_COUNT; ++i) {
		const run_config* conf = &RUN_CONFIGS[i];
		const int cnt = conf->count;
		printf("Polynomial: ");
		for (int k = 0; k < cnt; ++k) printf(k == cnt-1?"%g":"%g*L^%d + ", conf->poly[k], cnt - k - 1);
		printf("\n");
		const michailov_info inf = calc_michailov_stability(conf->poly, conf->count, conf->steps, conf->step);
		const double delta = fabs(inf.total_angle - inf.required_angle);
		printf("Total angle:  %g\n", inf.total_angle);
		printf("Required angle: %g\n", inf.required_angle);
		printf("Passed quadrants: %g\n", inf.passed_quadrants);
		printf("Delta: %g\n", delta);
		printf("Is stable: %s\n", delta<0.01?"Yes":"No");
		printf("--------------------------------\n");
	}
	return 0;
}