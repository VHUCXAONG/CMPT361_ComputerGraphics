#include <stdio.h>
#include <math.h>
#include "global.h"
#include "sphere.h"

//
// Global variables
//
extern int win_width;
extern int win_height;

extern GLfloat frame[WIN_HEIGHT][WIN_WIDTH][3];  

extern float image_width;
extern float image_height;

extern vec3 eye_pos;
extern float image_plane;
extern vec3 background_clr;
extern vec3 null_clr;

extern Spheres *scene;

// light 1 position and color
extern vec3 light1;
extern vec3 light1_intensity;

// global ambient term
extern vec3 global_ambient;

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int step_max;

/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
vec3 phong(vec3 eye, vec3 ray, vec3 surf_norm, Spheres *sph) {
//
// do your thing here
//
	vec3 raynormal = normalize(ray);
	float dist = dot(ray, ray);
	vec3 color=vec3(0,0,0);
	color += global_ambient * sph->mat_ambient;
	color += 1.0 * light1_intensity * sph->mat_ambient/(decay_a*dist+decay_b*sqrt(dist)+decay_c);
	printf("phong color:%f,%f,%f\n",color.x, color.y, color.z);
	return color;
}

/************************************************************************
 * This is the recursive ray tracer - you need to implement this!
 * You should decide what arguments to use.
 ************************************************************************/
vec3 recursive_ray_trace(vec3 eye, vec3 ray, int num) {
//
// do your thing here
//
	printf("entering recursive ray tracing\n");
	vec3 hit;
	Spheres *sph = intersect_scene(eye, ray, scene, &hit);
	printf("intersect point: %f, %f, %f\n", hit.x, hit.y, hit.z);
	if(sph==NULL) {
		printf("no intersect\n");
		return background_clr;
	}
	
	vec3 raydir = hit - eye;
	vec3 surf_normal = sphere_normal(hit,sph);
	return phong(eye, raydir, surf_normal, sph);
	
}

/*********************************************************************
 * This function traverses all the pixels and cast rays. It calls the
 * recursive ray tracer and assign return color to frame
 *
 * You should not need to change it except for the call to the recursive
 * ray tracer. Feel free to change other parts of the function however,
 * if you must.
 *********************************************************************/
void ray_trace() {
  printf("entering the ray tracer\n");
  int i, j;
  float x_grid_size = image_width / float(win_width);
  float y_grid_size = image_height / float(win_height);
  float x_start = -0.5 * image_width;
  float y_start = -0.5 * image_height;
  vec3 ret_color;
  vec3 cur_pixel_pos;
  vec3 ray;

  // ray is cast through center of pixel
  cur_pixel_pos.x = x_start + 0.5 * x_grid_size;
  cur_pixel_pos.y = y_start + 0.5 * y_grid_size;
  cur_pixel_pos.z = image_plane;
  printf("window height:%d\n",win_height);
  printf("window width:%d\n",win_width);
  printf("%d\n",WIN_HEIGHT);
  printf("%d\n",WIN_WIDTH);

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      printf("i:%d\n",i); 
      printf("j:%d\n",j);
      ray = cur_pixel_pos - eye_pos;
      ray = normalize(ray);
      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(eye_pos, ray, 0);
      //ret_color = background_clr; // just background for now

      // Parallel rays can be cast instead using below
      //
      // ray.x = ray.y = 0;
      // ray.z = -1.0;

      // Checkboard for testing
      //vec3 clr = vec3(float(i/32), 0, float(j/32));
      //ret_color = clr;

      frame[i][j][0] = GLfloat(ret_color.x);
      frame[i][j][1] = GLfloat(ret_color.y);
      frame[i][j][2] = GLfloat(ret_color.z);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
