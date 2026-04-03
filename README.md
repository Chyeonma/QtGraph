# QtGraph

**QtGraph** là ứng dụng desktop dùng để duyệt, chỉnh sửa mã nguồn và trực quan hoá cấu trúc dự án. Được xây dựng bằng **C++17**, **Qt 6 Widgets** và **CMake**.

## Tính Năng Chính

- **Editor đa tab** — mở nhiều file đồng thời, hỗ trợ Save / Save All / Undo / Redo / Cut / Copy / Paste
- **Syntax highlighting** — hỗ trợ sẵn 8 ngôn ngữ: C/C++, Python, Java, JavaScript, TypeScript, HTML, CSS, JSON
- **Quản lý file project** — tạo, đổi tên, xóa, copy, cut, paste file/folder ngay trong tree view
- **Drag & Drop** — kéo thả file/folder giữa các thư mục trong project
- **Xử lý file an toàn** — cảnh báo file binary, file text quá lớn, phát hiện thay đổi bên ngoài (external change detection)
- **Canvas View** — hiển thị cấu trúc project dưới dạng đồ thị (graph nodes)
- **Cài đặt tuỳ chỉnh** — font UI, font editor, hiển thị số dòng — lưu dưới dạng JSON
- **Plugin system** — nạp plugin động (`.so`/`.dll`) qua `QPluginLoader`, mở rộng syntax highlighting và tính năng mà không cần build lại app

## Kiến Trúc

Dự án sử dụng kiến trúc phân tầng 3 lớp với Dependency Injection:

```
src/
├── app/              # AppContext — DI Container, khởi tạo toàn bộ hệ thống
├── interfaces/       # Hợp đồng trừu tượng (IFileIO, IConfigStore, IFontProvider, ...)
├── models/           # Data structs thuần túy (AppSettings, FileClipboard, ...)
├── core/             # Triển khai cấp thấp (FileIO, FontProvider, JsonConfigStore, PathUtils)
├── services/         # Logic nghiệp vụ (ProjectService, FileService, EditorService, SettingsService, PluginManager)
├── editor/           # CodeViewer widget + HighlighterRegistry + 8 language highlighters
├── ui/               # Giao diện mỏng (MainWindow, TabManager, ProjectTreeView, CanvasView, ...)
└── plugins/sdk/      # Header tiện ích cho nhà phát triển plugin
```

| Tầng | Vai trò |
|------|---------|
| **Interfaces** | Định nghĩa các abstract class — mọi tầng chỉ phụ thuộc vào đây |
| **Core** | Triển khai thao tác hệ thống (file I/O, font, config JSON) |
| **Services** | Quản lý trạng thái và logic nghiệp vụ, phát Qt signals |
| **UI** | Nhận sự kiện người dùng → gọi Service → hiển thị kết quả |
| **AppContext** | DI Container — tạo và wiring tất cả components |

## Yêu Cầu Build

- **CMake** ≥ 3.16
- **Qt 6** (modules: Core, Gui, Widgets)
- Trình biên dịch hỗ trợ **C++17**

## Build Trên Linux

```bash
cmake -S . -B build
cmake --build build
./build/QtGraph
```

## Build Trên Windows

Mở project bằng **Qt Creator** hoặc cấu hình CMake với Qt 6 desktop kit.

### Windows Portable Release

Repository này bao gồm sẵn:
- GitHub Actions workflow để build bản portable cho Windows
- Script PowerShell sử dụng `windeployqt`

Xem chi tiết: `docs/windows-portable.md`

## Phát Triển Plugin

Plugin là shared library (`.so` / `.dll`) implement interface `IPlugin`. Chỉ cần:

1. Include `plugins/sdk/pluginsdk.h`
2. Implement các method: `name()`, `version()`, `initialize(AppContext*)`, `shutdown()`
3. Build thành `.so` / `.dll` và đặt vào thư mục `plugins/` cạnh executable

Ứng dụng sẽ tự động phát hiện và nạp plugin khi khởi động.

## Ghi Chú

- Cài đặt người dùng được lưu dưới dạng JSON trong thư mục config của hệ điều hành
- Font và cài đặt mặc định được nhúng sẵn trong app resources (`.qrc`)
- Dự án sử dụng hoàn toàn Dependency Injection — mọi service có thể mock được cho unit test
