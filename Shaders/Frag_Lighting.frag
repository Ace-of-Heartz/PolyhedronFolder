#version 430

// pipeline-ból bejövő per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimenő érték - a fragment színe
out vec4 fs_out_col;

// textúra mintavételező objektum
uniform sampler2D texImage;

uniform vec3 cameraPos;

// fenyforras tulajdonsagok
uniform vec4 lightPosDir1 = vec4( 0.0, 1.0, 0.0, 0.0);
uniform vec4 lightPosDir2 = vec4( 0.0, 1.0, 0.0, 0.0);

uniform vec3 La1 = vec3(0.0, 0.0, 0.0 );
uniform vec3 Ld1 = vec3(1.0, 1.0, 1.0 );
uniform vec3 Ls1 = vec3(1.0, 1.0, 1.0 );

uniform vec3 La2 = vec3(0.0, 0.0, 0.0 );
uniform vec3 Ld2 = vec3(1.0, 1.0, 1.0 );
uniform vec3 Ls2 = vec3(1.0, 1.0, 1.0 );

uniform float lightConstantAttenuation    = 1.0;
uniform float lightLinearAttenuation      = 0.0;
uniform float lightQuadraticAttenuation   = 0.0;

// anyag tulajdonsagok

uniform vec3 Ka = vec3( 1.0 );
uniform vec3 Kd = vec3( 1.0 );
uniform vec3 Ks = vec3( 1.0 );

uniform float Shininess = 1.0;

/* segítség:
	    - normalizálás: http://www.opengl.org/sdk/docs/manglsl/xhtml/normalize.xml
	    - skaláris szorzat: http://www.opengl.org/sdk/docs/manglsl/xhtml/dot.xml
	    - clamp: http://www.opengl.org/sdk/docs/manglsl/xhtml/clamp.xml
		- reflect: http://www.opengl.org/sdk/docs/manglsl/xhtml/reflect.xml
				reflect(beérkező_vektor, normálvektor);
		- pow: http://www.opengl.org/sdk/docs/manglsl/xhtml/pow.xml
				pow(alap, kitevő);
*/


vec3 ComputeLightEffects(vec4 lightPosDir,vec3 La,vec3 Ld, vec3 Ls) {
	vec3 normal = normalize( vs_out_norm );
	vec3 ToLight; // A fényforrásBA mutató vektor
	float LightDistance=0.0; // A fényforrástól vett távolság

	if ( lightPosDir.w == 0.0 ) // irány fényforrás (directional light)
	{
		// Irányfényforrás esetén minden pont ugyan abbóla az irányból van megvilágítva
		ToLight	= lightPosDir.xyz;
		// A távolságot 0-n hagyjuk, hogy az attenuáció ne változtassa a fényt
	}
	else				  // pont fényforrás (point light)
	{
		// Pontfényforrás esetén kkiszámoljuk a fragment pontból a fényforrásba mutató vektort, ...
		ToLight	= lightPosDir.xyz - vs_out_pos;
		// ... és a távolságot a fényforrástól
		LightDistance = length(ToLight);
	}
	//  Normalizáljuk a fényforrásba mutató vektort
	ToLight = normalize(ToLight);

	// Attenuáció (fényelhalás) kiszámítása
	float Attenuation = 1.0 / ( lightConstantAttenuation + lightLinearAttenuation * LightDistance + lightQuadraticAttenuation * LightDistance * LightDistance);

	// Ambiens komponens
	// Ambiens fény mindenhol ugyanakkora
	vec3 Ambient = La * Ka;

	// Diffúz komponens
	// A diffúz fényforrásból érkező fény mennyisége arányos a fényforrásba mutató vektor és a normálvektor skaláris szorzatával
	// és az attenuációval
	float DiffuseFactor = max(dot(ToLight,normal), 0.0) * Attenuation;
	vec3 Diffuse = DiffuseFactor * Ld * Kd;

	// Spekuláris komponens
	vec3 viewDir = normalize( cameraPos - vs_out_pos ); // A fragmentből a kamerába mutató vektor
	vec3 reflectDir = reflect( -ToLight, normal ); // Tökéletes visszaverődés vektora

	// A spekuláris komponens a tökéletes visszaverődés iránya és a kamera irányától függ.
	// A koncentráltsága cos()^s alakban számoljuk, ahol s a fényességet meghatározó paraméter.
	// Szintén függ az attenuációtól.
	float SpecularFactor = pow(max( dot( viewDir, reflectDir) ,0.0), Shininess) * Attenuation;
	vec3 Specular = SpecularFactor * Ls * Ks;

	return vec3( Ambient+Diffuse+Specular);
}



void main()
{
	// A fragment normálvektora
	// MINDIG normalizáljuk!
	vec3 normal = normalize( vs_out_norm );
	vec4 color = vec4(ComputeLightEffects(lightPosDir1,La1,Ld1,Ls1) + ComputeLightEffects(lightPosDir2,La2,Ld2,Ls2),1.0f);
//	fs_out_col = vec4(normal,1.0f);
	fs_out_col = color  * texture(texImage, vs_out_tex);
}