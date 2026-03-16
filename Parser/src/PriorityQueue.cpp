#include "PriorityQueue.h"

PriorityQueue& PriorityQueue::getInstance() {
    static PriorityQueue instance;
    return instance;
}

void PriorityQueue::push(std::unique_ptr<TransactionMessage> packet) {
    std::lock_guard<std::mutex> lock(mtx);
        pq.push(std::move(packet));
    // cv.notify_one();  // Notify waiting threads
}

std::unique_ptr<TransactionMessage> PriorityQueue::pop() {
    std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]() { return !pq.empty(); });

        auto packet = std::move(const_cast<std::unique_ptr<TransactionMessage>&>(pq.top()));
        pq.pop();
        return packet;;
}

bool PriorityQueue::empty() {
    std::lock_guard<std::mutex> lock(mtx);
    return pq.empty();
}

size_t PriorityQueue::size() {  // Returns the size of the priority queue
    std::lock_guard<std::mutex> lock(mtx);
    return pq.size();
}
