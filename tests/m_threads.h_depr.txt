#pragma once
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <switch.h>
#include <stdio.h>

typedef struct {
    pthread_t *threads;
    size_t size;
    size_t capacity;
} ThreadList;

void init_thread_list(ThreadList *list, size_t initial_capacity) {
    list->threads = (pthread_t *)malloc(initial_capacity * sizeof(pthread_t));
    if (list->threads == NULL) {
        perror("Failed to allocate memory for thread list");
        exit(EXIT_FAILURE);
    }
    list->size = 0;
    list->capacity = initial_capacity;
}

void add_thread(ThreadList *list, pthread_t thread) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->threads = (pthread_t *)realloc(list->threads, list->capacity * sizeof(pthread_t));
        if (list->threads == NULL) {
            perror("Failed to reallocate memory for thread list");
            exit(EXIT_FAILURE);
        }
    }
    list->threads[list->size++] = thread;
}

void join_and_cleanup_threads(ThreadList *list) {
    for (size_t i = 0; i < list->size; i++) {
        pthread_join(list->threads[i], NULL);
    }
    free(list->threads);
}

