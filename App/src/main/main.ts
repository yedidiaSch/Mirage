import { app, BrowserWindow } from 'electron';
import * as path from 'path';

/**
 * Main process for Electron application
 * Handles window creation and lifecycle management
 */
class Application {
  private mainWindow: BrowserWindow | null = null;

  constructor() {
    this.initializeApp();
  }

  private initializeApp(): void {
    // Quit when all windows are closed (except on macOS)
    app.on('window-all-closed', () => {
      if (process.platform !== 'darwin') {
        app.quit();
      }
    });

    // Create window when app is ready
    app.on('ready', () => {
      this.createWindow();
    });

    // Recreate window on macOS when dock icon is clicked
    app.on('activate', () => {
      if (BrowserWindow.getAllWindows().length === 0) {
        this.createWindow();
      }
    });
  }

  private createWindow(): void {
    this.mainWindow = new BrowserWindow({
      width: 1200,
      height: 800,
      webPreferences: {
        nodeIntegration: true,
        contextIsolation: false
        // No preload script needed with nodeIntegration enabled
      },
      title: 'SynthUI Desktop',
      backgroundColor: '#1e1e1e'
    });

    // Load the renderer
    // For now, always load from file until we set up dev server
    this.mainWindow.loadFile(path.join(__dirname, 'renderer/index.html'));

    this.mainWindow.on('closed', () => {
      this.mainWindow = null;
    });
  }
}

// Initialize application
new Application();
