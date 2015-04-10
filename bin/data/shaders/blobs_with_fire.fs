#version 150

uniform vec2      resolution;           // viewport resolution (in pixels)
uniform float     time;           // shader playback time (in seconds)

//in float depth;
in vec4 colorVarying;
in vec2 texCoordVarying;
																		 
out vec4 fragColor;

// uniform float frequency;
// uniform float scalar;
// uniform float blobDensity;
uniform float intensity;

//uniform vec3 headPoint;
uniform vec3 rHandPoint;
uniform vec3 lHandPoint;

uniform sampler2DRect videoTex;
//uniform sampler2DRect depthTex;

// float makePoint( float x, float y, float fx, float fy, float sx, float sy, float scale, float freq)
// {
//     float fsx = fx * freq;
//     float fsy = fy * freq;

//     float ssx = sx * scale;
//     float ssy = sy * scale;

//    float xx=x+(sin(time*fsx)*ssx);
//    float yy=y+(cos(time*fsy)*ssy);
//    return 1.0/sqrt(xx*xx+yy*yy);
// }

float noise(vec3 p) //Thx to Las^Mercury
{
  vec3 i = floor(p);
  vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
  vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;
  a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);
  a.xy = mix(a.xz, a.yw, f.y);
  return mix(a.x, a.y, f.z);
}

float sphere(vec3 p, vec4 spr)
{
  return length(spr.xyz-p) - spr.w;
}

float flame(vec3 p)
{
  float d = sphere(p*vec3(.7,.5,.7), vec4(.0,-1.,.0,1.));
    //decreasing 2nd noise multiplier returns "bigger" noise (less detailed)
  return d + (noise(p+vec3(.0,time*4.,.0)) + noise(p*2.)*.5)*.25*(p.y) ;
}

float scene(vec3 p)
{
  return min(100.-length(p) , abs(flame(p)) );
}

vec4 raymarch(vec3 org, vec3 dir)
{
    //increasing eps DIMS flame -- good range is 2 to 0.2
  float d = 0.0, glow = 0.0, eps = intensity;
  vec3  p = org;
  bool glowed = false;
  
  for(int i=0; i<64; i++)
  {
    d = scene(p) + eps;
    p += d * dir;
    if( d>eps )
    {
      if(flame(p) < .0)
        glowed=true;
      if(glowed)
            glow = float(i)/64.;
    }
  }
  return vec4(p,glow);
}


void main( void ) {

   vec2 tp = (texCoordVarying.xy/resolution.x) * 2.0 - vec2(1.0,resolution.y/resolution.x);

   //vec2 hp=tp+headPoint.xy;
   vec2 lp=tp+lHandPoint.xy;
   vec2 rp=tp+rHandPoint.xy;



//TRIED COMMENTING THIS... COULD CHANGE WITH EXISTING texcoord and resolution variables
// vec2 v = -1.0 + 2.0 * texCoordVarying.xy / resolution.xy;
//    v.x *= resolution.x/resolution.y;
  // vec2 v = -1.0 + 2.0 * fragCoord.xy / iResolution.xy;
  //  v.x *= iResolution.x/iResolution.y;
  
  //changed this to take hand coordinate
  vec3 org = vec3(lp.x, lp.y, 4.);
    //this controls width and shape:
  vec3 dir = normalize(vec3(tp.x*1.5, tp.y*1.1, -0.6));
  
  vec4 p = raymarch(org, dir);
  float glow = p.w;
  
    //adjust p.y multiplier down and added part up for 'bigger, brighter' flame
  vec4 col = mix(vec4(.7,.3,.0,1.), vec4(1.0,0.7,0.2,1.), p.y*.02+.8);
  vec4 glowedCol = mix(vec4(0.), col, pow(glow*2.5,4.));
  
  //fragColor = mix(vec4(0.), col, pow(glow*2.5,4.));
   
   //vec3 d=vec3(b,b,b) / (64.0 * blobDensity);
   
   //vec4 blobColor = vec4(d.x,d.y,d.z,1.0);
   //float interp = (d.x + d.y + d.z)/3.0;
   float interp = (glowedCol.x + glowedCol.y + glowedCol.z)/3.0;

   vec2 videoScaling = vec2(640.0/1024.0, 480.0/768.0);

   vec4 vidColor = vec4(texture(videoTex, texCoordVarying.xy * videoScaling).rgb, 1.0);
   //vec4 depthColor = vec4(texture(depthTex, texCoordVarying.xy * videoScaling).rgb, 1.0);

   //make grayScale
   float grayComp = (vidColor.r + vidColor.g + vidColor.b) / 3.0;
   vec4 vidGray = vec4(grayComp, grayComp, grayComp, 1.0);

   //float depthGray = ()
   //vec4 vidGray = vec4((texture2DRect(videoTex, gl_TexCoord[0].xy * videoScaling).r+texture2DRect(videoTex, gl_TexCoord[0].xy * videoScaling).g+texture2DRect(videoTex, gl_TexCoord[0].xy * videoScaling).b)*0.333, 1.0);

    fragColor = mix(vidGray, glowedCol, interp);
    //fragColor = mix(vec4(0.), col, pow(glow*2.5,4.));
   //fragColor = mix(vidGray, col, pow(glow*2.5,4.));
   //fragColor = vidGray;
   //fragColor = mix( vidGray, blobColor, interp);
}
