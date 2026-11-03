#include <stdio.h>
#include <assert.h>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static void hopf_system(double t, double* args, size_t args_count, double* dy, void* user);

typedef struct { 
	void (*system)(double, double*, size_t, double*, void*);
	int args_count;
	double args_init[16];
	double params[16];
	double t_start, t_end;
	int steps;
} hopf_config;

static const hopf_config HOPF_CONFIGS[] = {
	{ hopf_system, 2, {0.1, 0.0}, {-0.1}, 0.0, 20.0, 100000},
	{ hopf_system, 2, {0.4, 0.0}, {0.25}, 0.0, 20.0, 100000},
	{ hopf_system, 2, {0.6, 0.0}, {0.25}, 0.0, 20.0, 100000}
};
static const int RUN_CONFIGS_COUNT = (int)(sizeof(HOPF_CONFIGS) / sizeof(HOPF_CONFIGS[0]));
#define OUT_LINES 1000
#define NUM_FMT "%.16lf"

#define RK4_BUF_SIZE(size) ((size) * 5)
static void rk4_differ(double buf[], double ts, double te, size_t steps, void (*f)(double, double*, size_t, double*, void* user), double args[], const size_t args_count, void* user) {
	assert(buf && args && f && steps > 0 && args_count > 0);
	const double step = (te - ts) / (double)steps;
	const double step05 = step * 0.5;
	double* k1 = buf;
	double* k2 = k1 + args_count;
	double* k3 = k2 + args_count;
	double* k4 = k3 + args_count;
	double* tmp = k4 + args_count;

	for (size_t s = 0; s < steps; ++s) {
		const double t = ts + step * (double)(s);
		f(t, args, args_count, k1, user);
		for (size_t i = 0; i < args_count; ++i) tmp[i] = args[i] + step05 * k1[i];
		f(t + step05, tmp, args_count,k2, user);
		for (size_t i = 0; i < args_count; ++i) tmp[i] = args[i] + step05 * k2[i];
		f(t + step05, tmp, args_count,k3, user);
		for (size_t i = 0; i < args_count; ++i) tmp[i] = args[i] + step * k3[i];
		f(t + step, tmp, args_count, k4, user);
		for (size_t i = 0; i < args_count; ++i) args[i] += (step / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
	}
}

static void hopf_system(double t, double* args, size_t args_count, double* dy, void* user) {
	assert(args_count == 2);
	const double* params = (const double*)user;
	const double alpha = params[0];
	const double x = args[0], y = args[1];
	const double dist = x * x + y * y;
	dy[0] = alpha * x - y - x * dist;
	dy[1] = x + alpha * y - y * dist;
}

int main(void)
{
	double rk4_buf[RK4_BUF_SIZE(64)];
	double args[16];
	char filename[64];

	for (int i = 0;i < RUN_CONFIGS_COUNT;++i) {
		const hopf_config* conf = &HOPF_CONFIGS[i];
		const int args_count = conf->args_count;

		memcpy(args, conf->args_init, args_count * sizeof(double));

		snprintf(filename, sizeof(filename), "diff_solve%d.csv", i + 1);
		FILE* file = fopen(filename, "w");
		assert(file);

		for (int k = 0; k < args_count; ++k) fprintf(file, (k==args_count-1)?"Y%d":"Y%d,", k);
		fprintf(file, "\n");

		for (int k = 0; k < args_count; ++k) fprintf(file, (k==args_count-1) ? NUM_FMT:NUM_FMT",", args[k]);
		fprintf(file, "\n");

		double cur_t = conf->t_start;
		const double t_end = conf->t_end;
		const double dt = (t_end - cur_t) / (double)OUT_LINES;
		const size_t steps_per_line = conf->steps / OUT_LINES;

		for (size_t line = 0; line < OUT_LINES; ++line) {
			rk4_differ(rk4_buf, cur_t, cur_t + dt, steps_per_line, conf->system, args, (size_t)args_count, (void*)conf->params);
			for (int k = 0; k < args_count; ++k) fprintf(file, (k==args_count-1)?NUM_FMT : NUM_FMT",", args[k]);
			fprintf(file, "\n");
			cur_t += dt;
		}
		fclose(file);

	}
	return 0;
}