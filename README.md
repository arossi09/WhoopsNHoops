# WhoopSim

Plug in controller/drone transmitter to be able to move around as drone.
(Ive only tested bluetooth with an xbox controller & plugging in a radiomaster 
pocket)

Dependancies:
GLFW
freetype
glm
glew
OpenGL

To run:
mkdir build
cd build
cmake ..
make
./WhoopsnHoops


# TODO

fix hud logic so more module
fix skybox rendering so its able to render after all draws for preformance
add hud element behind drone model 
add fog attentuation from height (maybe with noise factor)
