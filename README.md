# HASP_P2

this repository is the home for COC's HASP 2025 P2 specific code and tools. It will house everything needed to allow a few students with properly configured systems to compile and load code onto the Parallax inc. P2 Edge board.

## Required Tools + config

1) [flexprop](https://github.com/totalspectrum/flexprop/releases), with .../flexprop/bin/flexcc and .../flexprop/bin/loadp2 executables on the system path.
2) [VSCode](https://code.visualstudio.com/download)


## Using (flexprop):
this repository is set up to have the following includes:
                "C:/your/path/to/flexprop/include",
                "${workspaceFolder}/include",
                "${workspaceFolder}/spin",
                "${workspaceFolder}"

Should you choose to use the flexprop IDE, you would need to add these to your include path. this can be done in the ide via file>Library directories. I personally dislike the ide as it does not give me the tools or the flexibility I desire.

## Using (VSCode)
VSCode doesn't love the flex specific syntax in files. The recommended extensions will give you syntax highlights, but will also complain with red squiggles under valid flex style code. you may disable the underline squiggles for C code with the ctrl+shift+p > "C/C++: Disable Error Squiggles" command, if you like.

The easiest way to debug your code is to just try building it and debugging the old fashioned way.

Currently two tasks are configured for compiling and loading the code to a P2, and these tools will only work with flexcc/flexprop

to compile and run a file, press ctrl+shift+p, select "Tasks: Run task", and select the "Build and run P2" option

to build only, do the same as above but select the "Build FlexSpin Propeller P2 Program" task.