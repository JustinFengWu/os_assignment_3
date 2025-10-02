/**
 * This file implements parallel mergesort.
 */

#include <stdio.h>
#include <string.h> /* for memcpy */
#include <stdlib.h> /* for malloc */
#include "mergesort.h"

/* this function will be called by mergesort() and also by parallel_mergesort(). */
void merge(int leftstart, int leftend, int rightstart, int rightend){
	int leftStart = leftstart; int leftStartB = leftStart;
	int rightStart = rightstart;

	// merge the two arrays by putting digits into B temporarily
	while (leftStart <= leftend && rightStart <= rightend) {
		if (A[leftStart] <= A[rightStart]) {
			B[leftStartB] = A[leftStart];
			leftStart++;
			leftStartB++;
		} else if(A[leftStart] > A[rightStart]) {
			B[leftStartB] = A[rightStart];
			leftStartB++;
			rightStart++;
		}
	}

	// order of these two cleanups dont matter
	while (leftStart <= leftend) {
		B[leftStartB] = A[leftStart];
			leftStart++;
			leftStartB++;
	}
	while (rightStart <= rightend) {
		B[leftStartB] = A[rightStart];
		leftStartB++;
		rightStart++;
	}

	// Copy back into A
	for (int i = leftstart; i <= rightend; i++) {
		A[i] = B[i];
	}

	return;
}

/* this function will be called by parallel_mergesort() as its base case. */
void my_mergesort(int left, int right){
	if (left == right) {
		return;
	}

	int mid = (left + right) / 2;
	my_mergesort(left, mid);
	my_mergesort(mid + 1, right);
	merge(left, mid, mid + 1, right);
}

/* this function will be called by the testing program. */
void * parallel_mergesort(void *arg){
	struct argument *args = arg;
    int left = args->left;
    int right = args->right;
    int level = args->level;
	int mid = (left + right) / 2;

	// reaching base case
    if (left == right) {
        free(args);
        return;
    }

    if (level < cutoff) {
        pthread_t threadId1, threadId2;
        struct argument *leftArgs = buildArgs(left, mid, level + 1);
        struct argument *rightArgs = buildArgs(mid + 1, right, level + 1);

        pthread_create(&threadId1, NULL, parallel_mergesort, leftArgs);
        pthread_create(&threadId2, NULL, parallel_mergesort, rightArgs);

        pthread_join(threadId1, NULL);
        pthread_join(threadId2, NULL);
    } else {
        // once cutoff has been reached use recursive version
        my_mergesort(left, mid);
        my_mergesort(mid + 1, right);
    }
    merge(left, mid, mid + 1, right);

    free(args);
    return;
}

/* we build the argument for the parallel_mergesort function. */
struct argument * buildArgs(int left, int right, int level){
	struct argument * arg = malloc(sizeof(struct argument));
	arg->left = left;
	arg->right = right;
	arg->level = level;	
	return arg;
}

