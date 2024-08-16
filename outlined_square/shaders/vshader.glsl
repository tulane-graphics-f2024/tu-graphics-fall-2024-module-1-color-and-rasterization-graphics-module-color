#version 150

in vec4 vPos;
in vec4 vColor;
out vec4 color;

void
main()
{
  gl_Position = vPos;
  color = vColor;
}
