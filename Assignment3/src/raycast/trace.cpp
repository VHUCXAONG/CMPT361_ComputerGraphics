#include <stdio.h>
#include <math.h>
#include "global.h"
#include "sphere.h"
//#include "object.h"

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

//extern Object *scene;
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
extern int board_on;
extern int refract_on;
extern int antialias_on;

extern int step_max;
extern struct plane pl;

//float precision = 0.00001;
float max_float(float a, float b) {
	if(a<=b) return b;
	return a;
}
/////////////////////////////////////////////////////////////////////

/*********************************************************************
 * Phong illumination - you need to implement this!
 *********************************************************************/
vec3 phong(vec3 eye, vec3 ray, vec3 surf_norm, void *sph, vec3 hit, int isplane) {
//
// do your thing here
//
	vec3 color=vec3(0,0,0);
	vec3 raynormal = normalize(ray);
	float dist = dot(ray, ray);
	vec3 diffuse;
	vec3 specular;
	vec3 reflect_vector = 2*dot(raynormal, surf_norm)*surf_norm - raynormal;

	if(isplane==1) {
		struct plane* pln= (struct plane*)sph;
		color += global_ambient * pln->mat_ambient;
		if((int(hit.x)+int(hit.z))%2==0) diffuse = max_float(dot(raynormal,surf_norm),0)*light1_intensity*pln->mat_diffuse1;
		else diffuse = max_float(dot(raynormal,surf_norm),0)*light1_intensity*pln->mat_diffuse2;
		
		specular = pow(max_float(dot(reflect_vector,eye),0), pln->shineness)*light1_intensity*pln->mat_specular;
	}
	else {
		Spheres *sphr = (Spheres *)sph;
		color += global_ambient * sphr->mat_ambient;
		//printf("ambient: %f %f %f\n", color.x, color.y, color.z);
		//printf("mat_diffuse: %f %f %f\n", sph->mat_diffuse.x,sph->mat_diffuse.y, sph->mat_diffuse.z);
		diffuse = max_float(dot(raynormal, surf_norm),0)*light1_intensity*sphr->mat_diffuse;
		//printf("diffuse: %f %f %f\n", diffuse.x, diffuse.y, diffuse.z);
		specular = pow(max_float(dot(reflect_vector, eye),0),sphr->mat_shineness)*light1_intensity * sphr->mat_specular;
		//printf("specular: %f %f %f\n", specular.x, specular.y, specular.z);
	}

	color += 1.0 * (diffuse+specular)/(decay_c*dist+decay_b*sqrt(dist)+decay_a);
	return color;
}
//compute the shadow
vec3 get_shadow(vec3 eye, vec3 ray, vec3 surf_norm, void *sph, vec3 hit, int isplane) {

	vec3 raynormal = normalize(ray);
	float dist = dot(ray, ray);
	vec3 color=vec3(0,0,0);
	vec3 reflect_vector = 2*dot(raynormal, surf_norm)*surf_norm - raynormal;
	vec3 diffuse;
	vec3 specular;

	if(isplane==1) {
		struct plane *pln = (struct plane *)sph;
		if((int(hit.x) + int(hit.z))%2==0) diffuse=max_float(dot(raynormal,surf_norm),0)*light1_intensity*pln->mat_diffuse1;
		else  diffuse=max_float(dot(raynormal,surf_norm),0)*light1_intensity*pln->mat_diffuse2;
		specular = pow(max_float(dot(reflect_vector,eye),0), pln->shineness)*light1_intensity*pln->mat_specular;
	}
	else {
		Spheres *sphr= (Spheres *)sph;
		color += global_ambient * sphr->mat_ambient;
		diffuse = max_float(dot(raynormal, surf_norm),0)*light1_intensity*sphr->mat_diffuse;
		specular = pow(max_float(dot(reflect_vector, eye),0),sphr->mat_shineness)*light1_intensity * sphr->mat_specular;
	}
	
	color += 1.0 * light1_shadow *(diffuse+specular)/(decay_c*dist+decay_b*sqrt(dist)+decay_a);
	return color;

}
//normal and inlight are both outward and unit vector
bool refract_light(vec3 normal, vec3 inlight, float ratio, vec3* outlight) {
	float root = 1 - pow(ratio, 2)*(1-pow(dot(normal, inlight),2)); 
	if(root<precision) {
//		printf("no root\n");
		return false; //no root
	}
	*outlight =  (ratio*dot(normal,inlight) - sqrt(root)) * normal - ratio*inlight;
	return true;
}
// computation of the input point/dir and output point/dir of the refractor light
bool refraction(vec3 inpoint, vec3 invector, void* obj, int isplane, vec3* outlightpoint, vec3* outlightvector) {
//	printf("*****");
	if(isplane) return false;
//	printf("not plane\n");
	Spheres * sph = (Spheres *)obj;
	//intermediate light
	vec3 midlight;
	vec3 in_normal = sphere_normal(inpoint, sph);
	float ratio = 1.0/sph->refractivity;
	if(!refract_light(in_normal, invector, ratio, &midlight)) {
//		printf("first refraction fail\n");
		return false; 
	}
	
	vec3 outpoint;
	int ispln;
	float intersect_t = intersect_sphere(inpoint, midlight, sph, &outpoint);
	if(intersect_t+1.0<precision)  {
//		printf("intersect with no object\n");
		return false;
	}
	ratio = 1.0/ratio;
	vec3 out_normal = -1 * sphere_normal(outpoint, sph);
	midlight = -1*midlight;
	vec3 outbeam;
	if(!refract_light(out_normal, midlight, ratio, &outbeam)) {
//		printf("second refraction fail\n");
		return false;
	}
	*outlightpoint = outpoint;
	*outlightvector = outbeam;
//	printf("good refraction\n");
	return true;
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
	int isplane;
	void *sph = intersect_scene(eye, ray, scene, &hit, &isplane);

	vec3 color = null_clr;
	if(sph==NULL) { 
		return background_clr;
	}

	vec3 lightvec = light1 - hit;
	vec3 lightvec_normal = normalize(lightvec);
	vec3 lighthit;
	int lightisplane;
	void * light_sph = intersect_scene(hit, lightvec_normal, scene, &lighthit, &lightisplane);
	
	vec3 surf_normal = isplane?vec3(0,1,0):sphere_normal(hit, (Spheres*)sph);

	if(light_sph==NULL) {
		color += phong(-1*ray, lightvec, surf_normal, sph, hit, isplane);
	}
	else {
		if(!shadow_on) {
			color += phong(-1*ray, lightvec, surf_normal, sph, hit, isplane);
		}
		else {
			color += get_shadow(-1*ray, lightvec, surf_normal, sph, hit, isplane);
		}
	}

	if(reflect_on) {
		vec3 reflect_vector = 2*dot(-1*ray, surf_normal)*surf_normal + ray;
		reflect_vector = normalize(reflect_vector);
		if(isplane) {
			color += ((struct plane*)sph)->reflectance * recursive_ray_trace(hit, reflect_vector,num+1);
		}
		else if(!isplane)
			color += ((Spheres *)sph)->reflectance * recursive_ray_trace(hit, reflect_vector, num+1);
	}
	
	if(refract_on) {
//		printf("refraction is on\n");
		vec3 outlightpoint, outlightvector; 
		if(refraction(hit, -1*ray, sph, isplane, &outlightpoint, &outlightvector)) {
			if(!isplane) {
//				printf("refraction point\n");
				Spheres * refractsph = (Spheres *)sph;
				color += refractsph->refr*recursive_ray_trace(outlightpoint, outlightvector, num+1);
			}
		}
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
      //printf("cur_pixel_pos:%f %f %f\n",cur_pixel_pos.x, cur_pixel_pos.y, cur_pixel_pos.z);
      ret_color = vec3(0,0,0);
      ray = cur_pixel_pos - eye_pos;
      ray = normalize(ray);
      //
      // You need to change this!!!
      //
      ret_color += recursive_ray_trace(eye_pos, ray, 0);

      if(antialias_on) {
	      vec3 pixel;
	      pixel.z = cur_pixel_pos.z;

	      // pixel 1
	      pixel.x = cur_pixel_pos.x - 0.25*x_grid_size;
	      pixel.y = cur_pixel_pos.y + 0.25*y_grid_size;
	      ray = normalize(pixel - eye_pos);
	      ret_color += recursive_ray_trace(eye_pos, ray, 0);

	      // pixel 2
	      pixel.x = cur_pixel_pos.x - 0.25*x_grid_size;
	      pixel.y = cur_pixel_pos.y - 0.25*y_grid_size;
	      ray = normalize(pixel - eye_pos);
	      ret_color += recursive_ray_trace(eye_pos, ray, 0);

	      // pixel 3
	      pixel.x = cur_pixel_pos.x + 0.25*x_grid_size;
	      pixel.y = cur_pixel_pos.y + 0.25*y_grid_size;
	      ray = normalize(pixel - eye_pos);
	      ret_color += recursive_ray_trace(eye_pos, ray, 0);

	      // pixel 4
	      pixel.x = cur_pixel_pos.x + 0.25*x_grid_size;
	      pixel.y = cur_pixel_pos.y - 0.25*y_grid_size;
	      ray = normalize(pixel - eye_pos);
	      ret_color += recursive_ray_trace(eye_pos, ray, 0);

	      ret_color = 0.2*ret_color;
      }

      frame[i][j][0] = GLfloat(ret_color.x);
      frame[i][j][1] = GLfloat(ret_color.y);
      frame[i][j][2] = GLfloat(ret_color.z);

      cur_pixel_pos.x += x_grid_size;
    }

    cur_pixel_pos.y += y_grid_size;
    cur_pixel_pos.x = x_start;
  }
}
