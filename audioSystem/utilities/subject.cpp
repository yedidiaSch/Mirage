#include "subject.h"
#include <algorithm>

void Subject::attach(IObserver* observer) 
{
    if (observer == nullptr) {
        return;
    }
    
    // Check if observer is already attached to avoid duplicates
    auto it = std::find(observers.begin(), observers.end(), observer);
    if (it == observers.end()) {
        observers.push_back(observer);
    }
}

void Subject::detach(IObserver* observer) 
{
    if (observer == nullptr) {
        return;
    }
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::notify(void* params) 
{
    for (IObserver* observer : observers) 
    {
        if (observer != nullptr) {
            observer->update(params);
        }
    }
}
