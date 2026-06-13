#ifndef CAMERA_H
#define CAMERA_H

#include <pathtracer/ray.cuh>
#include <cmath>

class Camera {
public:
    __host__ __device__ Camera() {}

    // Configures the camera matrix based on location, target, FOV, and aspect ratio
    __host__ __device__ void setup(
        Vec3 lookfrom, 
        Vec3 lookat, 
        Vec3 vup, 
        float vfov_degrees, 
        float aspect_ratio
    ) {
        origin = lookfrom;

        // Convert vertical FOV to radians
        float theta = vfov_degrees * M_PI / 180.0f;
        float h = std::tan(theta / 2.0f);
        
        // Calculate the physical size of the virtual viewport viewport
        float viewport_height = 2.0f * h;
        float viewport_width = aspect_ratio * viewport_height;

        // 1. Calculate the orthonormal basis vectors for the camera
        // 'w' is our look direction (pointing backwards from the target)
        w = (lookfrom - lookat).make_unit_vector();
        // 'right_axis' is perpendicular to the look vector and world up
        right_axis = vup.cross(w).make_unit_vector();
        // 'up_axis' is perpendicular to right and look vectors
        up_axis = w.cross(right_axis);

        // 2. Define the viewport vectors spanning across the image plane
        horizontal = right_axis * viewport_width;
        vertical = up_axis * viewport_height;

        // 3. Calculate the absolute 3D position of the lower-left pixel corner
        // LowerLeft = Origin - (Horizontal / 2) - (Vertical / 2) - w
        lower_left_corner = origin - (horizontal * 0.5f) - (vertical * 0.5f) - w;
    }

    // Convert normalized [0,1] screen coords to a scene ray
    __device__ Ray get_ray(float u, float v) const { 
        // Find the specific coordinate on our virtual 3D viewport plane
        Vec3 target_point = lower_left_corner + (horizontal * u) + (vertical * v);
        
        // Ray direction = Target Point on Viewport - Ray Origin
        return Ray(origin, target_point - origin); 
    }

    __host__ void dumpState(){
        /*
        Right handed coordinate system:
            Positive X to the left
            Positive Y upwards
            Positive Z into the screen
        */
        printf("origin=(%.3f, %.3f, %.3f) \n", origin.x, origin.y, origin.z);
        printf("lower_left_corner=(%.3f, %.3f, %.3f) \n", lower_left_corner.x, lower_left_corner.y, lower_left_corner.z);
        printf("horizontal=(%.3f, %.3f, %.3f) \n", horizontal.x, horizontal.y, horizontal.z);
        printf("vertical=(%.3f, %.3f, %.3f) \n", vertical.x, vertical.y, vertical.z);
    }

private:
    Vec3 origin;
    Vec3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;

    // Orthonormal basis vectors
    Vec3 right_axis;
    Vec3 up_axis;
    Vec3 w;

    Vec3 garbo;
};

#endif