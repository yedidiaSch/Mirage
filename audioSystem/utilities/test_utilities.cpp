#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <sstream>
#include <iomanip>

// Include all utility headers
#include "IObserver.h"
#include "subject.h"
#include "threadBase.h"
#include "QueueThread.h"
#include "TimerFd.h"

// ANSI color codes for beautiful output
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
}

// Test result tracking
struct TestResult {
    std::string testName;
    bool passed;
    std::string details;
    std::chrono::milliseconds duration;
};

class TestFramework {
private:
    std::vector<TestResult> results;
    int totalTests = 0;
    int passedTests = 0;

public:
    void runTest(const std::string& testName, std::function<bool()> testFunc) {
        std::cout << Colors::BLUE << "┌─ Running: " << Colors::BOLD << testName << Colors::RESET << std::endl;
        
        auto start = std::chrono::high_resolution_clock::now();
        bool result = false;
        std::string details = "";
        
        try {
            result = testFunc();
        } catch (const std::exception& e) {
            details = std::string("Exception: ") + e.what();
        } catch (...) {
            details = "Unknown exception occurred";
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        totalTests++;
        if (result) {
            passedTests++;
            std::cout << Colors::GREEN << "└─ ✓ PASSED" << Colors::DIM << " (" << duration.count() << "ms)" << Colors::RESET << std::endl;
        } else {
            std::cout << Colors::RED << "└─ ✗ FAILED" << Colors::DIM << " (" << duration.count() << "ms)";
            if (!details.empty()) {
                std::cout << " - " << details;
            }
            std::cout << Colors::RESET << std::endl;
        }
        std::cout << std::endl;
        
        results.push_back({testName, result, details, duration});
    }
    
    void printSummary() {
        std::cout << Colors::BOLD << Colors::CYAN << "═══════════════════════════════════════════════════════════════" << Colors::RESET << std::endl;
        std::cout << Colors::BOLD << Colors::WHITE << "                        TEST SUMMARY                           " << Colors::RESET << std::endl;
        std::cout << Colors::BOLD << Colors::CYAN << "═══════════════════════════════════════════════════════════════" << Colors::RESET << std::endl;
        
        double successRate = totalTests > 0 ? (double)passedTests / totalTests * 100.0 : 0.0;
        
        std::cout << Colors::WHITE << "Total Tests: " << Colors::BOLD << totalTests << Colors::RESET << std::endl;
        std::cout << Colors::GREEN << "Passed:      " << Colors::BOLD << passedTests << Colors::RESET << std::endl;
        std::cout << Colors::RED << "Failed:      " << Colors::BOLD << (totalTests - passedTests) << Colors::RESET << std::endl;
        std::cout << Colors::YELLOW << "Success Rate:" << Colors::BOLD << std::fixed << std::setprecision(1) << successRate << "%" << Colors::RESET << std::endl;
        
        std::cout << std::endl;
        
        // Show failed tests if any
        bool hasFailures = false;
        for (const auto& result : results) {
            if (!result.passed) {
                if (!hasFailures) {
                    std::cout << Colors::RED << Colors::BOLD << "Failed Tests:" << Colors::RESET << std::endl;
                    hasFailures = true;
                }
                std::cout << Colors::RED << "  ✗ " << result.testName;
                if (!result.details.empty()) {
                    std::cout << " - " << result.details;
                }
                std::cout << Colors::RESET << std::endl;
            }
        }
        
        if (hasFailures) {
            std::cout << std::endl;
        }
        
        std::cout << Colors::BOLD << Colors::CYAN << "═══════════════════════════════════════════════════════════════" << Colors::RESET << std::endl;
        
        if (passedTests == totalTests) {
            std::cout << Colors::GREEN << Colors::BOLD << "🎉 ALL TESTS PASSED! 🎉" << Colors::RESET << std::endl;
        } else {
            std::cout << Colors::YELLOW << Colors::BOLD << "⚠️  SOME TESTS FAILED ⚠️" << Colors::RESET << std::endl;
        }
        
        std::cout << Colors::BOLD << Colors::CYAN << "═══════════════════════════════════════════════════════════════" << Colors::RESET << std::endl;
    }
};

// Test Observer implementations
class TestObserver : public IObserver {
private:
    std::atomic<int> updateCount{0};
    void* lastParams = nullptr;

public:
    void update(void* params) override {
        updateCount++;
        lastParams = params;
    }
    
