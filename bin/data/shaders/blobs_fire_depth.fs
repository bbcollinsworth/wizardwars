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


//////////////////////
// Fire Flame shader

// procedural noise from IQ
vec2 hash( vec2 p )
{
  p = vec2( dot(p,vec2(127.1,311.7)),
       dot(p,vec2(269.5,183.3)) );
  return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
  const float K1 = 0.366025404; // (sqrt(3)-1)/2;
  const float K2 = 0.211324865; // (3-sqrt(3))/6;
  
  vec2 i = floor( p + (p.x+p.y)*K1 );
  
  vec2 a = p - i + (i.x+i.y)*K2;
  vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0);
  vec2 b = a - o + K2;
  vec2 c = a - 1.0 + 2.0*K2;
  
  vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
  
  vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
  
  return dot( n, vec3(70.0) );
}

float fbm(vec2 uv)
{
  float f;
  mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
  f  = 0.5000*noise( uv ); uv = m*uv;
  f += 0.2500*noise( uv ); uv = m*uv;
  f += 0.1250*noise( uv ); uv = m*uv;
  f += 0.0625*noise( uv ); uv = m*uv;
  f = 0.5 + 0.5*f;
  return f;
}

// no defines, standard redish flames
//#define BLUE_FLAME
//#define GREEN_FLAME


void main( void ) {

   vec2 tp = (texCoordVarying.xy/resolution.x) * 2.0 - vec2(1.0,resolution.y/resolution.x);

   //vec2 hp=tp+headPoint.xy;

   //vec2 lp=tp+lHandPoint.xy;
   vec2 lp=lHandPoint.xy*(2-intensity*1.5);
   vec2 rp=rHandPoint.xy*(2-intensity*1.5);
   lp.y +=0.15;
   rp.y +=0.15;
   lp /= 2.;
   rp /= 2.;
   //vec2 rp=tp+rHandPoint.xy;

   //lp;

  // vec2 uv = fragCoord.xy / iResolution.xy;
  vec2 uv = -tp;
  vec2 q = uv;//uv;
  float scaleMult = 2.0 - intensity*1.5;
  q*=scaleMult;
  //q.x *= 5.;
  //q.y *= 3.;
  float strength = floor(2.);//floor(q.x+1.); +sin(iGlobalTime*0.1)
  float T3 = max(3.,1.25*strength)*time;//iGlobalTime;
    
  //this controls xposition of frame - relative to multiplier above
  //q.x -= 2.5;//q.x = mod(q.x,1.)-0.5;
  q -= lp;
  q -= rp;
   
    //this controls y position
  //q.y -= 1.5;
  float n = fbm(vec2(strength*q.x,strength*q.y) - vec2(0,T3));
  float c = 1. - 16. * pow( max( 0., length(q*vec2(1.8+q.y*1.5,.75) ) - n * max( 0., q.y+.25 ) ),1.2 );
//  float c1 = n * c * (1.5-pow(1.25*uv.y,4.));
    //adjusting float before power changes intensity; lower number inside power is higher flame height
  //float intensityAdjust = abs(sin(iGlobalTime*0.2);
    //switched number inside power from intensity to 1 to get flame to not wash out screen top
    float c1 = n * c * ((1.+intensity*1.5)-pow((2.0-intensity)*q.y,4.));

    //float c1 = n * c * ((1.+intensity*1.5)-pow((2.0-intensity)*uv.y,4.));
    //float c1 = n * c * ((1.+abs(sin(time*0.2)*1.5))-pow((2.0-abs(sin(time*0.2)))*uv.y,4.));
  c1=clamp(c1,0.,1.);

  vec3 col = vec3(1.5*c1, 1.5*c1*c1*c1, c1*c1*c1*c1*c1*c1);
  
#ifdef BLUE_FLAME
  col = col.zyx;
#endif
#ifdef GREEN_FLAME
  col = 0.85*col.yxz;
#endif
  
  float a = c * (1.-pow(uv.y,3.));
  a = clamp(a,0.,1.);
  vec4 flameColor = vec4( mix(vec3(0.),col,a), 1.0);

   float interp = (flameColor.x + flameColor.y + flameColor.z)/3.0;

   vec2 videoScaling = vec2(640.0/1024.0, 480.0/768.0);

   vec4 vidColor = vec4(texture(videoTex, texCoordVarying.xy * videoScaling).rgb, 1.0);
   //vec4 depthColor = vec4(texture(depthTex, texCoordVarying.xy * videoScaling).rgb, 1.0);

   //make grayScale
   float grayComp = (vidColor.r + vidColor.g + vidColor.b) / 3.0;
   vec4 vidGray = vec4(grayComp, grayComp, grayComp, 1.0);

   //this inverts
   vidGray.rgb = 1.0-vidGray.rgb;


   //float depthGray = ()
   //vec4 vidGray = vec4((texture2DRect(videoTex, gl_TexCoord[0].xy * videoScaling).r+texture2DRect(videoTex, gl_TexCoord[0].xy * videoScaling).g+texture2DRect(videoTex, gl_TexCoord[0].xy * videoScaling).b)*0.333, 1.0);

   vec4 mixedColor = mix(vidColor, flameColor, interp);

   fragColor = mixedColor;

  //vec4 finalMix = vec4(texture(mixedColor, texCoordVarying.xy * videoScaling).rgb, 1.0);
    //fragColor = mix(vidGray, flameCol, interp);

    //fragColor = mix(vec4(0.), col, pow(glow*2.5,4.));
   //fragColor = mix(vidGray, col, pow(glow*2.5,4.));
   //fragColor = vidGray;
   //fragColor = mix( vidGray, blobColor, interp);
}
