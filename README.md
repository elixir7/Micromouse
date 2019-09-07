# Jerry the micromouse
<img src="images/render.png" width="80%" />

# Who is Jerry?
Jerry is a tiny (10x8 cm) high performance robot for the robotics competition Micromouse. 

The competition goal is to search a 16x16 grid maze which is 3x3 m^2 as fast as possible and find the path to the center.

Then you run from start to goal with your calculated route. 

To get the competition you take 1/30 of the search time and the run time.

World class mice are extremly fast, [have a look.](https://www.youtube.com/watch?v=NqdZ9wbXt8k&t=150s)

# Structure & Development
Hardware:
-   <b>3D_Models</b>,       pcb components, motor mount, rims etc.
-   <b>drawings</b>,        .dxf files
-   <b>pcb</b>,             EAGLE CAD files
-   <b>gerbers.zip</b>,     manufacturing files for pcb (Jerry was made by [JLC PCB](https://www.jlcpcb.com)
-   <b>jlcpcb.cam</b>,      CAM job for creating gerber files to JLC standard in EAGLE.

Software:
-   <b>Eagle</b>,           E-CAD for making PCB
-   <b>Fusion 360</b>,      M-CAD for making board outline, motor mounts, rims, etc.
-   <b>Keil uVision 5</b>,  IDE for C programming/debugging on Microcontrollers

Images:
-   Renders of jerry

## Software
- MCU configuration (pins, clock, etc) using STM32CubeMX
- Keil uVision 5 project
    - Low level drivers (motors, sensors, screen)
    - High level maze solving algoritm

## Contributors
-   Isak Åslund, aslundisak@gmail.com