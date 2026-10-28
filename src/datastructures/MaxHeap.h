#ifndef MAX_HEAP_H
#define MAX_HEAP_H

#include "../models/Post.h"

// A FeedItem temporarily connects an existing post to its calculated feed score.
// The post pointer is non-owning; PostLinkedList remains responsible for the Post.
struct FeedItem {
    const Post* post;
    int score;

    FeedItem() : post(NULL), score(0) {
    }

    FeedItem(const Post* feedPost, int feedScore)
        : post(feedPost), score(feedScore) {
    }
};

// A fixed-array max heap for ranking personalized-feed candidates.
class MaxHeap {
public:
    static const int MAX_FEED_POSTS = 500;

private:
    FeedItem heap[MAX_FEED_POSTS];
    int heapSize;

    // Higher score wins. Equal scores use the newer (higher-ID) post first.
    bool hasHigherPriority(const FeedItem& first, const FeedItem& second) const {
        if (first.score != second.score) {
            return first.score > second.score;
        }

        if (first.post == NULL) {
            return false;
        }

        if (second.post == NULL) {
            return true;
        }

        return first.post->postID > second.post->postID;
    }

    void swapItems(int firstIndex, int secondIndex) {
        FeedItem temporary = heap[firstIndex];
        heap[firstIndex] = heap[secondIndex];
        heap[secondIndex] = temporary;
    }

    void heapifyUp(int index) {
        while (index > 0) {
            int parentIndex = (index - 1) / 2;

            if (!hasHigherPriority(heap[index], heap[parentIndex])) {
                break;
            }

            swapItems(index, parentIndex);
            index = parentIndex;
        }
    }

    void heapifyDown(int index) {
        while (true) {
            int leftChild = 2 * index + 1;
            int rightChild = 2 * index + 2;
            int highestPriority = index;

            if (leftChild < heapSize &&
                hasHigherPriority(heap[leftChild], heap[highestPriority])) {
                highestPriority = leftChild;
            }

            if (rightChild < heapSize &&
                hasHigherPriority(heap[rightChild], heap[highestPriority])) {
                highestPriority = rightChild;
            }

            if (highestPriority == index) {
                break;
            }

            swapItems(index, highestPriority);
            index = highestPriority;
        }
    }

public:
    MaxHeap() : heapSize(0) {
    }

    bool insert(const FeedItem& item) {
        if (heapSize == MAX_FEED_POSTS || item.post == NULL) {
            return false;
        }

        heap[heapSize] = item;
        heapifyUp(heapSize);
        heapSize++;
        return true;
    }

    bool extractMax(FeedItem& removedItem) {
        if (isEmpty()) {
            return false;
        }

        removedItem = heap[0];
        heapSize--;

        if (heapSize > 0) {
            heap[0] = heap[heapSize];
            heapifyDown(0);
        }

        return true;
    }

    const FeedItem* peek() const {
        if (isEmpty()) {
            return NULL;
        }

        return &heap[0];
    }

    bool isEmpty() const {
        return heapSize == 0;
    }

    int size() const {
        return heapSize;
    }

    void clear() {
        heapSize = 0;
    }
};

#endif
