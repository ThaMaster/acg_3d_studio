# ACG 3D Studio

Repository for the 3D studio created in the Advanced Computer Graphcis course.

## Compiling

It is very simple to compile all the applications that are written in this repository. Since all the necessary files are located in the *applications* folder, the environment variables must first be set with the following command:

```shell
/$ source setup_env.bash
```

Next run the **build_all.bash** script to build all the dependencies. You may need to change the permissons of the file and can be done with the following command:

```shell
/$ chmod 755 build_all.bash
```

Then all dependencies have been built and simple use *make* and *make install* to build the files located in the *applications* folder:

```shell
/$ make & make install
```

Since the environemnt variables already have been set, you can start the different files at your current location and do not need to find the executable.

### Bash files

The given bash files are DOS text files and must therefore be converted to a Unix text file. This can be done by executing the following command:

> dos2unix bash_script.sh

## Keyboard shortcuts

This section lists all the differnet keybinds used for navigating and using the 3D studio. A graphical user interface (GUI) will be developed which will be the primary way of navigation and transformation but these keyboard shortcuts are made for an enhanced user experience.

The keyboard shortcust are summarized in the table below.

<div align=center>

| Key        | Function                                                     | Progress    |
|:----------:|:------------------------------------------------------------:|:-----------:|
| W          | **Moves** camera **forward**                                 | Done        |
| S          | **Moves** camera **backwards**                               | Done        |
| A          | **Moves** camera **left**                                    | Done        |
| D          | **Moves** camera **right**                                   | Done        |

</div>

It is worth mentioning that this table is subject to change during development of the program. Therefore it is good practice to always check this table if any uncertainties occur.

## Change log

This section details the changes that occurs with each session of working on this project. After each programming session a new list with all the things that have been done and other information will be posted. This is to have a clear view of what has been done and what is needed to be fixed/added to the project.

### Change log - 2024-01-25

- Started integrating the Visitor Design Pattern in the lab1 application.
- Added a folder for scenes that contains test scenes that should work for lab1. The folder is located [here](/scenes/lab1/).
- Implemented a recursive function that travels the xml files. The function takes a *root group* that will be altered when the function returns.
- Some things are added to some nodes in preperation for later integration.

### Change log - 2024-01-24

- Modified the test of the visitor pattern so that it now uses the accept function form the Transform and not from the Group. (May change)
- Deleted old attempt at implementing the specification content.
- Created mulitple folders containing a more structured architecture for attempting to implementing the required parts.

## TODO

This section details what is to be done in the next programming session. This section is an attempt at keeping a good focus on what to work on the next time a programming session comences.

### TODO - 2024-01-25

- Perform rendering with the render node instead of the way that the provided *viewer* application handles it.
- Remove prints from the nodes and visitors.
- Implement the state class.
- Check wether the communication is correct between the implemented componets in regards to the specification.
- Comment the code.

### TODO - 2024-01-24

- Decide if the accept() function should be from the Group or the Transform.
- Continue to implement the unfinished components.
- Start to comment parts of the code in an attempt at making it less tedious later.
- Read up on the [Visitor Design Pattern](https://sourcemaking.com/design_patterns/visitor/cpp/2).

## Helpful resources when developing the 3D Studio

Below follows some useful links and resources to the different parts of the project. These resources includes reference pages, references cards, manuals, documentation and more.

### C++ Resources

This section includes all the learning resources used to get accustomed with the C++ programming language.

- [C++ Reference Page](https://en.cppreference.com/w/cpp)
- [C++ Tutorial](https://www.learncpp.com/?utm_content=cmp-true)
- [C++ Tour](https://isocpp.org/tour)
- [C++ Documentation Guidelines](https://developer.lsst.io/cpp/api-docs.html#)