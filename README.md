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


https://www.richardlord.net/blog/ecs/what-is-an-entity-framework
https://docs.spacestation14.com/en/robust-toolbox/ecs.html
https://docs.spacestation14.com/en/ss14-by-example/adding-a-simple-bikehorn.html

fix architecture alittle
- make a renderable class that the scene extends and follow this guide ^

fix hud logic so more module
fix skybox rendering so its able to render after all draws for preformance
add hud element behind drone model 
add fog attentuation from height (maybe with noise factor)
velocity particle lines for speed
vertex coloring for baked lighting


