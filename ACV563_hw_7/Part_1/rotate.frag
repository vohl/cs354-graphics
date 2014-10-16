varying vec3 normal;
 
vec4 light0 ()
{
  vec4 color;
 
  vec3 lightDir = vec3(gl_LightSource[0].position); 
 
  vec4 ambient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
 
  vec4 diffuse = gl_LightSource[0].diffuse * max(dot(normal,lightDir),0.0) * gl_FrontMaterial.diffuse;
 
  color = ambient + diffuse;
 
  return color;
}
 
vec4 light1 ()
{
  vec4 color;
 
  vec3 lightDir = vec3(gl_LightSource[1].position); 
 
  vec4 ambient = gl_LightSource[1].ambient * gl_FrontMaterial.ambient;
 
  vec4 diffuse = gl_LightSource[1].diffuse * max(dot(normal,lightDir),0.0) * gl_FrontMaterial.diffuse;
 
  color = ambient + diffuse;
 
  return color;
}
 
vec4 light2 ()
{
  vec4 color;
 
  vec3 lightDir = vec3(gl_LightSource[2].position); 
 
  vec4 ambient = gl_LightSource[2].ambient * gl_FrontMaterial.ambient;
 
  vec4 diffuse = gl_LightSource[2].diffuse * max(dot(normal,lightDir),0.0) * gl_FrontMaterial.diffuse;
 
  color = ambient + diffuse;
 
  return color;
}
 
vec4 light3 ()
{
  vec4 color;
 
  vec3 lightDir = vec3(gl_LightSource[3].position); 
 
  vec4 ambient = gl_LightSource[3].ambient * gl_FrontMaterial.ambient;
 
  vec4 diffuse = gl_LightSource[3].diffuse * max(dot(normal,lightDir),0.0) * gl_FrontMaterial.diffuse;
 
  color = ambient + diffuse;
 
  return color;
} 
 
void main()
{ 
  vec4 light;
 
  if(vec3(gl_LightSource[0].position) != vec3(0.0, 0.0, 0.0))
    light += light0();
  if(vec3(gl_LightSource[1].position) != vec3(0.0, 0.0, 0.0))
    light += light1();
  if(vec3(gl_LightSource[2].position) != vec3(0.0, 0.0, 0.0))
    light += light2();
  if(vec3(gl_LightSource[3].position) != vec3(0.0, 0.0, 0.0))
    light += light3();
 
  gl_FragColor = light;
}