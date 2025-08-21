cl /std:c++20 /DNOMINMAX /DUNICODE /DTE_WINDOWING /DTE_DX11 /nologo /Fobuild\ /Fdbuild\ /Febuild/tiny_engine_sample.exe ^
/EHsc /FS /Zi /MDd sample.cpp ^
/link /SUBSYSTEM:CONSOLE user32.lib gdi32.lib d3d11.lib dxguid.lib dsound.lib
