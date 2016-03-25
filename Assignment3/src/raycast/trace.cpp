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
extern float light1_shadow;

// global ambient term
extern vec3 global_ambient;

// light decay parameters
extern float decay_a;
extern float decay_b;
extern float decay_c;

extern int shadow_on;
extern int reflect_on;
extern int step_max;

float max_float(float a, float b) {
	if(a<=b) return b;
	return a;
}
//compute the shadow
vec3 get_shadow(vec3 eye, vec3 ray, vec3 surf_norm, Spheres *sph) {
	vec3 raynormal = normalize(ray);
	float dist = dot(ray, ray);
	vec3 color=vec3(0,0,0);
	color += global_ambient * sph->mat_ambient;
	//vec3 ambient = light1_intensity * sph->mat_ambient;
	vec3 diffuse = dot(raynormal, surf_norm)*light1_intensity*sph->mat_diffuse;

	vec3 reflect_vector = 2*dot(raynormal, surf_norm)*surf_norm - raynormal;
	//printf("reflect vector:%f %f %f\n", reflect_vector.x, reflect_vector.y, reflect_vector.z);
	//printf("dot product1:%f\n", dot(-1*raynormal, surf_norm));
	//printf("dot product2:%f\n", dot(reflect_vector, surf_norm));
	vec3 specular = max_float(pow(max_float(dot(reflect_vector, eye),0),sph->mat_shineness),0)*light1_intensity * sph->mat_specular;
	//printf("specular color:%f %f %f\n", specular.x, specular.y, specular.z);
	
	color += 1.0 * light1_shadow *(diffuse+specular)/(decay_c*dist+decay_b*sqrt(dist)+decay_a);
	//printf("phong color:%f,%f,%f\n",color.x, color.y, color.z);
	return color;

}
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
	//vec3 ambient = light1_intensity * sph->mat_ambient;
	vec3 diffuse = dot(raynormal, surf_norm)*light1_intensity*sph->mat_diffuse;

	vec3 reflect_vector = 2*dot(raynormal, surf_norm)*surf_norm - raynormal;
	//printf("reflect vector:%f %f %f\n", reflect_vector.x, reflect_vector.y, reflect_vector.z);
	//printf("dot product1:%f\n", dot(-1*raynormal, surf_norm));
	//printf("dot product2:%f\n", dot(reflect_vector, surf_norm));
	vec3 specular = max_float(pow(max_float(dot(reflect_vector, eye),0),sph->mat_shineness),0)*light1_intensity * sph->mat_specular;
	//printf("specular color:%f %f %f\n", specular.x, specular.y, specular.z);
	
	color += 1.0 * (diffuse+specular)/(decay_c*dist+decay_b*sqrt(dist)+decay_a);
	//printf("phong color:%f,%f,%f\n",color.x, color.y, color.z);
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
	//printf("step_max and num:%d %d\n", step_max, num);
	if(num>step_max) return null_clr;
	//printf("entering recursive ray tracing\n");
	vec3 hit;
	Spheres *sph = intersect_scene(eye, ray, scene, &hit);
	vec3 raynormal = normalize(ray);
	//printf("intersect point: %f, %f, %f\n", hit.x, hit.y, hit.z);
	if(sph==NULL) {
		if (num>0) return null_clr;
		//printf("no intersect\n");
		return background_clr;
	}
	//printf("********\n");
	//printf("eye intersection:%u\n", sph);
	//printf("intersection point:%f %f %f\n", hit.x, hit.y, hit.z);

	vec3 lightvec = light1 - hit;
	vec3 lightvec_normal = normalize(lightvec);
	vec3 lighthit;
	Spheres * light_sph = intersect_scene(hit, lightvec, scene, &lighthit);
	// the intersection point is not blocked by othe objects
	//printf("light intersection:%u\n", light_sph);
	//printf("intersection point:%f %f %f\n", lighthit.x, lighthit.y, lighthit.z);
	
	vec3 color = null_clr;
	vec3 surf_normal = sphere_normal(hit, sph);
	//if (light_sph == NULL) {
	//printf("not blocked");
	if(shadow_on && light_sph!=NULL) {
		color += get_shadow(normalize(-1*ray), lightvec, surf_normal, sph);
	}
	else {
		color +=  phong(normalize(-1*ray), lightvec, surf_normal, sph);
	}

	if(reflect_on) {
		vec3 reflect_vector = 2*dot(normalize(-1*ray), surf_normal)*surf_normal + normalize(ray);
		//reflect_vector = normalize(reflect_vector);
		color += sph->reflectance * recursive_ray_trace(hit, reflect_vector,num+1);
	}
	
	//}
//	if(shadow_on) { // it is blocked by other objects
		//printf("blocked");
//		color+= get_shadow(light1, lightvec, surf_normal, sph);
//	}
//	else { 
//		color += global_ambient*sph->mat_ambient;
//	}

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
      //
      // You need to change this!!!
      //
      ret_color = recursive_ray_trace(eye_pos, ray, 0);
      //if(ret_color.x!=background_clr.x || ret_color.y!=background_clr.y || ret_color.z!=background_clr.z) {
//	      printf("x:%d, y:%d\n",i, j);
//	      printf("returned colour:%f %f %f\n", ret_color.x, ret_color.y, ret_color.z);
 //     }
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
      //printf("frame: %f %f %f\n", frame[i][j][0], frame[i][j][1], frame[i][j][2]);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
