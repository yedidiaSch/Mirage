/**
 * Preload script for Electron
 * Runs before the renderer process loads
 */

import { contextBridge } from 'electron';

// Expose protected methods that allow the renderer process to use
// the native audio system while maintaining security
contextBridge.exposeInMainWorld('electron', {
  platform: process.platform,
  version: process.versions.electron
});
