#include"include/vec.h"
class Object {
public:
	int index;

	char type; // is it a sphere or a plane

	vec3 mat_ambient;    // material property used in Phong model
	vec3 mat_diffuse;
	vec3 mat_specular;
	float mat_shineness;

	float reflectance;       // this number [0,1] determines how much 
				   // reflected light contributes to the color
				   // of a pixel
	Object * next;
	
	Object(int id, char t, vec3 amb, vec3 diff, vec3 spec, float shine, float reflec):
	index(id), type(t), mat_ambient(amb), mat_diffuse(diff), mat_specular(spec), mat_shineness(shine), reflectance(reflec), next(NULL){}
	
	virtual ~Object() {
		delete this;
	}
	virtual float intersect(vec3 origin, vec3 dir, vec3* hit) = 0;
	virtual vec3 get_surfnormal(vec3 point) = 0;
	Object* add_Object(Object *o);
	
};

class sphere: public Object {
public:
	vec3 center;
	float radius;
	
	sphere(int id, vec3 amb, vec3 diff, vec3 spec, float shine, float reflec, float r, vec3 c):
	Object(id, 's', amb, diff, spec, shine, reflec), radius(r), center(c) {};
	virtual ~sphere() {
		delete this;
	}
	
	float intersect(vec3 origin, vec3 dir, vec3* hit);
	vec3 get_surfnormal(vec3 point);
	
};

class plane: public Object {
public:
	vec3 lowerbottom;
	vec3 uppertop;
	
	plane(int id, vec3 lb, vec3 ut):
	Object(id, 'p', vec3(0,0,0), vec3(0,0,0), vec3(0,0,0), 10, 0.4), lowerbottom(lb), uppertop(ut) {};
	virtual ~plane() {
		delete this;
	}

	float intersect(vec3 origin, vec3 dir, vec3* hit);
	vec3 get_surfnormal(vec3 point);
	
	
};

Object * intersect_scene(vec3 origin, vec3 dir, Object* scene, vec3* hit) ;


