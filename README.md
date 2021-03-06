# digitalsim Digital Logic Simulator

## Requirements
- Visual Studio 2019
  - Unit Tests require the Google Test Library
- GUI Library
  - _SDL2_ (2.0.8) [link](https://www.libsdl.org/download-2.0.php)
  - _SDL2_image_ (2.0.3) [link](https://www.libsdl.org/projects/SDL_image/)
  - _SDL2_ttf_ (2.0.14) [link](https://www.libsdl.org/projects/SDL_ttf/)

## Configuration
  SDL Library paths are configured in `CoreUI/SDLPath.props`.
  The following variables need to be updated to match your environment:
    - _SDLBase_: base directory of the _SDL2_ library
    - _SDLImageBase_: base directory of the _SDL2_image_ library
    - _SDLTTFBase_: base directory of the _SDL2_ttf_ library

## Components
- _CoreUI_: GUI Library (will be moved to separate project eventually)
- _DigiLib_: Digital Library
- _profile_: Profiling project
- _proto_: Test app (console)
- _protoGUI_: Test app (GUI)
- _Tests_: Unit tests
