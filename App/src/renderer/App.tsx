import React from 'react';
import { Synthesizer } from './components/Synthesizer';
import './App.css';

/**
 * Main application component
 * Follows SOLID principles by delegating functionality to specialized components
 */
const App: React.FC = () => {
  return (
    <div className="app">
      <header className="app-header">
        <h1>SynthUI Desktop</h1>
      </header>
      <main className="app-main">
        <Synthesizer />
      </main>
    </div>
  );
};

export default App;
