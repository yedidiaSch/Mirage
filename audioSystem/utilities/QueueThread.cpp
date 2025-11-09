#include "QueueThread.h"
#include <iostream>



QueueThread::QueueThread() 
{
    start();
}

QueueThread::~QueueThread() 
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        m_running = false;
    }
    queueCondition.notify_all();  // Wake up the waiting thread
    
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void QueueThread::put(std::function<void()> task) 
{
    if (!task) {
        return; // Don't add null/empty tasks
    }
    
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        taskQueue.push(task);
    }

    queueCondition.notify_one();
}

void QueueThread::thread() 
{
    while (m_running) 
    {
        std::function<void()> task;
        {

            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this] { return !m_running || !taskQueue.empty(); });
            if (!m_running && taskQueue.empty()) 
            {
                return;
            }

            task = std::move(taskQueue.front());
            taskQueue.pop();

        }

        // Execute task with exception handling
        if (task) {
            try {
                task();
            } catch (const std::exception& e) {
                std::cerr << "Exception in QueueThread task: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Unknown exception in QueueThread task" << std::endl;
            }
        }
    }
}