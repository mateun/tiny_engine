cl /std:c++20 /DNOMINMAX /DUNICODE /DTE_WINDOWING /DTE_DX11 /nologo /Fobuild\ /Fd"build\obj.pdb" /Febuild/tiny_engine_sample.exe ^
/EHsc /FS /Zi /MDd /Od sample.cpp ^
/link ^
/SUBSYSTEM:CONSOLE user32.lib gdi32.lib d3d11.lib dxguid.lib d3dcompiler.lib dsound.lib
