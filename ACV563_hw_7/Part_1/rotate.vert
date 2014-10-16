varying vec3 normal;
void main()
{
  gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
 
  normal = gl_NormalMatrix * gl_Normal;
 
}