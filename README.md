# Path Tracer

## Setup

### GPU Version Mac Setup
You must first compile the C++ source.  Navigate to the GPU/src directory and run the following commands from the terminal.
```
brew install qt5
brew link --force qt5 && sudo ln -s /usr/local/Cellar/qt/5.11.0/mkspecs /usr/local/mkspecs && sudo ln -s /usr/local/Cellar/qt/5.11.0/plugins /usr/local/plugins
```

The build the project with the following commands.
```
cmake .
make
./pathtracer
```

Next you must run the UI.  Navigate to the GPU/src/ui directory.
First, you must install all the dependencies.  Import the necessary python imports with the following commands.
```
brew install sqlite
brew link sqlite

brew install xz
brew link xz

brew install python
brew link python

brew install python@2
brew link python@2

brew install sip
brew link sip

export PYTHONPATH=/usr/local/lib/python2.7/site-packages:$PYTHONPATH
```




Then, make sure that you are using Python 2.7.  Run the UI with the following command.
```
python ui.py
```