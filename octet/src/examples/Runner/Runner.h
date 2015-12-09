////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//

#include "Julia.h"
namespace octet {
  /// Scene containing a box with octet.
	

  class Runner : public app {

    Julia_shader julia_shader_;

	struct my_vertex {
		vec3p pos;
		vec2p tex;
	};
	class GameObject {
	public:
		GameObject() {}
		GameObject(ref<scene_node> node, btRigidBody* rigidBody, ref<mesh_instance> meshInstance)
		{
			_node = node;
			_rigidBody = rigidBody;
			_meshInstance = meshInstance;
		}

		ref<scene_node> getNode() { return _node; }
		btRigidBody* getRigidBody() { return _rigidBody; }
		ref<mesh_instance> getMeshInstance() { return _meshInstance; }

	private:
		ref<scene_node> _node;
		btRigidBody* _rigidBody;
		ref<mesh_instance> _meshInstance;
	};
    // scene for drawing box
	
    ref<visual_scene> app_scene;
	bool freeCamera;
	GameObject player;
	//GameObject background;
	std::vector<GameObject> listGameObjects;

	int obstacleDrawDistance;
	int obstacleGap;
	int roadWidth;
	int playerSize;
	int backgroundDistance;
	int lastDist;

  public:
    /// this is called when we construct the class before everything is initialised.
    Runner(int argc, char **argv) : app(argc, argv) {
    }
	// x = right
	// y = Up
	// -z = straight
    /// this is called once OpenGL is initialized
    void app_init() {

	  julia_shader_.init();

	  freeCamera = false;
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();
	  app_scene->get_camera_instance(0)->set_far_plane(1000.0f);
	  obstacleDrawDistance = 450;
	  obstacleGap = 50;
	  backgroundDistance = 500;
	  roadWidth = 5;
	  playerSize = 1;
	  lastDist = obstacleDrawDistance;
	  listGameObjects = std::vector<GameObject>();

	  material *red = new material(vec4(1, 0, 0, 1));
	  material *blue = new material(vec4(0, 0, 1, 1));
	  material *purple = new material(vec4(1, 0, 1, 1));


	  mat4t mat;
	  mat.translate(0, -10, 0);
	  listGameObjects.push_back(createGameObject(mat, new mesh_box(vec3(roadWidth, 0.1f, 100000)), red, false, 1.0f));



	  mat.translate(0, 2, 0);
	  player = createGameObject(mat, new mesh_box(vec3(playerSize)), purple, true, 10.0f);

	  
	  for (int i = 0; i * obstacleGap < obstacleDrawDistance; i++)
	  {
		  createObstacle((i+1) * obstacleGap, new mesh_sphere(vec3(0), 1), blue);
	  }



    }


	void createBackground()
	{


		mat4t modelToWorld;
		modelToWorld.loadIdentity();
		modelToWorld.translate(vec3(0, player.getNode()->get_position().y(), player.getNode()->get_position().z() - backgroundDistance));
		modelToWorld.scale(1, 1, 1);
		mat4t cameraToWorld = mat4t().translate(app_scene->get_camera_instance(0)->get_node()->get_position());
		mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		julia_shader_.render(modelToProjection, 100, 100);


		float vertices[] = {
			-500, -500, -200,
			500, -500, -200,
			-500, 500, -200,
			500, 500, -200,
		};


		// attribute_pos (=0) is position of each corner
		// each corner has 3 floats (x, y, z)
		// there is no gap between the 3 floats and hence the stride is 3*sizeof(float)
		glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vertices);
		glEnableVertexAttribArray(attribute_pos);

		// finally, draw the sprite (4 vertices)
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);



	}

	void createObstacle(float distanceFromPlayer, mesh *msh, material *mtl)
	{
		float xCoord = rand() % 5 - 2.5f;
		vec3 relativePos = vec3(xCoord, 0, -distanceFromPlayer);
		mat4t mat;
		mat.translate(0, player.getNode()->get_position().y(),player.getNode()->get_position().z());
		mat.translate(relativePos);
		listGameObjects.push_back(createGameObject(mat, msh, mtl, true, 99999.0f));
	}

	GameObject createGameObject(mat4t_in mat, mesh *msh, material *mtl, bool is_dynamic = false, float mass = 1)
	{
		btRigidBody* rigidBody = NULL;
		scene_node* node = NULL;
		ref<mesh_instance> mi = app_scene->add_shape(mat, msh, mtl, is_dynamic, mass, &rigidBody, &node);
		return GameObject(node, rigidBody, mi);
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
		vec3 cameraLoc = player.getNode()->get_position() - vec3(0, -5, -17);
		app_scene->get_camera_instance(0)->get_node()->translate(-app_scene->get_camera_instance(0)->get_node()->get_position());
		app_scene->get_camera_instance(0)->get_node()->translate(cameraLoc);
	}


	void handleMovement()
	{
		player.getNode()->translate(vec3(0, 0, -3.0f));

		float movement = 0.0f;
		if (is_key_down(key_left))
			movement -= 0.5f;
		if (is_key_down(key_right))
			movement += 0.5f;
		float newX = abs(player.getNode()->get_position().x() + movement);
		if (newX < roadWidth - playerSize)
		{
			player.getNode()->translate(vec3(movement, 0, 0));
		}

		//background.getNode()->translate(-background.getNode()->get_position());
		//background.getNode()->translate(vec3(0, player.getNode()->get_position().y(), player.getNode()->get_position().z() - backgroundDistance));
	}

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);
	  
      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);
	  updateCamera();

	  handleMovement();

	  if (-player.getNode()->get_position().z() + obstacleDrawDistance > lastDist)
	  {
		  createObstacle(-player.getNode()->get_position().z() + obstacleDrawDistance + obstacleGap, new mesh_sphere(vec3(0), 1), new material(vec4(0, 0, 1, 1)));
		  lastDist = -player.getNode()->get_position().z() + obstacleDrawDistance + obstacleGap;
	  }

	  std::cout << "Player Position : (" << player.getNode()->get_position().x() << "," << player.getNode()->get_position().y() << "," << player.getNode()->get_position().z() << ")\n";
//	  std::cout << "BG Position : (" << background.getNode()->get_position().x() << "," << background.getNode()->get_position().y() << "," << background.getNode()->get_position().z() << ")\n";

	  createBackground();


      // draw the scene
      app_scene->render((float)vx / vy);


    }
  };
}
