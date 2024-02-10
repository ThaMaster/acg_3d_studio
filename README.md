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

The keyboard shortcuts are summarized in the table below.

<div align=center>

| Key        | Function                                                     | Progress    |
|:----------:|:------------------------------------------------------------:|:-----------:|
| W          | **Moves** camera **forward**                                 | Done        |
| S          | **Moves** camera **backwards**                               | Done        |
| A          | **Moves** camera **left**                                    | Done        |
| D          | **Moves** camera **right**                                   | Done        |
| R          | **Reloads** The scene                                        | Done        |
| SPACE      | **Resets** the camera position                               | In progress |
| SHIFT      | Makes the camera move **faster** when holding the button     | Done        |
| ESC        | **Exits** the application                                    | Done        |

</div>

It is worth mentioning that this table is subject to change during development of the program. Therefore it is good practice to always check this table if any uncertainties occur.

## XML node guide

This secition lists how to use each of the different nodes when creating a scene using the XML format. The first section explains the most basic nodes that is needed to create a basic scene. The later sections details some special nodes that can be used to switch shaders, create animation and more.

### Basic Nodes

This section details the basic nodes that are necessary to create a simple scene. The later sections will explain more complex nodes which can manipulate the scene in interesting ways.

#### Scene

```xml
    <scene name="scene_name">
        ...
    </scene>
```

#### Group

```xml
    <group name="group_name">
        ...
    </group>
```

#### Transform

```xml
    <transform name="transform_name" translate="x y z" rotate="x y z" scale="x y z">
        ...
    </transform>
```

#### Geometry

```xml
    <geometry name="geometry_name" path="path/to/model"/>
```

### Level of Detail (LOD)

```xml
    <lod name="lod_name" maxDistance="float">
        ...
    </lod>
```

### Update Nodes

```xml
    <update>
        ...
    </update>
```

#### Rotation

```xml
    <rotate speed="float" axis="x y z"/>
```

### State Nodes

```xml
    <state name="state_name" enableCullface="true/false" enableLight="true/false">
        ...
    </state>
```

#### Light

```xml
    <light position="x y z" specular="r g b" diffuse="r g b"/>
```

#### Material

```xml
    <material ambient="r g b" specular="r g b" diffuse="r g b" shininess="float"/>
```

#### Texture

```xml
    <texture path="path/to/texture"/>
```

#### Shaders

```xml
    <shaders vPath="path/to/vertex/shader" fPath="path/to/fragment/shader"/>
```

## Change log

This section details the changes that occurs with each session of working on this project. After each programming session a new list with all the things that have been done and other information will be posted. This is to have a clear view of what has been done and what is needed to be fixed/added to the project.

### Change log - 2024-02-08

- Calculates the bounding box correctly now. The camera correctly initializes at the correct position and direction.
- Implemented support for stating which shaders to use in the state in the XML file.
- Created some shaders including one that inverts the colors.
- Changed so that the scene node in the xml becomes a group, makes it possible to have state and *UpdateCallbakcs* for the whole scene.
- The geometries now initializes the shaders and uploads the data directly in the loader.
- Correctly handles the creation of the default scene.
- Can now have multiple LOD files and they each get tracked seperately.
- The distance between the camera and the LOD geometry now is calculated from the bounding box center of the geometry.

### Change log - 2024-02-06

- Implemented the *Level of Detail (LOD)* node. You supply the node with geometries that it will select from depending on the distance from the camera.
- The *LOD* node only contains *Group* nodes that represents objects.
- Added so that scenes now can be completely created from the scene XML files.
- States and *UpdateCallbacks* can be created in the XML files.

### Change log - 2024-02-05

- The state will contains the actual light objects.
- Refactored out each parsing of each node to their own function.
- Tested more object files and fixed some bugs.
- Implemented the *Map* that stores each loaded object and only loads from disk if it is not present in the map.
- Fixed so that the geometries only inits the shader and uploads all the data once when the *RenderVisitor* arrives to the node. This fixed the performance problem!
- Renamed the *M_Node* class to *Node* since this project no longer uses *Mesh* or the previous *Node* class.

