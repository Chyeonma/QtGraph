# Windows Portable Build

This project can be packaged for Windows as a portable `.zip` archive.

## What the package contains

- `QtGraph.exe`
- Qt runtime DLLs copied by `windeployqt`
- platform plugins needed to launch the app on Windows
- Microsoft C++ runtime copied with `--compiler-runtime`

The result is a folder that can be unzipped and launched directly on a Windows machine without installing Qt manually.

## GitHub Actions workflow

Workflow file:
- `.github/workflows/windows-portable.yml`

How to use it:
1. Push the project to a GitHub repository.
2. Open the `Actions` tab on GitHub.
3. Run `Build Windows Portable`.
4. Download the `QtGraph-windows-portable` artifact.
5. Send the generated `.zip` file to the Windows user.

## Manual Windows build

Prerequisites:
- Qt 6 for MSVC 2022
- CMake
- Visual Studio 2022 Build Tools or Visual Studio 2022

Build and deploy:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
./scripts/windows/deploy-portable.ps1 -BuildDir build -Configuration Release -AppName QtGraph -OutputRoot dist
```

Expected output:
- `dist/QtGraph-windows-portable.zip`

## Notes

- Fonts and default settings are already embedded in the app resources.
- Runtime user settings are still created on the target Windows machine in the user's config directory when the app runs for the first time.
