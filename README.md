# QtGraph

`QtGraph` is a desktop project explorer and code workspace built with `Qt 6 Widgets`, `C++17`, and `CMake`.

It focuses on a lightweight workflow for:
- opening a project folder
- browsing files and directories in a tree view
- viewing and editing text files in tabs
- syntax highlighting for multiple languages
- simple canvas-based project visualization
- font and editor settings stored per user

## Main Features

- Editable text tabs with `Save`, `Undo`, `Redo`, `Cut`, `Copy`, and `Paste`
- Syntax highlighting for `C/C++`, `Python`, `Java`, `JavaScript`, `TypeScript`, `HTML`, `CSS`, and `JSON`
- Safe handling for unsupported or very large files
- Welcome tab with project entry actions
- Settings tab for UI font, editor font, and default line numbers
- Tree view file operations including create, rename, delete, copy, cut, paste, and drag-and-drop move/copy
- External file change detection with reload/keep-current-buffer choice

## Build Requirements

- `CMake >= 3.16`
- `Qt 6` with `Core`, `Gui`, and `Widgets`
- A C++17 compiler

## Build on Linux

```bash
cmake -S . -B build
cmake --build build
./build/QtGraph
```

## Build on Windows

Open the project with `Qt Creator` or configure it with `CMake` using a Qt 6 desktop kit.

## Windows Portable Release

This repository already includes:
- a GitHub Actions workflow to build a Windows portable package
- a PowerShell deploy script using `windeployqt`

See:
- `docs/windows-portable.md`

## Project Structure

- `src/core`: filesystem and font helpers
- `src/editor`: code viewer and syntax highlighters
- `src/settings`: app settings model, persistence, and settings tab
- `src/ui`: main window, tabs, welcome view, tree view, and canvas view
- `resources`: bundled fonts and Qt resources
- `.github/workflows`: CI workflow for Windows packaging

## Notes

- Runtime user settings are written to the current user's config directory.
- Fonts and default settings are embedded into the app resources.
