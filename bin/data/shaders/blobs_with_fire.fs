#version 150

uniform vec2      resolution;           // viewport resolution (in pixels)
uniform vec2      kRes;
uniform float     time;           // shader playback time (in seconds)

//in float depth;
in vec4 colorVarying;
in vec2 texCoordVarying;
                                     
out vec4 fragColor;

uniform float p1Intensity;
uniform float p2Intensity;

uniform vec3 p1SpellPoint;
uniform vec3 p2SpellPoint;

uniform vec3 p1PrevSpell;
uniform vec3 p2PrevSpell;

uniform int p1SpellType;
uniform int p2SpellType;

uniform float p1Impact;
uniform float p2Impact;

uniform float collide;
uniform vec3 collidePoint;

uniform float flicker;

uniform float p1Health;
uniform float p2Health;

//uniform float flareSize;



//uniform vec2 res;

uniform sampler2DRect videoTex;

//vec4 spellColor;


//////////////////////
// Fire Flame shader

// procedural noise from IQ
vec2 hash( vec2 p)
{
  p = vec2( dot(p,vec2(127.1,311.7)),
       dot(p,vec2(269.5,183.3)) );

  // if (sType == 1){
  // return -1.0 + 2.0*fract(sin(p)*43758.5453123);
  // } else if (spellType == 2){
    return -1.0 + 2.0*fract(sin(p*0.0001)*43758.5453123);
  // }
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

float wbm(vec2 uv)
{
  float f;
  mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
  f  = 0.5000*noise( uv ); uv = m*uv;
  f += 0.2500*noise( uv ); uv = m*uv;
  f += 0.1250*noise( uv ); uv = m*uv;
  f += 0.0625*noise( uv ); uv = m*uv;

  f = 0.6 + 0.5*f;
  return f;
}

// no defines, standard redish flames
//#define BLUE_FLAME
//#define GREEN_FLAME


vec4 makeSpell(vec2 tp, int player, int spellType){

  float intensity;
  vec2 spellPoint;
  vec2 psp;
  vec4 spellColor;


  if (player == 2){
    intensity = p2Intensity;
    spellPoint = vec2(p2SpellPoint.x,p2SpellPoint.y);
    psp = vec2(p2PrevSpell.x,p2PrevSpell.y);
  } else {
    intensity = p1Intensity;
    spellPoint = vec2(p1SpellPoint.x,p1SpellPoint.y);
    psp = vec2(p1PrevSpell.x,p1PrevSpell.y);
  }

  //why is both this and scaleMult in here?
  vec2 sp=spellPoint.xy*(2.-intensity*1.5);
  psp *= (2-intensity*1.5);

  //MAKE FLAME
  if (spellType == 1){
  vec2 uv = -tp;
  vec2 q = uv;
  //q.x += -1.0*cos(q.y*3.141592653589)*(psp.x-sp.x);
  q.x += -q.y*(psp.x-sp.x);//cos(-q.y*(sp.x-psp.x));
  //q.y += q.x*(psp.y-sp.y);
  float scaleMult = 2.0 - intensity*1.5;
  q*=scaleMult;
  //q.x += -q.y*(psp.x-sp.x);//cos(-q.y*(sp.x-psp.x));
  float strength = floor(2.);
  float T3 = max(3.,1.25*strength)*time;

  //vec2 sp=spellPoint.xy*(2-intensity*1.5);
  //sp.x += sp.x*sin(q.y)+psp.x*cos(q.y);

  //will increasing this actually LOWER fire?
   sp.y -= 0.05;

  q -= sp;
    //this controls y position
  //q.y -= 1.5;

  float n = fbm(vec2(strength*q.x,strength*q.y) - vec2(0,T3));
  float c = 1. - 16. * pow( max( 0., length(q*vec2(1.8+q.y*1.5,.75) ) - n * max( 0., q.y+.25 ) ),1.2 );

    //adjusting float before power changes intensity; lower number inside power is higher flame height
    float c1 = n * c * ((1.+intensity*1.5)-pow((2.0-intensity)*q.y,4.));
  c1=clamp(c1,0.,1.);

  vec3 col = vec3(1.5*c1, 1.5*c1*c1*c1, c1*c1*c1*c1*c1*c1);

  //THIS MAKES P2 GREEN FIRE:
  // if (player == 2){
  //   col = 0.85*col.yxz;
  // } 
  
  
  float a = c * (1.-pow(uv.y,3.));
  a = clamp(a,0.,1.);
  vec4 flameColor = vec4( mix(vec3(0.),col,a), 1.0);

  spellColor = flameColor;

  } else if (spellType == 2) {

    //MAKE WATER
  vec2 uv = tp;
  vec2 q = uv;

  float scaleMult = 2.0 - intensity*1.5;
  q*=scaleMult;

  q += sp;
  q.y+=intensity*0.2;
  q.x += sin(q.y)*(psp.x-sp.x);
  //q.x += sin(q.y*1.5)*(psp.x-sp.x);//cos(-q.y*(sp.x-psp.x));

//POSSIBLE ROTATION:
// vec2 tempQ = q;
    // q.x = q.x*sin(psp.x-sp.x) + q.y*cos(psp.x-sp.x);
    // q.y = tempQ.y*sin(psp.x-sp.x) - tempQ.x*cos(psp.x-sp.x); // factor in y distance?

  float strength = floor(6.);

  float T3 = max(3.,1.25*strength)*time*0.8+pow(q.y,1.25)*2.; //was *1.2

  float n = wbm(vec2(strength*q.x,strength*q.y) - vec2(0,T3));
  strength=26.;
  float T3B = max(3.,1.25*strength)*time*0.6+pow(abs(q.y),1.25)*2.;
   n = n*0.5 + (n*0.5)/(0.001+1.5*wbm(vec2(strength*q.x,strength*q.y) - vec2(0,T3B)));

  n*=1.0+pow(intensity,8.)*0.5;

  //Second part of this - with vec2 - adjusts shape
  float c = 1. - (16./ abs(q.y*4.+1.)) * pow( max( 0., length(q*vec2(1.8+q.y*1.5,.75) ) - n * max( 0., q.y+.25 ) ),1.2 );

  //float c = 1. - 64.*(abs(pow(q.y),1.)*4.+1.) * pow( max( 0., length(q*vec2(1.8+q.y*1.5,.75) ) - n * max( 0., q.y+.25 ) ),1.2 );

    //adjusting float before power changes intensity; this is adjusted to reduce washout as water gets more intense
    float c1 = n * c * ((1.1+intensity*0.4-pow(intensity,8.)*0.7)-pow((0.5)*q.y,2.));

    c1= c1*1.05+sin(c1*3.4)*0.4;
    c1*=0.95-pow(q.y,2.0);
    c1=clamp(c1,0.6,1.);

  vec3 col = vec3(1.5*c1*c1*c1, 1.5*c1*c1*c1*c1, 1.25*c1*c1*c1*c1);

col = col.zyx;
  
  float a = c * (1.-pow(abs(uv.y),10.));
  a = clamp(a,0.,0.9);
  vec4 waterColor = vec4( mix(vec3(0.),col,a), 1.0);

  spellColor = waterColor;
  
  } 

  return spellColor;

}


vec4 makeFlare(vec2 p, vec2 pos, vec3 lcol, float sz)
{
  p = -p;
  vec2 q = p-pos;
    q *= sz;
    
    // vec2 tempQ = q;
    // q.x = q.x*sin(psp.x-sp.x) + q.y*cos(psp.x-sp.x);
    // q.y = tempQ.y*sin(psp.x-sp.x) - tempQ.x*cos(psp.x-sp.x); // factor in y distance?
    
  float a = atan(q.x,q.y);
    float r = length(q);
    
    float rz= 0.;
    rz += .2/(length((q)*vec2(7.,100.))); //horiz streaks
    rz += 0.3*(pow(abs(fract(a*.97+.52)-0.5),1.)*(sin(a*15.)*0.15+0.85)*exp2((-r*5.))); //Spokes
  
    //lcol.r=rz;
    vec3 col = vec3(rz)*lcol;   
    col += exp2((1.-length(q))*50.-50.)*lcol*vec3(3.);
    col += exp2((1.-length(q))*20.-20.)*lcol*vec3(1,0.95,0.8)*0.5;    
    col = clamp(col,0.,1.);

    float alpha = (col.r + col.g + col.b)/3;

    vec4 finalFlare = vec4( mix(vec3(0.),col,alpha), 1.0);

    return finalFlare;
}

float sbm(vec2 uv)
{
  float f;
  mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
  f  = 0.5000*noise( uv ); uv = m*uv;
  f += 0.2500*noise( uv ); uv = m*uv;
  f += 0.1250*noise( uv ); uv = m*uv;
  f += 0.0625*noise( uv ); uv = m*uv;
    //increasing first float here widens flow
  f = 0.5 + 0.25*f;
  return f;
}

vec4 makeSteam(vec2 tp, float sz){

  float intensity;
  vec2 steamPoint;


  if (p2Intensity > p1Intensity){
    intensity = p1Intensity;
    //spellPoint = vec2(p2SpellPoint.x,p2SpellPoint.y);
  } else {
    intensity = p2Intensity;
    //spellPoint = vec2(p1SpellPoint.x,p1SpellPoint.y);
  }

  steamPoint = vec2(collidePoint.x,collidePoint.y);
 // steamPoint = (vec2(p1SpellPoint.x,p1SpellPoint.y)+vec2(p2SpellPoint.x,p2SpellPoint.y))*0.5;

  vec2 sp=steamPoint.xy*(2.0-intensity*1.5);

  vec2 uv = -tp;
  vec2 q = uv;

  //q.y += q.x*(psp.y-sp.y);
  float scaleMult = 2.0 - intensity*1.5;
  q*=scaleMult;
  //q.x += -q.y*(psp.x-sp.x);//cos(-q.y*(sp.x-psp.x));
  float strength = floor(2.);
  //float T3 = max(3.,1.25*strength)*time;
  float T3 = max(3.,1.25*strength)*time*0.08+pow(q.y,1.)*1.;

  //vec2 sp=spellPoint.xy*(2-intensity*1.5);
  //sp.x += sp.x*sin(q.y)+psp.x*cos(q.y);
   //sp.y += 0.05;

   //sp.y*=-1.;
  q -= sp;
  //q.y += 0.1;
    //this controls y position
  //q.y -= 1.5;

  float n = sbm(vec2(strength*q.x,strength*q.y) - vec2(0,T3));
  n*=1.+pow(intensity,8.)*0.8;
  float c = 1. - 2.0 * pow( max( 0., length(q*vec2(1.8+q.y*1.5,.75) ) - n * max( 0., q.y+.25 ) ),1.2 );

  //float c = 1. - 16. * pow( max( 0., length(q*vec2(1.8+q.y*1.5,.75) ) - n * max( 0., q.y+.25 ) ),1.2 );

    //adjusting float before power changes intensity; this is adjusted to reduce washout as water gets more intense
    float c1 = n * c * ((1.5+pow(intensity,1.)*0.5)-pow((1.0)*abs(q.y),2.));//-abs(sin(iGlobalTime*0.2))*uv.y
    
    // c1= c1*1.05+sin(c1*3.4)*0.4;
    // c1*=0.95-pow(q.y,2.0);
    c1=clamp(c1,0.4,1.);

  vec3 col = vec3(1.25*c1*c1*c1, 1.25*c1*c1*c1*c1, 1.25*c1*c1*c1*c1);

  col = col.zyx;
  
  float a = c * (1.-pow(uv.y,10.));
  a = clamp(a,0.,0.9);
  vec4 steamColor = vec4( mix(vec3(0.),col,a), 1.0);
  
  return steamColor;

}

void main( void ) {


   vec2 tp = (texCoordVarying.xy/resolution.x) * 2.0 - vec2(1.0,resolution.y/resolution.x);

  vec2 videoScaling = vec2(kRes.x/resolution.x, kRes.y/resolution.y);

   vec4 vidColor = vec4(texture(videoTex, texCoordVarying.xy * videoScaling).rgb, 1.0);
   //vec4 depthColor = vec4(texture(depthTex, texCoordVarying.xy * videoScaling).rgb, 1.0);

   //if color stream, make grayScale
   //float grayComp = (vidColor.r + vidColor.g + vidColor.b) / 3.0;
   //vec4 vidGray = vec4(grayComp, grayComp, grayComp, 1.0);

   vec4 vidGray = vidColor;
   vidGray.rgb = 1.0-vidColor.rgb;


    if (vidGray.x < 0.1 || vidGray.x > 0.8){
     vidGray.b = 0.0;
     vidGray.r = 0.0;
     vidGray.g = 0.0;
    }

    //if (tp.x > -0.2 && tp.x < 0.2){
     //float fade = 1-cos(tp.x*(3.141592653589));
     float fade = abs(tp.x*2.0);
     fade = clamp(fade,0.0,1.0);
     vidGray *= fade; // CHANGED FOR BRIGTHER
  //   // vidGray.r *= fade;
  //   // vidGray.g *= fade;
   //}

   if (tp.x < 0.0){  
    vidGray.rb *=0.8;
    vidGray *= (p1Health+0.2);
    //vidGray *= ((1-p1Health)*flicker);
   } else {
    vidGray.gb *=0.8;
    vidGray *= (p2Health+0.2);
    //vidGray *= ((1-p2Health)*flicker);
   } 

    
    //vidGray.b = 0.0;
    //vidGray.r = 0.0;
    //vidGray.g = 0.0;
   //}
   //vidGray*=1./threshold;
   vidGray = clamp(vidGray,0.,1.);
   //this inverts
   //vidGray.rgb = 1.0-vidGray.rgb;
  

   vec4 spell1;
   vec4 spell2;

   if (p1SpellType != 0){
    spell1 = makeSpell(tp,1,p1SpellType);
    float interp = (spell1.x + spell1.y + spell1.z)/3.0;
    //vidGray = mix(vidGray,spell1,interp);
    vidGray+=spell1;
    // if (p1SpellType == 2){
    //   vidGray.y+=spell1.y;
    // }
   } else {
    spell1 = vec4(0.);
   }

   if (p2SpellType != 0){
    spell2 = makeSpell(tp,2,p2SpellType);
    float interp = (spell2.x + spell2.y + spell2.z)/3.0;
   // vidGray = mix(vidGray,spell2,interp);
    vidGray += spell2;
    //spellsMixed = mix(spell1,spell2,1.0);
   } else {
    spell2 = vec4(0.);
   }

if (collide > 0){
   vec4 steam = makeSteam(tp, collide);//input flaresize
    //float interp = (lightFlare1.x + lightFlare1.y + lightFlare1.z)/3.0;
   //vidGray = mix(vidGray,lightFlare1,interp);
   vidGray += steam;
 }

   if (p1Impact > 0){
   vec4 lightFlare1 = makeFlare(tp, vec2(p1SpellPoint.x,p1SpellPoint.y), vec3(1.), p1Impact);//input flaresize
    //float interp = (lightFlare1.x + lightFlare1.y + lightFlare1.z)/3.0;
   //vidGray = mix(vidGray,lightFlare1,interp);
   vidGray += lightFlare1;
 }
  if (p2Impact > 0){
    vec4 lightFlare2 = makeFlare(tp, vec2(p2SpellPoint.x,p2SpellPoint.y), vec3(1.), p2Impact);
    float interp = (lightFlare2.x + lightFlare2.y + lightFlare2.z)/3.0;
    vidGray = mix(vidGray,lightFlare2,interp);
}


  fragColor = vidGray;

  //vec4 spellsMixed = mix(spell1,spell2,1.0);

  //float interp = (spellsMixed.x + spellsMixed.y + spellsMixed.z)/3.0;

  //fragColor = mix(vidGray, spellsMixed, interp);
  
   //vec4 mixedColor = mix(vidGray, spellColor, interp);

   //fragColor = mixedColor;
   

    //fragColor = vidGray;

}