### Change log - 2024-02-04

- Removed the Mesh and Node class from the project.
- The material is no longer stored directly in the geometry node. It is instead stored in the state only if a mesh has a material.
- The geometry is now responsible for all the buffers and it no longer uses the Mesh class to render object.
- Restructured such that if object file is loaded, create an *Group* node and create *Geometry* nodes for each mesh of the object.
- Light now uses the *Geometry* class for creating its own mesh (Might change!).

### Change log - 2024-02-03

- Added an additional light to all transform nodes to test if the state functions correctly, *(spoiler it does!)*.
- Created a *Merge()* function that aims to merge two states and return the result. This is to be used when the *RenderVisitor* traverses the scene graph. If the node does not have a state, simply copy all the current state information into the result and return.
- Created a rotation callback to test if the callback function works.
- Added so that each node have a vector of potential *UpdateCallbacks*. This way, each node can update several things in a singel node.

### Change log - 2024-02-02

- Started to implement the state class into the project.
- Implemented so that each node can have an *UpdateCallback* that gets called by a *UpdateVisitor* only if the callback is present.

### Change log - 2024-01-28

- Change so that each geometry calculate s its boudning box and then the group/transform nodes exapnds their bounding box depending on their children.
- If the xml contains multiple objects with the same node name, it will skip the file path and load the already existing object again.
- Continued to implement some things in the UpdateVisitor and the State classes.

### Change log - 2024-01-27

- Rendering now happens using the visitor design pattern.
- Created the file UpdateCallback which will be derived from when creating update callbacks for the update visitor.

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

### TODO - 2024-02-08

- Change the way the camera snaps when reloading the scene.
- Check the specification and control that all necessary requirements have been met.
- Implement the support for having different textures in the states in the XML file.
- Implement the support for having different material in the states in the XML file.
- Create additional *UpdateCallbacks*.
- Refactor the code.
- Comment the code.
- Write the report.

### TODO - 2024-02-06

- See over what the nodes should be able to contain, example "should the *LOD* node contain *States* or *UpdateCallbacks*?
- See over that everything works and test more scenes.
- Create additional *UpdateCallbacks*.
- Refactor the code.
- Comment the code.
- Write the report.

### TODO - 2024-02-05

- Implement the *Level of Detail (LOD)*. Maybe upload every object file to the buffer and use the *indexOffset* in **glDrawElements()** to determine which *LOD* to render.
- Adjust so that pressing the **SPACE** key correctly resets the view and fix the snapping when rotating camera after the reset.
- Create additional *UpdateCallback* functions.
- Implement more functionality with the parsing of the XML files.
- Comment the code!
- Write the report.

### TODO - 2024-02-04

- Move the storage of textures from the material to the state.
- Change so that the state only keeps track of which light should be enabled, and move the actual light vector to the scene class.
- Test new objects and create more test-scenes.
- Create more *UpdateCallbacks*.
- Introduce a map to the scene that holds all the *Object Group Nodes* for each loaded object.
- Make the **SPACE** key only reset the camera position and not reload the scene.
- Check dependencies and refactor so the code becomes good!
- Start to comment the code.

### TODO - 2024-02-03

- Remove the usage of the already present *Mesh* class to render the geometry. It should happen in geometry by itself.
- Implement the support of textures and materials in the state class.
- Make a function to construct your own scene graph where you can either load files or create your own geometry. (This makes it easiser to create a scene instead of needing to implement changes in the *Loader* class).
- Create additional *UpdateCallback* functions for cool effects.
- Create a document for the report.

### TODO - 2024-01-28

- Finish the implementation of the UpdateVisitor and state class.
- Remove some uneccassary windows build files.
- Think about how to implement the state functionality until the next working session.
- Change so that the objects does not get loaded if the same filepath is specified rather than the file name.

### TODO - 2024-01-27

- Finish the UpdateCallback function.
- Begin to implement the UpdateVisitor.
- NEED TO START IMPLEMENTING THE STATE FUNCTIONALITY!!!

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
