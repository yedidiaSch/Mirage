#include <sys/timerfd.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <cstring> // Include this header for strerror
#include <atomic> // Include for std::atomic
#include <poll.h> // For poll(), struct pollfd, and POLLIN

#include "TimerFd.h"

using namespace std::chrono;



TimerFd::TimerFd() : m_timer_fd(timerfd_create(CLOCK_REALTIME, 0))
{
    if(-1 == m_timer_fd)
    {
        throw std::runtime_error("timerfd_create error: " + std::string(strerror(errno)));
    }
}

TimerFd::~TimerFd() 
{
    Stop();
    close(m_timer_fd);
}

void TimerFd::SetTimer(std::chrono::milliseconds delay, std::chrono::milliseconds interval)
{
    itimerspec task_timer{};
    task_timer.it_interval.tv_sec =
        std::chrono::duration_cast<std::chrono::seconds>(interval).count();
    task_timer.it_interval.tv_nsec =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            interval % std::chrono::seconds(1)).count();
    task_timer.it_value.tv_sec =
        std::chrono::duration_cast<std::chrono::seconds>(delay).count();
    task_timer.it_value.tv_nsec =
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            delay % std::chrono::seconds(1)).count();

    if (timerfd_settime(m_timer_fd, 0, &task_timer, NULL) == -1)
    {
        throw std::runtime_error("timerfd_settime error: " + std::string(strerror(errno)));
    }
}

long TimerFd::GetTimer()
{
    itimerspec task_timer;

    if (timerfd_gettime(m_timer_fd, &task_timer) == -1)
    {
        throw std::runtime_error("timerfd_gettime error: " + std::string(strerror(errno)));
    }
    
    return task_timer.it_value.tv_sec * SEC_TO_MILI +
           task_timer.it_value.tv_nsec / MILI_TO_NANO;
}

void TimerFd::Start()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    start();
}

void TimerFd::Stop()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_running) {
            return; // Already stopped
        }
        m_running = false;
        
        // Disable the timer to prevent further expirations
        struct itimerspec disable_timer = {};
        if (timerfd_settime(m_timer_fd, 0, &disable_timer, NULL) == -1) {
            std::cerr << "Failed to disable timer: " << std::string(strerror(errno)) << std::endl;
        }
    }
    
    // Call the base class stop() to join the thread
    stop();
}

void TimerFd::thread()
{
    while (m_running)
    {
        uint64_t buffer;
        size_t return_size = sizeof(buffer);

        // Set up polling to check m_running periodically
        struct pollfd pfd;
        pfd.fd = m_timer_fd;
        pfd.events = POLLIN;
        pfd.revents = 0;
        
        // Poll with a short timeout to check m_running periodically
        int poll_result = poll(&pfd, 1, 50); // 50ms timeout for quicker response
        
        // Check if we should exit
        if (!m_running) break;
        
        if (poll_result < 0)
        {
            if (errno == EINTR) continue; // Interrupted, retry
            if (!m_running) break;
            std::cerr << "Poll error: " << std::string(strerror(errno)) << std::endl;
            break;
        }
        else if (poll_result == 0)
        {
            // Timeout, check m_running again
            continue;
        }
        
        // Check if there's data to read
        if (pfd.revents & POLLIN)
        {
            int result = read(m_timer_fd, &buffer, return_size);
            if (result < 0)
            {
                if (errno == EINTR) continue;
                if (!m_running) break;
                std::cerr << "Read error: " << std::string(strerror(errno)) << std::endl;
                break;
            }
            
            // Only call onTimeout if still running
            if (m_running)
            {
                onTimeout();
            }
        }
    }
}
