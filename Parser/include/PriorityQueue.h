#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include "TransactionMessage.h"
#include "Request.h"
#include "Response.h"

class PriorityQueue {
private:
    struct Compare {
        bool operator()(const std::unique_ptr<TransactionMessage>& a,
                        const std::unique_ptr<TransactionMessage>& b) const {
            return a->parsedData.seqNo > b->parsedData.seqNo;
        }
    };

    std::priority_queue<
        std::unique_ptr<TransactionMessage>,
        std::vector<std::unique_ptr<TransactionMessage>>,
        Compare> pq;

    std::mutex mtx;
    std::condition_variable cv;

    PriorityQueue() {}

public:
    PriorityQueue(const PriorityQueue&) = delete;
    PriorityQueue& operator=(const PriorityQueue&) = delete;

    static PriorityQueue& getInstance();

    void push(std::unique_ptr<TransactionMessage> packet);
    std::unique_ptr<TransactionMessage> pop();
    bool empty();
    size_t size();
};
