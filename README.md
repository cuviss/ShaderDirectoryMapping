# Introduction

A simple plugin to set Virtual Path for UE project Shader.

# HOW TO USE

## Download the plugin , put it in Plugins folder in your project, Enable the plugin in Editor. 

Optional, You can choose to modify .uproject file if you know how.

## Edit - Project Settings - Game - Shader Directory Mapping

by default, "/Project" is relate to "Shaders" folder in the UE Project's folder.
1, Virtual Path: You could add you custom shader Mapping in the setting, Virtual Path must start with "/".
2, Target Path: if Target Path Type is Project Relative default, just write the folder name, such as "Test", "Test/Shaders", Don't start with "/".

##  Restart Unreal Editor 

When you have modified it , you must restart Unreal Editor to make it works.