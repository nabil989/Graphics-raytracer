Nabil Chowdhury
Albert Sun

    - Note: Our dragons still take ~20 seconds to run with BVH

Additional Features

- custom_scene located in /custom_scenes

- Procedural texture mapping
    - We implemented procedural texture mapping using a custom noise function
    - The option can be turned on by setting proceduralTextures to true in material.cpp
    - There are also variables to change to select textures to view and to change the noise offset
    - View textures by running any scene with textures preferrably box.ray or texture_box.ray

- CEL shading
    - We implemented CEL shading ontop of Phong. It has six light intensity buckets and also features a rim on lit edges
    - This can be turned on by editing the CELshading variable in material.cpp
    - One problem we have not been able to solve is combining mutliple lights with CEL shading, In our implementation,
      multiple lights result in a gridlike shading pattern, instead of blending the lights together. 
