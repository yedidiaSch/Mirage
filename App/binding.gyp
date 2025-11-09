{
  "targets": [
    {
      "target_name": "audioSystemNative",
      "cflags!": ["-fno-exceptions"],
      "cflags_cc!": ["-fno-exceptions"],
      "sources": [
        "native/AudioSystemWrapper.cpp",
        "../audioSystem/src/Core/audioSystem.cpp",
  "../audioSystem/src/Core/audioDevice.cpp",
        "../audioSystem/src/Adapters/AudioSystemAdapter.cpp",
        "../audioSystem/src/Config/ConfigReader.cpp",
        "../audioSystem/src/Effects/IEffect.cpp",
        "../audioSystem/src/Effects/DelayEffect.cpp",
        "../audioSystem/src/Effects/LowPassEffect.cpp",
        "../audioSystem/src/Effects/OctaveEffect.cpp",
        "../audioSystem/src/Midi/MidiDevice.cpp",
        "../audioSystem/src/Waves/SineWave.cpp",
        "../audioSystem/src/Waves/SquareWave.cpp",
        "../audioSystem/src/Waves/SawtoothWave.cpp",
        "../audioSystem/src/Waves/TriangleWave.cpp",
        "../audioSystem/src/Envelope/ADSREnvelope.cpp",
        "../audioSystem/utilities/subject.cpp",
        "../audioSystem/utilities/threadBase.cpp",
        "../audioSystem/utilities/QueueThread.cpp",
        "../audioSystem/utilities/TimerFd.cpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "../audioSystem/src",
        "../audioSystem/src/Core",
        "../audioSystem/src/Adapters",
        "../audioSystem/src/Common",
        "../audioSystem/src/Config",
        "../audioSystem/src/Effects",
        "../audioSystem/src/Waves",
        "../audioSystem/src/Envelope",
        "../audioSystem/src/Midi",
        "../audioSystem/utilities",
        "/usr/include/rtaudio",
        "/usr/include/rtmidi",
        "/usr/include/libxml2"
      ],
      "dependencies": [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "defines": [],
      "cflags_cc": [
        "-std=c++17",
        "-frtti",
        "-fexceptions"
      ],
      "conditions": [
        [
          "OS=='linux'",
          {
            "libraries": [
              "-lrtaudio",
              "-lrtmidi",
              "-lpthread",
              "-lxml2"
            ]
          }
        ]
      ]
    }
  ]
}
