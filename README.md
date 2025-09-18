# ClipTheScreen
**WORK-IN-PROGRESS**

A Windows desktop application that allows users to select any portion of their screen and record it as a video. Perfect for creating quick screen recordings, tutorials, or capturing specific areas of your desktop.

https://github.com/user-attachments/assets/aac5846a-3968-43f3-abb5-20552059109c

## Features

- **Interactive Screen Selection**: Click and drag to select any rectangular area of your screen
- **Multi-Monitor Support**: Works across multiple joined monitors
- **Configurable Recording Settings**: Customize FPS and recording duration via configuration file
- **Real-time Preview**: Visual feedback during area selection with overlay
- **Automatic Playback**: Automatically opens the recorded video after completion
- **High Performance**: Optimized screen capture using Windows GDI and OpenCV

## How It Works

1. **Launch the application** - Run `ScreenRecorder.exe`
2. **Select recording area** - Click and drag to create a selection rectangle
3. **Automatic recording** - The application captures the selected area for the configured duration
4. **Video output** - The recorded video is saved as `vid.mp4` and automatically opened

## Configuration

The application uses a `config.txt` file to control recording parameters:

```
[clipduration]=10
[fps]=30
```

### Configuration Options

- **`[clipduration]`**: Recording duration in seconds (default: 10)
- **`[fps]`**: Frames per second for the recording (default: 30)

## Requirements

### System Requirements
- Windows 10 or later
- Visual C++ Redistributable (included in build)

### Dependencies
- **OpenCV 4.x**: Computer vision library for video processing
- **FFmpeg**: Video encoding/decoding (avcodec, avformat, avutil, swscale, swresample)
- **libcurl**: HTTP client library (for potential upload functionality)
- **OpenSSL**: Cryptographic functions
- **Windows SDK**: For Windows API functions

## Building from Source

### Prerequisites
- Visual Studio 2019 or later
- vcpkg package manager
- Windows 10 SDK

### Build Steps

1. **Install dependencies via vcpkg**:
   ```bash
   vcpkg install opencv4:x64-windows
   vcpkg install curl:x64-windows
   vcpkg install openssl:x64-windows
   ```

2. **Open the solution**:
   - Open `ScreenRecorder.sln` in Visual Studio

3. **Build the project**:
   - Select Release|x64 configuration
   - Build → Build Solution

4. **Run the executable**:
   - The built executable will be in `x64/Release/ScreenRecorder.exe`

## Project Structure

```
ClipTheScreen/
├── ScreenRecorder/           # Main project directory
│   ├── main.cpp             # Source code
│   ├── config.txt           # Configuration file
│   └── ScreenRecorder.vcxproj # Visual Studio project file
├── x64/                     # Build output directory
│   ├── Debug/               # Debug build files
│   └── Release/             # Release build files
└── ScreenRecorder.sln       # Visual Studio solution file
```

## Technical Details

### Core Technologies
- **Language**: C++20
- **Graphics**: Windows GDI for screen capture
- **Video Processing**: OpenCV for video encoding
- **UI**: Windows API for overlay window
- **Video Format**: MP4 with H.264 encoding

### Key Components
- **Screen Capture**: Uses `BitBlt` for efficient screen region capture
- **Overlay Window**: Transparent overlay for area selection
- **Video Encoding**: OpenCV VideoWriter for MP4 output
- **Configuration**: Simple text-based configuration system

## Usage Tips

- **Minimum Selection**: The application requires a minimum selection size of 5x5 pixels
- **Performance**: Higher FPS settings will result in larger file sizes
- **Multi-Monitor**: Works seamlessly across multiple monitors
- **File Location**: Videos are saved in the same directory as the executable

## Troubleshooting

### Common Issues
- **"Missing DLL" errors**: Ensure all required DLLs are in the same directory as the executable
- **Poor performance**: Try reducing the FPS setting in `config.txt`
- **Large file sizes**: Consider lowering the FPS or reducing the recording duration

### Required DLLs
The following DLLs must be present alongside the executable:
- `opencv_*.dll` (OpenCV libraries)
- `av*.dll` (FFmpeg libraries)
- `libcurl*.dll` (libcurl library)
- `libcrypto*.dll` (OpenSSL library)

## License

This project is licensed under the terms specified in the LICENSE file.

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

## Future Enhancements

- Audio recording support
- Multiple output formats
- Cloud upload integration
- GUI configuration interface
- Hotkey support for quick recording


