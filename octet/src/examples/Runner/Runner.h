////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
namespace octet {
  /// Scene containing a box with octet.
  class Runner : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;
	bool freeCamera;

	vec3 cameraPos;
	quat cameraRot;

  public:
    /// this is called when we construct the class before everything is initialised.
    Runner(int argc, char **argv) : app(argc, argv) {
    }
	// x = right
	// y = Up
	// z = straight
    /// this is called once OpenGL is initialized
    void app_init() {
	  freeCamera = true;
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  cameraPos = app_scene->get_camera_instance(0)->get_node()->get_position();
	  cameraRot = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent().toQuaternion();
	  app_scene->get_camera_instance(0)->set_far_plane(1000.0f);


      material *red = new material(vec4(1, 0, 0, 1));


      mesh_box *box = new mesh_box(vec3(5,0.1f,100000));



      scene_node *node = new scene_node();
      app_scene->add_child(node);
      app_scene->add_mesh_instance(new mesh_instance(node, box, red));
	  node->translate(vec3(0,-10,0));
    }


	void cameraFollowMouse()
	{
		float sensitivity = 100.0f / 360.0f;
		int xPos, yPos;
		get_absolute_mouse_movement(xPos, yPos);
		set_mouse_pos(0, 0);

		mat4t &camera_to_world = app_scene->get_camera_instance(0)->get_node()->access_nodeToParent();

		camera_to_world.x() = vec4(1, 0, 0, 0);
		camera_to_world.y() = vec4(0, 1, 0, 0);
		camera_to_world.z() = vec4(0, 0, 1, 0);

		float angle_x = (float)(app_scene->get_camera_instance(0)->get_node()->get_position().x()-xPos) * sensitivity;
		float angle_y = (float)(app_scene->get_camera_instance(0)->get_node()->get_position().y() -yPos) * sensitivity;
		angle_y = std::max(-90.0f, std::min(angle_y, 90.0f));
		camera_to_world.rotateY(angle_x);
		camera_to_world.rotateX(angle_y);
	}

	void updateCamera()
	{
		if (freeCamera)
		{
			cameraFollowMouse();
		}
	}


    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	  
      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);
	  updateCamera();

      // draw the scene
      app_scene->render((float)vx / vy);

      // tumble the box  (there is only one mesh instance)
      scene_node *node = app_scene->get_mesh_instance(0)->get_node();
    }
  };
}
