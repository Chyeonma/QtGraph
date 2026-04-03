# Settings Folder

This folder stores the settings structure used by the app.

Structure:
- `defaults/default-settings.json`: default values bundled into the app resources
- `data/app-settings.json`: legacy in-repo sample layout for development reference

Current supported settings:
- `uiFontFamily`
- `uiFontSize`
- `editorFontFamily`
- `editorFontSize`
- `defaultShowLineNumbers`

At runtime, the app now saves the active configuration into the user's writable config directory returned by `QStandardPaths::AppConfigLocation`.
