########## There is no makefile of this project, for that a makefile is hard to be linked in this involuted environment with too many head files. ##########

########## Use Visual Studio 2019 to open the project "openglwarm.sln" ###########

1. Visual Studio 2019 should have downloaded the workloads "Desktop Development with C++"

2. A folder "GLFW" is contained in the project "openglwarm". Inside the folder is a "libraries" folder which contains 3 subfolders: glm-master, Includes, Libs.

   Grant the necessary permissions when prompted.

3. Adding Include and Libs: Open the project, go to 

   Project Properties -> Configuration Properties -> VC++ Directories -> Include Directories.

   Edit the path. Modify the path of "Includes" folder to the current path in "GLFW//libraries".

   Go to Configuration Properties -> VC++ Directories -> General -> Library Directories.

   Edit the path. Modify the path of "Libs" folder to the current path in "GLFW//libraries".

4. Adding glm-master:

   Go to Project Properties -> Configuration Properties -> C/C++ -> General -> Additional Include Directories.

   Edit the path. Modify the path of "glm-master" to the current path in "GLFW//libraries".

5. Copy the "glew32.dll" file in the project into the fole "C:\Windows\System32".


6. Open the project "openglwarm.sln". Click the "Local Windows Debugger" to test the project.



########## Test the project ##########


1. To test a new obj file --
   view.cpp -> line 58
   Replace the content of char map[] with the path of this new obj file, like "data\\cube.obj".

2. To test a new shaders file --
   view.cpp -> line 92 and line 93
   Replace the content of "vertexPath" and "fragmentPath" with the new shaders files' paths.

3. Functionality keyboard control --
   program the project -> for the "control", input "1" and enter -> for the "flag", input "1", "2", or "3" and enter

4. Functionality mouse control --
   program the project -> for the "control", input "2" and enter -> for the "flag", input "1", "2", or "3" and enter
   
5. Functionality Gouraud shading --
   view.cpp -> line 92 and line 93
   Replace the content of "vertexPath" and "fragmentPath" with "source_gouraud.vs" and "source_gouraud.fs".
   program the project -> for the "control", input "0", "1" or "2" and enter -> for the "flag", input "2" or "3" and enter

6. Functionality Phong shading --
   view.cpp -> line 92 and line 93
   Replace the content of "vertexPath" and "fragmentPath" with "source_phong.vs" and "source_phong.fs".
   program the project -> for the "control", input "0", "1" or "2" and enter -> for the "flag", input "2" or "3" and enter

7. Functionality Z-buffer --
   view.cpp -> line 58
   Replace the content of char map[] with the path "separate triangles.obj".
   view.cpp -> line 92 and line 93
   Replace the content of "vertexPath" and "fragmentPath" with "source.vs" and "source_ztest.fs".
   program the project -> for the "control", input "0", "1" or "2" and enter -> for the "flag", input "2" or "3" and enter
	
8. If made any changes to the project, please recompile the project.

