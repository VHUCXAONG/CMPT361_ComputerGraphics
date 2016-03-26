#include <stdio.h>
#include <math.h>
#include "global.h"
//#include "sphere.h"
#include "object.h"

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

extern Object *scene;

// light 1 position and color
extern vec3 light1;
extern vec3 light1_intensity;
extern float light1_shadow;

// global ambient term
extern vec3 global_ambient;

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int reflect_on;
extern int board_on;
extern int step_max;

float max_float(float a, float b) {
	if(a<=b) return b;
	return a;
}
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
vec3 phong(vec3 eye, vec3 ray, vec3 surf_norm, Object *sph, vec3 hit) {
//
// do your thing here
//
	vec3 color=vec3(0,0,0);
	if(sph->type=='p') {
		if((int(hit.x)+int(hit.z))%2==0) return vec3(0,0,0);
		else return vec3(1,1,1);
	}
	else {
		vec3 raynormal = normalize(ray);
		float dist = dot(ray, ray);
		color += global_ambient * sph->mat_ambient;

		vec3 diffuse = max_float(dot(raynormal, surf_norm),0)*light1_intensity*sph->mat_diffuse;

		vec3 reflect_vector = 2*dot(raynormal, surf_norm)*surf_norm - raynormal;

		vec3 specular = pow(max_float(dot(reflect_vector, eye),0),sph->mat_shineness)*light1_intensity * sph->mat_specular;
	
		color += 1.0 * (diffuse+specular)/(decay_c*dist+decay_b*sqrt(dist)+decay_a);
	}
	return color;
}
//compute the shadow
vec3 get_shadow(vec3 eye, vec3 ray, vec3 surf_norm, Object *sph, vec3 hit) {
	if(sph->type=='p') return vec3(0,0,0);

	vec3 raynormal = normalize(ray);
	float dist = dot(ray, ray);
	vec3 color=vec3(0,0,0);
	color += global_ambient * sph->mat_ambient;

	vec3 diffuse = max_float(dot(raynormal, surf_norm),0)*light1_intensity*sph->mat_diffuse;

	vec3 reflect_vector = 2*dot(raynormal, surf_norm)*surf_norm - raynormal;

	vec3 specular = pow(max_float(dot(reflect_vector, eye),0),sph->mat_shineness)*light1_intensity * sph->mat_specular;
	
	color += 1.0 * light1_shadow *(diffuse+specular)/(decay_c*dist+decay_b*sqrt(dist)+decay_a);
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
	if(num>step_max) return null_clr;
	vec3 hit;
	Object *sph = intersect_scene(eye, ray, scene, &hit);

	vec3 color = null_clr;
	if(sph==NULL) 
		return background_clr;

	vec3 lightvec = light1 - hit;
	vec3 lightvec_normal = normalize(lightvec);
	vec3 lighthit;
	Object * light_sph = intersect_scene(hit, lightvec_normal, scene, &lighthit);
	
	vec3 surf_normal = sph->get_surfnormal(hit);

	if(shadow_on && light_sph!=NULL) {
		color += get_shadow(-1*ray, lightvec, surf_normal, sph, hit);
	}
	else {
		color +=  phong(-1*ray, lightvec, surf_normal, sph, hit);
	}

	if(reflect_on) {
		vec3 reflect_vector = 2*dot(-1*ray, surf_normal)*surf_normal + ray;
		reflect_vector = normalize(reflect_vector);
		color += sph->reflectance * recursive_ray_trace(hit, reflect_vector,num+1);
	}

	return color;

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
  //printf("window height:%d\n",win_height);
  //printf("window width:%d\n",win_width);
  //printf("%d\n",WIN_HEIGHT);
  //printf("%d\n",WIN_WIDTH);

  for (i=0; i<win_height; i++) {
    for (j=0; j<win_width; j++) {
      ray = cur_pixel_pos - eye_pos;
      ray = normalize(ray);
      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(eye_pos, ray, 0);

      frame[i][j][0] = GLfloat(ret_color.x);
      frame[i][j][1] = GLfloat(ret_color.y);
      frame[i][j][2] = GLfloat(ret_color.z);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
