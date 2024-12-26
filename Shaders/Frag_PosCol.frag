#version 430

// pipeline-ból bejövő per-fragment attribútumok
in vec3 vs_out_color;

// kimenő érték - a fragment színe
out vec4 fs_out_col;
uniform float mult;

void main()
{
	fs_out_col = vec4( vs_out_color * mult, 1.0 );
}
