/*
 * Copyright (c) 2024 Wojciech Graj
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 **/

#define _GNU_SOURCE

#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct SortCtx {
	int data;
	pthread_barrier_t *barrier;
	pthread_mutex_t *mutex;
	int *result;
	unsigned *idx;
};

void *sort_task(void *);

void sched_sort(int *data, unsigned count)
{
	const int priority_max = sched_get_priority_max(SCHED_FIFO);
	const int priority_min = sched_get_priority_min(SCHED_FIFO);

	pthread_barrier_t barrier;
	pthread_barrier_init(&barrier, NULL, count + 1);

	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);

	cpu_set_t cpu;
	CPU_ZERO(&cpu);
	CPU_SET(0, &cpu);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setaffinity_np(&attr, sizeof(cpu), &cpu);

	unsigned data_idx = 0;

	pthread_t *threads = malloc(sizeof(pthread_t) * count);
	struct SortCtx *ctxs = malloc(sizeof(struct SortCtx) * count);

	unsigned i;
	for (i = 0; i < count; i++) {
		ctxs[i] = (struct SortCtx) {
			.barrier = &barrier,
			.mutex = &mutex,
			.data = data[i],
			.result = data,
			.idx = &data_idx,
		};
		pthread_create(&threads[i], &attr, sort_task, &ctxs[i]);
		pthread_setschedparam(threads[i], SCHED_FIFO, &(struct sched_param) {
			.sched_priority = (data[i] < priority_max) ? ((data[i] > priority_min) ? data[i] : priority_min) : priority_max,
		});
	}

	pthread_barrier_wait(&barrier);

	for (i = 0; i < count; i++) {
		pthread_join(threads[i], NULL);
	}

	pthread_barrier_destroy(&barrier);
	pthread_mutex_destroy(&mutex);
	free(threads);
	free(ctxs);
}

void *sort_task(void *arg)
{
	struct SortCtx *ctx = (struct SortCtx*)arg;
	pthread_barrier_wait(ctx->barrier);

	pthread_mutex_lock(ctx->mutex);
	ctx->result[*ctx->idx] = ctx->data;
	(*ctx->idx)++;
	pthread_mutex_unlock(ctx->mutex);

	return NULL;
}

int main(int argc, char *argv[])
{
	int *data = malloc(sizeof(int) * (argc - 1));

	int i;
	for (i = 0; i < argc - 1; i++)
		data[i] = atoi(argv[i + 1]);

	sched_sort(data, argc - 1);

	for (i = 0; i < argc - 1; i++)
		printf("%d ", data[i]);
	putchar('\n');

	free(data);
	return 0;
}
