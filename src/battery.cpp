#include "Renderable.h"
#include "IRenderable.h"
#include "IMoveable.h"
#include "Moveable.h"
#include <iostream>

class Battery : public Renderable{
public:
 	IRenderable renderData; 
	//IMoveable moveData;
	Battery(){
		renderData = new Renderable();
		renderData.position = new PositionComponent();
		renderData.display = new DisplayComponent();
	}
	//instances of each class
	// TODO add attributes
  // object
  // position
  // rotation
  // texture
  // etc
  void render() override {
    // TODO render
  }
};
