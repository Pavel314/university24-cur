#include <stdio.h>
#include <assert.h>
#include <stddef.h>

typedef struct {
	double alpha, x, y;
	double t_start, t_end;
	int steps;
} hopf_config;

static const hopf_config HOPF_CONFIGS[] = {
	{ -0.1, 0.1, 0.0, 0.0, 20.0, 100000},
	{ 0.25, 0.4, 0.0, 0.0, 20.0, 100000},
	{ 0.25, 0.6, 0.0, 0.0, 20.0, 100000},
};
static const int RUN_CONFIGS_COUNT = (int)(sizeof(HOPF_CONFIGS) / sizeof(HOPF_CONFIGS[0]));
#define OUT_LINES 1000
#define NUM_FMT "%.16lf"

#define RK4_BUF_SIZE(size) ((size) * 5)
static void rk4_differ(double buf[], double t_start, double t_end, size_t steps, void (*f)(double, double*, double*, size_t, void* user), double args[], const size_t args_count, void* user) {
	assert(buf && args && f && steps > 0 && args_count > 0);
	const double step = (t_end - t_start) / (double)steps;
	const double step05 = step * 0.5;
	double* k1 = buf;
	double* k2 = k1 + args_count;
	double* k3 = k2 + args_count;
	double* k4 = k3 + args_count;
	double* tmp = k4 + args_count;

	for (size_t s = 0; s < steps; ++s) {
		const double t = t_start + step * (double)(s);
		f(t, args, k1, args_count, user);
		for (size_t i = 0; i < args_count; ++i) tmp[i] = args[i] + step05 * k1[i];
		f(t + step05, tmp, k2, args_count, user);
		for (size_t i = 0; i < args_count; ++i) tmp[i] = args[i] + step05 * k2[i];
		f(t + step05, tmp, k3, args_count, user);
		for (size_t i = 0; i < args_count; ++i) tmp[i] = args[i] + step * k3[i];
		f(t + step, tmp, k4, args_count, user);
		for (size_t i = 0; i < args_count; ++i) args[i] += (step / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
	}
}

static void hopf_system(double t, double* args, double* dy, size_t args_count, void* user) {
	assert(args_count == 2);
	const double alpha = *(const double*)user;
	const double x = args[0], y = args[1];
	const double dist = x * x + y * y;
	dy[0] = alpha * x - y - x * dist;
	dy[1] = x + alpha * y - y * dist;
}

int main(void)
{
	printf(
		"Welcome to Laboratory work 2, (2026.10.31) by Pavel314\n"
		"Goal: for given presets numerically solve 2D Hopf bifurcation ODE equations,\n"
		"      save results to CSV files, and visualize trajectories by external tool\n"
		"Parameters: output lines = %d\n"
		"================================================================\n",
		OUT_LINES
	);

	double rk4_buf[RK4_BUF_SIZE(2)];
	char filename[256];

	for (int i = 0; i < RUN_CONFIGS_COUNT; ++i) {

		const hopf_config* conf = &HOPF_CONFIGS[i];
		snprintf(filename, sizeof(filename), "hopf_solved%d[alpha=%g, x=%g, y=%g, t0=%g, t1=%g, steps=%d].csv", i + 1, conf->alpha, conf->x, conf->y, conf->t_start, conf->t_end, conf->steps);

		printf("Preset %d/%d:\nSaving to %s\n\n", i + 1, RUN_CONFIGS_COUNT, filename);
		
		FILE* file = fopen(filename, "w");
		assert(file);
		if (!file) return 1;

		fprintf(file, "X,Y\n");
		fprintf(file, NUM_FMT "," NUM_FMT "\n", conf->x, conf->y);

		double args[] = { conf->x, conf->y };
		const double dt = (conf->t_end - conf->t_start) / (double)OUT_LINES;
		size_t steps = conf->steps / OUT_LINES;
		assert(conf->steps % OUT_LINES == 0);
		for (size_t line = 0; line < OUT_LINES; ++line) {
			const double t_cur = conf->t_start + dt * (double)line;
			rk4_differ(rk4_buf, t_cur, t_cur + dt, steps, hopf_system, args, 2, (void*)&conf->alpha);
			fprintf(file, NUM_FMT "," NUM_FMT "\n", args[0], args[1]);
		}
		fclose(file);
	}
	return 0;
}