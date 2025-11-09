#pragma once

#include <string>
#include <memory>
#include <unordered_map>

/**
 * Abstract base class for effect parameters
 */
class IEffectParameters {
public:
    virtual ~IEffectParameters() = default;
    virtual std::string getEffectName() const = 0;
    virtual void reset() = 0; // Reset to default values
    virtual std::unique_ptr<IEffectParameters> clone() const = 0;
};

/**
 * Parameters for DelayEffect
 */
class DelayParameters : public IEffectParameters {
public:
    float delayTime = 0.3f;    // Delay time in seconds (0.1 - 2.0)
    float feedback = 0.3f;     // Feedback amount (0.0 - 0.9)
    float mix = 0.5f;          // Dry/wet mix (0.0 = dry, 1.0 = wet)

    std::string getEffectName() const override { return "delay"; }
    
    void reset() override {
        delayTime = 0.3f;
        feedback = 0.3f;
        mix = 0.5f;
    }
    
    std::unique_ptr<IEffectParameters> clone() const override {
        auto params = std::make_unique<DelayParameters>();
        params->delayTime = delayTime;
        params->feedback = feedback;
        params->mix = mix;
        return params;
    }
};

/**
 * Parameters for LowPassEffect
 */
class LowPassParameters : public IEffectParameters {
public:
    float cutoffFreq = 1000.0f;  // Cutoff frequency in Hz (20 - 20000)
    float resonance = 0.7f;      // Filter resonance (0.1 - 2.0)

    std::string getEffectName() const override { return "lowpass"; }
    
    void reset() override {
        cutoffFreq = 1000.0f;
        resonance = 0.7f;
    }
    
    std::unique_ptr<IEffectParameters> clone() const override {
        auto params = std::make_unique<LowPassParameters>();
        params->cutoffFreq = cutoffFreq;
        params->resonance = resonance;
        return params;
    }
};

/**
 * Parameters for OctaveEffect
 */
class OctaveParameters : public IEffectParameters {
public:
    float octaveShift = 1.0f;    // Octave shift multiplier (0.5, 1.0, 2.0, 4.0)
    float mix = 0.5f;            // Dry/wet mix (0.0 = dry, 1.0 = wet)

    std::string getEffectName() const override { return "octave"; }
    
    void reset() override {
        octaveShift = 1.0f;
        mix = 0.5f;
    }
    
    std::unique_ptr<IEffectParameters> clone() const override {
        auto params = std::make_unique<OctaveParameters>();
        params->octaveShift = octaveShift;
        params->mix = mix;
        return params;
    }
};

/**
 * Container for all effect parameters
 */
class EffectParametersManager {
public:
    void setParameters(const std::string& effectName, std::unique_ptr<IEffectParameters> params) {
        parameters_[effectName] = std::move(params);
    }
    
    template<typename T>
    T* getParameters(const std::string& effectName) {
        auto it = parameters_.find(effectName);
        if (it != parameters_.end()) {
            return dynamic_cast<T*>(it->second.get());
        }
        return nullptr;
    }
    
    void resetAll() {
        for (auto& pair : parameters_) {
            pair.second->reset();
        }
    }
    
    void clear() {
        parameters_.clear();
    }

private:
    std::unordered_map<std::string, std::unique_ptr<IEffectParameters>> parameters_;
};