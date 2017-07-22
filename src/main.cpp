#include <iostream>

#include "GL/gl3w.h"
#include "GLFW/glfw3.h"
#include "linmath.h"

#include "Shader.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include "Knife.hpp"

#define WIDTH 960
#define HEIGHT 540

#define CAMERA_SPEED 4.0f

enum RENDERMODE
{
	WIREFRAME, SOLID, BOTH
};

int main(int argc, char *argv[])
{
	// Initialise GLFW
		if (!glfwInit())
		{
			std::cout << "Error while initializing GLFW";
			return 1;
		}
	
	// Setup window
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Mesh Cutting Example", NULL, NULL);
		glfwMakeContextCurrent(window);
		bool fullscreen = false;
		bool optionChange = false;
		bool slicing = false;

		if (!window)
		{
			glfwTerminate();
			std::cout << "Error while opening a window";
			return 1;
		}

	// Initialise gl3w
		if (gl3wInit())
		{
			std::cout << "Error while initializing gl3w";
			return 1;
		}

		std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	// Load shaders
		Shader solid;
		if (!solid.Load("solid"))
			std::cout << "SOLID SHADER ERROR" << std::endl;
		
		Shader wire;
		if (!wire.Load("wire"))
			std::cout << "WIREFRAME SHADER ERROR" << std::endl;

		Shader slash;
		if (!slash.Load("slash"))
			std::cout << "SLASH SHADER ERROR" << std::endl;

	// Setup scene
		Scene scene;
		mat4x4 T;
		mat4x4_identity(T);
		
		//mat4x4_translate(T, 0.0f, -1.0f, 0.0f);
		//scene.AddCuttableObject("barrel", "barrel", "wood", T);

		mat4x4_translate(T, 0.0f, -3.0f, 3.0f);
		scene.AddObject("scene", "scene", T);

		mat4x4_translate(T, 0.0f, -3.0f, 0.0f);
		scene.AddObject("pedestal", "pedestal", T);
		mat4x4_translate(T, 0.0f, -1.5f, 0.0f);
		scene.AddCuttableObject("triangle", "triangle", "triangle", T);

		mat4x4_translate(T, 0.0f, -3.0f, 8.0f);
		scene.AddObject("pedestal", "pedestal", T);
		mat4x4_translate(T, 0.0f, -1.5f, 8.0f);
		scene.AddCuttableObject("cube", "stone", "stone_dark", T);

		mat4x4_translate(T, 5.0f, -3.0f, 8.0f);
		scene.AddObject("pedestal", "pedestal", T);
		mat4x4_translate(T, 5.0f, -1.5f, 8.0f);
		scene.AddCuttableObject("barrel", "barrel", "wood", T);

		mat4x4_translate(T, -5.0f, -3.0f, 8.0f);
		scene.AddObject("pedestal", "pedestal", T);
		mat4x4_translate(T, -5.0f, -1.5f, 8.0f);
		scene.AddCuttableObject("car", "car", "black", T);

	// Setup camera
		Camera cam;
		cam.YawRight(0.2f);

	// Setup OpenGL parameters
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		glViewport(0, 0, WIDTH, HEIGHT);
		glClearColor(0.1f, 0.1f, 0.125f, 1.0f);

	// Setup application parameters
		double dt = 0;
		double mouseX, mouseY;
		double mouseXprev, mouseYprev;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		RENDERMODE renderMode = SOLID;

		float sliceX = 0.0;
		float sliceY = 0.0;

	// Setup knife
		Knife knife(cam, WIDTH, HEIGHT);

	// Main loop
		while(!glfwWindowShouldClose(window))
		{
			// Clear canvas
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Update camera position
				cam.Update();
			
			// Render solid mesh
				if (renderMode == SOLID || renderMode == BOTH)
				{
					glUseProgram(solid.GetProgram());
					cam.UpdateViewProjectionMatrices(solid.GetProgram());
					scene.RenderAll(solid.GetProgram());
				}

			// Render wireframe mesh
				glDisable(GL_DEPTH_TEST);
				if (renderMode == WIREFRAME || renderMode == BOTH)
				{
					glUseProgram(wire.GetProgram());
					cam.UpdateViewProjectionMatrices(wire.GetProgram());	
					scene.RenderAllWireframe(solid.GetProgram());
				}
				glEnable(GL_DEPTH_TEST);

			// Render knife
				glUseProgram(slash.GetProgram());
				cam.UpdateViewProjectionMatrices(slash.GetProgram());
				knife.Render(slash.GetProgram());
			
			// Unbind VAO and shader
				glBindVertexArray(0);
				glUseProgram(0);

			// Handle mouse input
				mouseXprev = mouseX;
				mouseYprev = mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);

			// End loop on ESC
				if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;

			// Toggle fullscreen on F4 press
				if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS && !optionChange && !slicing)
				{
					optionChange = true;
					fullscreen = !fullscreen;

					if (fullscreen)
					{
						GLFWmonitor* monitor = glfwGetPrimaryMonitor();
						const GLFWvidmode* mode = glfwGetVideoMode(monitor);
						glfwSetWindowMonitor(window, monitor, 0, 0, WIDTH, HEIGHT, mode->refreshRate);
					}
					else
					{
						glfwSetWindowMonitor(window, NULL, WIDTH * 0.5, HEIGHT * 0.5, WIDTH, HEIGHT, 0);
					}
				}
			
			// Switch rendering modes on F3 press
				if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && !optionChange && !slicing)
					{
						optionChange = true;

						switch (renderMode)
						{
							case WIREFRAME: renderMode = SOLID; break;
							case SOLID: renderMode = BOTH; break;
							case BOTH: renderMode = WIREFRAME; break;
						}
					}

			// Toggle camera mode on F press
				if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !optionChange && !slicing)
				{
					cam.ToggleMode();
					optionChange = true;
				}

			// Reset model on R press
				if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !optionChange && !slicing)
				{
					std::cout<<"RESET"<<std::endl;
					scene.ResetAll();
					optionChange = true;
				}

			// Option input release
				if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE)
					if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_RELEASE)
						if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
							if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
								optionChange = false;
			
			// Move camera on WASD
				if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !slicing) cam.MoveRight(CAMERA_SPEED * dt);
				if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !slicing) cam.MoveRight(-CAMERA_SPEED * dt);
				if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !slicing) cam.MoveForward(CAMERA_SPEED * dt);
				if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !slicing) cam.MoveForward(-CAMERA_SPEED * dt);

			// Rotate camera on MOUSE2
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS && !slicing)
				{
					cam.YawRight((mouseXprev - mouseX) * -0.01f);
					cam.PitchUp((mouseYprev - mouseY) * 0.01f);
				}

			// Slash on MOUSE1
				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !slicing)
				{
					slicing = true;
					knife.SetStart(mouseX, mouseY);
					sliceX = mouseX;
					sliceY = mouseY;
				}

				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && slicing)
				{
					vec3 p;
					cam.GetPosition(p);

					vec3 a, b;
					cam.CastRay((2.0f * sliceX) / WIDTH - 1.0f, 1.0f - (2.0f * sliceY) / HEIGHT, a);
					cam.CastRay((2.0f * mouseX) / WIDTH - 1.0f, 1.0f - (2.0f * mouseY) / HEIGHT, b);

					vec3_add(a, a, p);
					vec3_add(b, b, p);

					vec3 fw;
					fw[0] = 1.0f;
					fw[1] = 0.0f;
					fw[2] = 0.0f;

					vec3 dir;
					vec3_sub(dir, b, a);
					vec3_norm(dir, dir);

					float cosTheta = vec3_mul_inner(fw, dir);

					vec3 axis;
					vec3_mul_cross(axis, fw, dir);
					vec3_norm(axis, axis);

					quat r;
					quat_rotate(r, acos(cosTheta), axis);
					quat_norm(r, r);

					mat4x4 R;
					mat4x4_from_quat(R, r);

					mat4x4 _transform;
					mat4x4_translate(_transform, a[0], a[1], a[2]);
					mat4x4_mul(_transform, _transform, R);

					vec3 d;
					vec3_sub(d, a, b);
					float s = vec3_len(d);
					mat4x4_scale_aniso(_transform, _transform, s, 1.0f, 1.0f);
					
					knife.SetEnd(mouseX, mouseY);
				}

				if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE && slicing)
				{
					slicing = false;

					vec3 p, n;
					if (knife.Cut(p, n) > 0.01f)
						scene.SliceAll(p, n, 2.0f);
				}

			// Pause physics on SPACE press
				if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
				{
					scene.UpdateAllPhysics(dt);
				}

			// Animate slash
				knife.Animate(dt);

			// Calculate delta time
				dt = glfwGetTime();
				glfwSetTime(0);
				if (dt > 0.2) dt = 0.2;

			// Flip buffers and poll events
				glfwSwapBuffers(window);
				glfwPollEvents();
		}
	
	// Close window
		glfwDestroyWindow(window);
		glfwTerminate();

	return 0;
}