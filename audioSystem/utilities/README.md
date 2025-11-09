# Utilities

A shared utilities module for internal use across multiple projects. This repository contains common helper functions, classes, and reusable logic designed to simplify development and improve code maintainability.

---

## Features

- **Thread Management**: Base classes for creating and managing threads (`ThreadBase`, `QueueThread`).
- **Observer Pattern**: Implementation of the observer pattern (`Subject`, `IObserver`).
- **Timer Functionality**: High-performance timer using file descriptors (`TimerFd`).

---

## File Structure

```
├── .gitignore         # Git ignore rules
├── LICENSE            # MIT License
├── README.md          # Project documentation
├── IObserver.h        # Observer interface
├── subject.h          # Subject class for observer pattern
├── subject.cpp        # Implementation of Subject class
├── threadBase.h       # Base class for thread management
├── threadBase.cpp     # Implementation of ThreadBase
├── QueueThread.h      # Thread with task queue
├── QueueThread.cpp    # Implementation of QueueThread
├── TimerFd.h          # Timer class using file descriptors
├── TimerFd.cpp        # Implementation of TimerFd
```

---


## Usage

### Thread Management

Use `ThreadBase` to create custom threads:

```cpp
class MyThread : public ThreadBase {
protected:
    void thread() override {
        while (m_running) {
            // Your thread logic here
        }
    }
};
```

### Observer Pattern

Attach observers to a subject:

```cpp
Subject subject;
MyObserver observer;
subject.attach(&observer);
subject.notify();
```

### Timer Functionality

Set up a timer with `TimerFd`:

```cpp
TimerFd timer;
timer.SetTimer(std::chrono::milliseconds(1000), std::chrono::milliseconds(500));
timer.Start();
```

---

## Testing

The utilities library comes with a comprehensive test suite that validates all functionality with beautiful CLI output.

### Running Tests

**Option 1: Using the test runner script (recommended)**
```bash
./run_tests.sh
```

**Option 2: Using Makefile directly**
```bash
make test
```

**Option 3: Manual compilation and execution**
```bash
make clean
make
./test_utilities
```

### Test Coverage

The test suite covers:
- ✅ **Observer Pattern**: Attach/detach observers, notifications with/without parameters, null pointer handling
- ✅ **ThreadBase**: Thread start/stop operations, race condition prevention, exception handling
- ✅ **QueueThread**: Task execution, exception handling in tasks, null task filtering
- ✅ **TimerFd**: One-shot and periodic timers, proper cleanup, timeout accuracy
- ✅ **Thread Safety**: Concurrent operations across multiple threads

### Memory Testing

Run tests with Valgrind for memory leak detection:
```bash
./run_tests.sh --valgrind
# or
make valgrind
```

---

## License

This project is licensed under the [MIT License](LICENSE).

---

## Contributing

Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.

---




