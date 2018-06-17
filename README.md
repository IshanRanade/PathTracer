# Path Tracer

## Setup

### GPU Version Mac Setup
You must first compile the C++ source.  Navigate to the /GPU/src directory and run the following commands from the terminal.
```
brew install qt5
brew link --force qt5 && sudo ln -s /usr/local/Cellar/qt/5.11.0/mkspecs /usr/local/mkspecs && sudo ln -s /usr/local/Cellar/qt/5.11.0/plugins /usr/local/plugins
```

Then, navigate to the directory /GPU directory and create a directory build.  The navigate to the directory /GPU/build.
Then build the project with the following commands.  This will create the files necessary to run the program in the build directory and leave the src directory untouched.
```
cmake ../src/
make
./pathtracer
```
