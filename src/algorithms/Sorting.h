#ifndef SORTING_H
#define SORTING_H

#include "../models/Post.h"

// Search results are limited to 500 pointers, matching the feed capacity.
// These functions rearrange only pointers; PostLinkedList still owns the posts.
namespace PostSorting {

const int MAX_SEARCH_RESULTS = 500;

inline int calculateEngagementScore(const Post* post) {
    if (post == NULL) {
        return 0;
    }

    return (3 * post->likeCount) + (5 * post->commentCount);
}

inline void merge(const Post* results[], int left, int middle, int right) {
    const Post* temporary[MAX_SEARCH_RESULTS];
    int leftIndex = left;
    int rightIndex = middle + 1;
    int temporaryIndex = 0;

    while (leftIndex <= middle && rightIndex <= right) {
        // Higher post IDs are newer. Keeping the left item first on an equal
        // ID also preserves Merge Sort's stable merge behavior.
        if (results[leftIndex]->postID >= results[rightIndex]->postID) {
            temporary[temporaryIndex] = results[leftIndex];
            leftIndex++;
        } else {
            temporary[temporaryIndex] = results[rightIndex];
            rightIndex++;
        }
        temporaryIndex++;
    }

    while (leftIndex <= middle) {
        temporary[temporaryIndex] = results[leftIndex];
        leftIndex++;
        temporaryIndex++;
    }

    while (rightIndex <= right) {
        temporary[temporaryIndex] = results[rightIndex];
        rightIndex++;
        temporaryIndex++;
    }

    for (int index = 0; index < temporaryIndex; index++) {
        results[left + index] = temporary[index];
    }
}

inline void mergeSortByNewest(const Post* results[], int left, int right) {
    if (left >= right) {
        return;
    }

    int middle = left + (right - left) / 2;
    mergeSortByNewest(results, left, middle);
    mergeSortByNewest(results, middle + 1, right);
    merge(results, left, middle, right);
}

// Merge Sort: O(n log n) time and O(n) extra space for a result array.
inline void mergeSortByNewest(const Post* results[], int resultCount) {
    if (resultCount > 1) {
        mergeSortByNewest(results, 0, resultCount - 1);
    }
}

inline bool hasHigherEngagement(const Post* first, const Post* second) {
    int firstScore = calculateEngagementScore(first);
    int secondScore = calculateEngagementScore(second);

    if (firstScore != secondScore) {
        return firstScore > secondScore;
    }

    // A newer post wins an equal-engagement tie.
    return first->postID > second->postID;
}

inline int partitionByEngagement(const Post* results[], int low, int high) {
    const Post* pivot = results[high];
    int smallerIndex = low - 1;

    for (int index = low; index < high; index++) {
        if (hasHigherEngagement(results[index], pivot)) {
            smallerIndex++;
            const Post* temporary = results[smallerIndex];
            results[smallerIndex] = results[index];
            results[index] = temporary;
        }
    }

    const Post* temporary = results[smallerIndex + 1];
    results[smallerIndex + 1] = results[high];
    results[high] = temporary;
    return smallerIndex + 1;
}

inline void quickSortByEngagement(const Post* results[], int low, int high) {
    if (low >= high) {
        return;
    }

    int pivotIndex = partitionByEngagement(results, low, high);
    quickSortByEngagement(results, low, pivotIndex - 1);
    quickSortByEngagement(results, pivotIndex + 1, high);
}

// Quick Sort: O(n log n) average time, O(n^2) worst-case time, and an
// average O(log n) recursive call stack. The final item is used as the pivot.
inline void quickSortByEngagement(const Post* results[], int resultCount) {
    if (resultCount > 1) {
        quickSortByEngagement(results, 0, resultCount - 1);
    }
}

}  // namespace PostSorting

#endif
