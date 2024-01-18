Anders Backman,

Umeå University
2023-12-22

1. DESCRIPTION
2. BUILDING
  2.1 LINUX
  2.2 WINDOWS
3. EXECUTING
4. XML format
5. Create your first application

# 1. DESCRIPTION

VRLIB is a collection of handy classes for building some examples and a very simple rendering engine.

The environment variable VR_PATH is used to locate files. So make sure it is set by executing setup_env.bat
(or do source setup_env.bash under Unix)

- viewer
  A "complete" application for loading 3D model files using Assimp
  Use this as a start of your development (if you like).
  It has support for a very simple xml file format. I recommend that you try to use something like this to build your scenes.
  It will speed up your modeling A LOT!

If you want to create another application (assignment), just copy the viewer directory with everything in it and name it... lab1 or something.
Then run cmake in the root directory of vrlib and you will have another application ready for development while keeping the previous one.

# 2. BUILDING

## 2.0 dependencies

Notice that you need first need to execute setup_env.bat/bash depending on your platform before continuing.
Then depending on your platform, start a command shell and run build_all.bat or build_all.bash.
On Unix, png, jpeg and zlib will be required pre-installed dependencies.

## 2.1 LINUX

Note: The scripts (build_all.bash/setup_env.bash) assumes you can or are using the bash shell.
So if you are using some other shell, just look into setup_env.bash and mimic whatever you have to expose the same environment variables as that script.

Linux usually have a lot of stuff available as packages (whereas windows have none)
You need to make sure you have glfw, png, zlib, freetype installed in your default paths.
These are usually installed using for example apt install (Ubuntu).

Example of how to install all the dependencies on Ubuntu 22.04:

sudo apt-get install libglfw3 -y
sudo apt-get install libglfw3-dev -y
sudo apt-get install libfreetype6-dev -y
sudo apt-get install zlib1g -y
sudo apt-get install libpng-dev -y

In addition you might need some more libraries to get the graphics to work:

sudo apt install libxcursor-dev -yXxf86vm -y
sudo apt-get install libxxf86vm-dev -y
sudo apt-get install libxinerama-dev -y
sudo apt-get install libxi-dev -y

sudo apt-get upgrade mesa-common-dev libgl1-mesa-dev libglu1-mesa-dev -y

Next, make sure you run the setup file to setup some environment variables:

> source setup_env.bash

Next, you need to build some of the dependencies (in the dependencies directory)
See build_all.bash

You might need to change permission on the script:

> chmod 755 build_all.bash

Then you can run it:

> ./build_all.bash

Everything should end up in the installed directory (after a make install)

When you have run build_all.bat once, you do not need to run it again.
It is only for building all the dependencies once.

See chapter 5 below.

Look into bin directory for the executables.

Before running the viewer you have to set an environment variable VR_PATH.
This should point to the vrlib directory (where the sub-directories models, shaders exist).
This should have been set in the setup_env.bash that you already executed.

Now if you want to build only the examples again, just write:

> make
> make install

In the top directory

## 2.2 WINDOWS

First, make sure you run the script to setup the environment:

> setup_env.bat /VS2022 (or whatever VS version you are using) (Default is VS2022)

ONLY 64 Bit is currently supported (lets forget about 32bit, ok?)

You might get an error about a missing visual studio. Make sure you choose the correct one!
The default might not exist on your computer.

Notice that everything is built with RELEASE. There will be no DEBUG libraries.
This means, that if you want to build things in DEBUG in your project...you will have to do some work.
A lot of it. Meaning, editing the build_all.bat to reflect that you want to build everything now with debug.

I would strongly recommend using RelWithDebInfo. That gives you just as much information as debug, but it is faster.
You might want to disable optimization and inline for your code (google how to do that).
Do NOT mix Debug and Release in the same project. That will cause crasches. Always. (Blame Visual Studio for that.)


Next, to build everything you should run build_all.bat

To start visual studio with the project (vrlib) then just go to build\vrlib\ where you find vrlib.sln
Write devenv /useenv vrlib.sln to start visual studio with this project.

When you have run build_all.bat once, you do not need to run it again.
It is only for building all the dependencies once.

If you now want to build only the examples just do:

> devenv /useenv vrlib.sln

To add your own example, See chapter 5 below.

# 3. EXECUTING

Running from debugger in VisualStudio, require you to set the path (PATH) so all the binaries can be found.
This should be done if you execute setup_env.bat BEFORE starting visual studio with devenv /useenv vrlib.sln
If you start visual studio by double clicking the vrlib.sln file, you might not get the correct PATH.

usage:

> viewer scenes/monkey.xml
> viewer models/monkey.obj
> viewer scenes/residentialBuildings.xml

Keyboard bindings:

'r' - reload the scene
'wasd' - Left/Right/Forward/Backward
'ctrl' - Move up
'space' - Move down
'shift' - Increase speed
'esc' - quit the application

# 4. XML format

In the directory scenes there are examples of an xml format which can be read using the objViewer (files with extension .xml)

Below is an example of this format. It should be easy to understand.
The parsing of this format is done in Loader.cpp. Notice that it is not very error safe.

<?xml version="1.0" encoding="utf-8"?>
<scene>
  <node name="monkey">
    <file path="models/monkey.obj"> </file>
    <transform translate="0 0 0" rotate="0 0 0" scale="2 2 2"></transform>
  </node>
</scene>

# 5. Create your first application

Go to the application directory
Copy the viewer directory to for example lab1
Run cmake in the top directory again.
run make then make install:

> cd applications
> cp -r viewer lab1
> cd ..
> cmake .
> make
> make install
> lab1

Problems?

email: andersb@cs.umu.se
