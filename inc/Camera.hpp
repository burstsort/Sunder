#ifndef CAMERA
#define CAMERA

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

#define MIN_DIST 1.0f
#define MAX_DIST 12.0f

#define FAR_CLIP 1.0f
#define NEAR_CLIP 0.00001f

#define DEFAULT_FOV (M_PI * 0.25f)

class Camera
{
	public:
		Camera();
		~Camera();

		void ToggleMode();

		void ChangeTarget(vec3 target);

		void MoveForward(float amount);
		void MoveRight(float amount);

		void PitchUp(float amount);
		void YawRight(float amount);

		void Update();

		void GetViewMatrix(mat4x4 view);
		void GetProjectionMatrix(mat4x4 proj);

		void UpdateViewProjectionMatrices(GLint program);

		void GetPosition(vec3 pos);
		void GetForward(vec3 fwd);
		void GetUp(vec3 up);

		void CastRay(float x, float y, vec3 dir);

	private:
		float _fov;
		float _aspect;

		bool _orbit;

		vec3 _position;
		vec3 _target;
		vec3 _up;

		float _distance;
		float _angleH;
		float _angleV;

		mat4x4 _view;
		mat4x4 _proj;
};

#endif //CAMERA