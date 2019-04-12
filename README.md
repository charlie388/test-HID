# testHID
The Qt test program for hidapi

Only test in windows 10

1. Download msys2
   reference https://www.devdungeon.com/content/install-gcc-compiler-windows-msys2-cc  
   execute "MSYS2 MinGW 64-bit" shell  
   install package : base-devel git mingw-w64-x86_64-gcc  

2. git clone these two project on the same level  
	     Parent_Folder  
	       |  
	       +hidapi  
	       +test-HID  

3. build hidapi. please read README.txt of hidapi project.  
   Reference to "Building HIDAPI into a shared library on Unix Platforms:".  

2. Double click test-HID.pro and go into "Qt Creator" IDE.  
   Build test-HID project.  
