# gazebo_model_spawner

This plugin allows for generation of objects at different times and locations in a Gazebo simulation. To do this, first ensure that all Gazebo models are predefined in the `models/` directory. After that, modify the ModelSpawner.cc on line 83 to include the correct file path. To compile the plugin, create a `build/` directory, `cd` into it, and run `cmake ../` followed by `make`. Finally, register the plugin by editing your `~/.bashrc` file and including the line  
```export GAZEBO_PLUGIN_PATH=${GAZEBO_PLUGIN_PATH}:~/model_spawner/build```