    int getUpdateCount() const { return updateCount; }
    void* getLastParams() const { return lastParams; }
    void reset() { updateCount = 0; lastParams = nullptr; }
};

// Test Thread implementations
class TestThread : public ThreadBase {
private:
    std::atomic<int> iterations{0};
    std::atomic<bool> shouldStop{false};

public:
    void thread() override {
        while (m_running && !shouldStop) {
            iterations++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void requestStop() { shouldStop = true; }
    int getIterations() const { return iterations; }
    void resetIterations() { iterations = 0; shouldStop = false; }
};

// Test Timer implementation
class TestTimer : public TimerFd {
private:
    std::atomic<int> timeoutCount{0};

public:
    void onTimeout() override {
        timeoutCount++;
    }
    
    int getTimeoutCount() const { return timeoutCount; }
    void resetCount() { timeoutCount = 0; }
};

// Test functions
bool testObserverPattern() {
    Subject subject;
    TestObserver observer1, observer2;
    
    // Test attach
    subject.attach(&observer1);
    subject.attach(&observer2);
    subject.attach(nullptr); // Should handle gracefully
    subject.attach(&observer1); // Should not duplicate
    
    // Test notify with params
    int testValue = 42;
    subject.notify(&testValue);
    
    if (observer1.getUpdateCount() != 1 || observer2.getUpdateCount() != 1) {
        return false;
    }
    
    if (observer1.getLastParams() != &testValue || observer2.getLastParams() != &testValue) {
        return false;
    }
    
    // Test notify with nullptr
    subject.notify(nullptr);
    
    if (observer1.getUpdateCount() != 2 || observer2.getUpdateCount() != 2) {
        return false;
    }
    
    if (observer1.getLastParams() != nullptr || observer2.getLastParams() != nullptr) {
        return false;
    }
    
    // Test detach
    subject.detach(&observer1);
    subject.detach(nullptr); // Should handle gracefully
    
    subject.notify(&testValue);
    
    // observer1 should not be updated, observer2 should
    if (observer1.getUpdateCount() != 2 || observer2.getUpdateCount() != 3) {
        return false;
    }
    
    return true;
}

bool testThreadBase() {
    TestThread testThread;
    
    // Test start
    testThread.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    if (testThread.getIterations() < 3) {
        return false;
    }
    
    // Test stop
    testThread.requestStop();
    testThread.stop();
    
    int finalIterations = testThread.getIterations();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    
    // Should not increase after stop
    if (testThread.getIterations() != finalIterations) {
        return false;
    }
    
    return true;
}

bool testQueueThread() {
    std::atomic<int> taskCounter{0};
    std::atomic<int> exceptionCounter{0};
    
    {
        QueueThread queueThread;
        
        // Add a few simple tasks
        queueThread.put([&taskCounter]() {
            taskCounter++;
        });
        
        queueThread.put([&taskCounter]() {
            taskCounter++;
        });
        
        // Add task that throws exception
        queueThread.put([&exceptionCounter]() {
            exceptionCounter++;
            throw std::runtime_error("Test exception");
        });
        
        // Add one more task to ensure thread continues after exception
        queueThread.put([&taskCounter]() {
            taskCounter++;
        });
        
        // Test null task (should be ignored)
        std::function<void()> nullTask;
        queueThread.put(nullTask);
        
        // Give tasks time to execute
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        // QueueThread will be destroyed here, calling stop() in destructor
    }
    
    // Verify results after QueueThread is destroyed
    if (taskCounter != 3) {
        return false;
    }
    
    if (exceptionCounter != 1) {
        return false;
    }
    
    return true;
}

bool testTimerFd() {
    {
        TestTimer timer;
        
        // Set one-shot timer (50ms delay, 0 interval)
        timer.SetTimer(std::chrono::milliseconds(50), std::chrono::milliseconds(0));
        timer.Start();
        
        // Should not timeout yet
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        if (timer.getTimeoutCount() != 0) {
            timer.Stop();
            return false;
        }
        
        // Should timeout now
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (timer.getTimeoutCount() != 1) {
            timer.Stop();
            return false;
        }
        
        // Should still be 1 (one-shot timer)
        std::this_thread::sleep_for(std::chrono::milliseconds(75));
        if (timer.getTimeoutCount() != 1) {
            timer.Stop();
            return false;
        }
        
        timer.Stop();
    }
    
    {
        TestTimer periodicTimer;
        periodicTimer.resetCount();
        
        // Test periodic timer (30ms delay, 30ms interval)
        periodicTimer.SetTimer(std::chrono::milliseconds(30), std::chrono::milliseconds(30));
        periodicTimer.Start();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        int timeouts = periodicTimer.getTimeoutCount();
        periodicTimer.Stop();
        
        // Should have 3-4 timeouts (initial + 2-3 intervals, allowing for timing variations)
        if (timeouts < 3 || timeouts > 5) {
            return false;
        }
    }
    
    return true;
}

bool testThreadSafety() {
    Subject subject;
    TestObserver observer;
    subject.attach(&observer);
    
    std::atomic<bool> running{true};
    std::atomic<int> notifyCount{0};
    
    // Multiple threads notifying simultaneously
    std::vector<std::thread> threads;
    for (int i = 0; i < 3; i++) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 10 && running; j++) {
                subject.notify(nullptr);
                notifyCount++;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    
    // Let them run briefly then stop
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    running = false;
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Observer update count should match notify count
    if (observer.getUpdateCount() != notifyCount) {
        return false;
    }
    
    // Should have at least some notifications
    if (notifyCount < 10) {
        return false;
    }
    
    return true;
}

void printHeader() {
    std::cout << Colors::BOLD << Colors::CYAN << std::endl;
    std::cout << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    UTILITIES TEST SUITE                      ║" << std::endl;
    std::cout << "║                     by Yedidya Schwartz                     ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << Colors::RESET << std::endl;
}

int main() {
    printHeader();
    
    TestFramework framework;
    
    std::cout << Colors::BOLD << Colors::MAGENTA << "🧪 Starting comprehensive utility tests..." << Colors::RESET << std::endl << std::endl;
    
    // Run all tests
    framework.runTest("Observer Pattern Basic Functionality", testObserverPattern);
    framework.runTest("ThreadBase Start/Stop Operations", testThreadBase);
    framework.runTest("QueueThread Task Execution & Exception Handling", testQueueThread);
    framework.runTest("TimerFd One-shot and Periodic Timers", testTimerFd);
    framework.runTest("Thread Safety & Concurrent Operations", testThreadSafety);
    
    std::cout << std::endl;
    framework.printSummary();
    
    return 0;
}