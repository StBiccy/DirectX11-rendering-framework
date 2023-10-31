# real-time-rendering-framework-artefact-StBiccy
real-time-rendering-framework-artefact-StBiccy created by GitHub Classroom

Great progress so far, normal mapping is causing you isses as it's unfortunately a bit more complex that an is currently implemented

### Rendering
Great use of OBJ and hardcoded models to showcase proficiency with 3D
Would like to see more models added to the scene to show different complex models rendered

### Lighting
Lighting all looks good and performs calculations correctly
Combines effectivel with texture color and specular map 
Normal mapping is not correct though

Currently you use
```
    if(hasNormMap == 1)
    {
        input.normal = normTex.Sample(bilinearSampler, input.texCoord);
        normalW = normalize(input.normal);
    }
```
This would sample the same normal direction for each face
As the texture contains a Vector3 pointing generally down the +z axis
This mean every face of your cube has a 'normal' in the same direction

Actual normal mapping makes use of tangent and bitangent space
A good resource to start with this is LearnOpenGL who have a decent breakdown of the requirements
https://learnopengl.com/Advanced-Lighting/Normal-Mapping


