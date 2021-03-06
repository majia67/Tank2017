#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec2 vTexc[];

out vec2 fTexc;

void main()
{
	vec4 p1 = gl_in[0].gl_Position;
	vec4 p2 = gl_in[1].gl_Position;

	gl_Position = p1;
	fTexc = vTexc[0];
	EmitVertex();

	if ((p1.x - p2.x) * (p1.y - p2.y) < 0)
		// Unit direction is up or down
		gl_Position = vec4(p2.x, p1.y, p1.z, 1.0);
	else
		// Unit direction is left or right
		gl_Position = vec4(p1.x, p2.y, p1.z, 1.0);
	fTexc = vec2(vTexc[1].x, vTexc[0].y);
	EmitVertex();
	
	if ((p1.x - p2.x) * (p1.y - p2.y) < 0)
		gl_Position = vec4(p1.x, p2.y, p1.z, 1.0);
	else
		gl_Position = vec4(p2.x, p1.y, p1.z, 1.0);
	fTexc = vec2(vTexc[0].x, vTexc[1].y);
	EmitVertex();

	gl_Position = p2;
	fTexc = vTexc[1];
	EmitVertex();

	EndPrimitive();
}
