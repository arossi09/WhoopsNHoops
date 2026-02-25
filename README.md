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
OpenAL
homebrew:
openal-soft and libsnfile

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

- convert collidable objects so they work with AABB
- Add to entityProcess so that it manages spawning entites as well/entity set needRespawn flag on update?
- add bloom to pickups
- fix some of the Layout Issues
- add mountain ring with billboarded trees
- velocity particle lines for speed
- vertex coloring for baked lighting
- point lights?

- sound manager




