// Test if native module can be loaded
try {
  const nativeModule = require('./build/Release/audioSystemNative.node');
  console.log('✓ Native module loaded successfully');
  console.log('Available methods:', Object.keys(nativeModule));
  
  // Try creating an instance
  const audioSystem = new nativeModule.AudioSystem(44100);
  console.log('✓ AudioSystem instance created');
  console.log('AudioSystem methods:', Object.getOwnPropertyNames(Object.getPrototypeOf(audioSystem)));
  
  // Try starting audio
  audioSystem.start();
  console.log('✓ Audio started');
  
  // Try setting waveform
  audioSystem.setWaveform('sine');
  console.log('✓ Waveform set to sine');
  
  // Try playing a note
  audioSystem.triggerNote(440);
  console.log('✓ Note triggered at 440Hz');
  
  setTimeout(() => {
    audioSystem.triggerNoteOff();
    console.log('✓ Note stopped');
    audioSystem.stop();
    console.log('✓ Audio stopped');
    process.exit(0);
  }, 2000);
  
} catch (error) {
  console.error('✗ Error:', error.message);
  console.error(error.stack);
  process.exit(1);
}
