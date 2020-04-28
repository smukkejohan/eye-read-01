/*
"Magnifying glass" by Emmanuel Keller aka Tambako - August 2019
License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
Contact: tamby@tambako.ch
*/

#define pi 3.141593

// The defines to switch on and off some features
#define specular
#define shadow
#define lens_refraction

#define PAPER_OBJ      1
#define LENS_OBJ       2
#define RIM_OBJ        3
#define HANDLE_OBJ     4

struct Lamp
{
  vec3 position;
  vec3 color;
  float intensity;
  float attenuation;
};

struct RenderData
{
  vec3 col;
  vec3 pos;
  vec3 norm;
  float objnr;
};

Lamp lamps[3];
    
// Campera options
vec3 campos = vec3(0.5, -0.4, 9.);
vec3 camtarget = vec3(0., 0., 0.);
vec3 camdir;
float fov = 5.5;
float angle;

const float mg_scale = 3.1;
const float mg_angle = 45.;

const float normdelta = 0.0015;
const float maxdist = 60.;

const vec3 ambientColor = vec3(0.3);
const float ambientint = 0.12;
const vec3 speccolor = vec3(0.95, 0.97, 1.);

const float specint_paper = 0.04;
const float specshin_paper = 15.;
const float specint_glass = 0.055;
const float specshin_glass = 480.;
const float specint_rim = 0.18;
const float specshin_rim = 25.;
const float specint_handle = 0.04;
const float specshin_handle = 15.;

const float shi = 0.6;
const float shf = 0.2;

float glass_ior = 1.5;
vec3 glass_vol_col = vec3(0.96, 0.99, 0.96);

vec3 rim_color = vec3(0.88, 0.9, 0.92);
vec3 handle_color = vec3(0.23);

// Antialias. Change from 1 to 2 or more AT YOUR OWN RISK! It may CRASH your browser while compiling!
const float aawidth = 0.8;
const int aasamples = 1;

// Union operation from iq
vec2 opU(vec2 d1, vec2 d2)
{
    return (d1.x<d2.x) ? d1 : d2;
}

// From https://www.shadertoy.com/view/Xds3zN
float sdCylinder(vec3 p, vec2 h, float t, float rr)
{
    h-= vec2(rr);
    vec2 d = abs(vec2(length(p.xz*(1. + t*p.y)),p.y)) - h;
    return -rr + min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float sdTorus(vec3 p, vec2 t)
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

vec2 rotateVec(vec2 vect, float angle)
{
    vec2 rv;
    rv.x = vect.x*cos(angle) + vect.y*sin(angle);
    rv.y = vect.x*sin(angle) - vect.y*cos(angle);
    return rv;
}

float get_mg_posz()
{
    return texture(iChannel2, vec2(0., 0.)).r;
}

float get_mg_angle()
{
    return texture(iChannel2, vec2(0., 0.)).b;
}

float get_zoom()
{
    return texture(iChannel2, vec2(0., 0.)).a;
}

float map_lens(vec3 pos)
{
    pos.z+= 3.2 - get_mg_posz();
    pos/= mg_scale;
    pos.z-= 0.77;
    float d = length(pos) - .95;
    pos.z-= 1.81;
    d = max(d, length(pos) - .95);
    d = max(d, length(pos.xy) - 0.26);
    
    //d = max(d, -pos.y);
    return d;
}

float map_rim(vec3 pos)
{
    pos.z-= 2. + get_mg_posz();
    pos/= mg_scale;
    
    vec3 pos2 = pos.xzy;
    float d = sdCylinder(pos2, vec2(0.26, 0.03), 0., 0.);
    d = max(d, -sdCylinder(pos2, vec2(0.26, 0.18), 0., 0.)) - 0.003;
    
    vec3 pos3 = pos;
    pos3.xy = rotateVec(pos3.xy, get_mg_angle()-mg_angle/180.*pi - pi);
    pos3.y+= 0.287;
    d = min(d, sdCylinder(pos3, vec2(0.02, 0.026), 0., 0.));
    pos3.y-= 0.01;
    d = min(d, sdTorus(pos3, vec2(0.02, 0.0037)));
    pos3.y+= 0.028;
    d = min(d, sdTorus(pos3, vec2(0.02, 0.0037)));
    pos3.y+= 0.006;
    d = min(d, sdTorus(pos3, vec2(0.02, 0.0037)));

    //d = max(d, -pos.y);
    return d;
}

float map_handle(vec3 pos)
{
    pos.z-= 2. + get_mg_posz();
    pos/= mg_scale;
    
    vec3 pos3 = pos;
    pos3.xy = rotateVec(pos3.xy, get_mg_angle()-mg_angle/180.*pi - pi);
    
    return sdCylinder(pos3 + vec3(0., 0.556, 0.), vec2(0.032, 0.24), 0.4, 0.008);
}

float map_paper(vec3 pos)
{
    return pos.z;
}

vec2 map(vec3 pos, bool inside, bool nolens)
{
    vec2 res;
    
    float paper = map_paper(pos);
    
    vec2 iMouse2;
    if (iMouse.x==0. && iMouse.y==0.)
       iMouse2 = iResolution.xy*vec2(0.45, 0.5);
    else
       iMouse2 = iMouse.xy;
    
    pos.xy+= (5.6*vec2(1., -1.)*iMouse2.xy/iResolution.xx + vec2(-2.8, 1.6))/get_zoom();
    float lens = map_lens(pos);
    if (inside)
       res = vec2(-lens, LENS_OBJ);
    else
    {
       if (nolens)
           res = vec2(paper, PAPER_OBJ);
       else
           res = opU(vec2(paper, PAPER_OBJ), vec2(lens, LENS_OBJ));
        
       float rim = map_rim(pos);
       res = opU(res, vec2(rim, RIM_OBJ));
        
       float handle = map_handle(pos);
       res = opU(res, vec2(handle, HANDLE_OBJ));
    }
    
    return res;
}

vec2 trace(vec3 cam, vec3 ray, float maxdist, bool inside)
{
    float t = 0.3;
    vec3 pos;
    float dist;
    float objnr = 0.;
    
      for (int i = 0; i < 100; ++i)
    {
        pos = ray*t + cam;
        vec2 res = map(pos, inside, false);
        dist = res.x;
        if (dist>maxdist || abs(dist)<0.0001)
            break;
        t+= dist*0.9;
        objnr = abs(res.y);
      }
        
      return vec2(t, objnr);
}

vec3 getNormal(vec3 pos, float e, bool inside)
{
    vec3 n = vec3(0.0);
    for( int i=0; i<4; i++ )
    {
        vec3 e2 = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e2*map(pos + e*e2, inside, false).x;
    }
    return normalize(n);
}

vec3 sky_color(vec3 ray)
{
    return texture(iChannel1, ray).rgb;
}

vec3 obj_color(vec3 norm, vec3 pos, float objnr, vec3 ray)
{
    vec3 colo;
    
    if (int(objnr)==PAPER_OBJ)
       colo = texture(iChannel0, vec2(0.5, 0.3) + pos.xy*vec2(-0.18, 0.18)).rgb;
    else if (int(objnr)==RIM_OBJ)
       colo = rim_color;
    else if (int(objnr)==HANDLE_OBJ)
       colo = handle_color;
    else if (int(objnr)==LENS_OBJ)
       colo = vec3(0.6);
    else
       colo = sky_color(ray);

    return colo;
}


// From https://www.shadertoy.com/view/Xds3zN;
float softshadow(vec3 ro, vec3 rd, float mint, float tmax)
{
    float res = 1.;
    float t = mint;
    for(int i=0; i<15; i++)
    {
        vec2 mr = map(ro + rd*t, false, true);
        float h = mr.x;
        res = min(res, 13.*h/t);
        t += clamp(h, 0.01, 0.7);
        if (h<0.001) break;
    }
    return smoothstep(0.0, 0.5, res);
}

vec3 lampShading(Lamp lamp, vec3 norm, vec3 pos, vec3 ocol, float objnr)
{
    vec3 pl = normalize(lamp.position - pos);
    float dlp = distance(lamp.position, pos);
    vec3 pli = pl/pow(1. + lamp.attenuation*dlp, 2.);
    vec3 nlcol = normalize(lamp.color);
    float dnp = dot(norm, pli);
    
    vec3 col = vec3(0.);
    float specint;
    float specshin;
      
    // Diffuse shading
    if (int(objnr)==PAPER_OBJ || int(objnr)==HANDLE_OBJ)
       col = ocol*nlcol*lamp.intensity*smoothstep(-0.1, 1., dnp);

    specint = int(objnr)==PAPER_OBJ?specint_paper:(int(objnr)==RIM_OBJ?specint_rim:(int(objnr)==HANDLE_OBJ?specint_handle:specint_glass));
    specshin = int(objnr)==PAPER_OBJ?specshin_paper:(int(objnr)==RIM_OBJ?specshin_rim:(int(objnr)==HANDLE_OBJ?specshin_handle:specshin_glass));
    
    // Specular shading
    #ifdef specular
    
    if (dot(norm, lamp.position - pos) > 0.0)
        col+= speccolor*nlcol*lamp.intensity*specint*pow(max(0.0, dot(reflect(pl, norm), normalize(pos - campos))), specshin);
    #endif
    
    // Softshadow
    #ifdef shadow
    //if (int(objnr)==HOLE_OBJ)
       col*= shi*softshadow(pos, normalize(lamp.position - pos), shf, 2.) + 1. - shi;
    #endif
    
    return col;
}

vec3 lampsShading(vec3 norm, vec3 pos, vec3 ocol, float objnr)
{
    vec3 col = vec3(0.);
    for (int l=0; l<3; l++) // lamps.length()
        col+= lampShading(lamps[l], norm, pos, ocol, objnr);
    
    return col;
}

vec3 getGlassAbsColor(float dist, vec3 color)
{
    return pow(color, vec3(0.1 + pow(dist*8., 2.)));
}

// Fresnel reflectance factor through Schlick's approximation: https://en.wikipedia.org/wiki/Schlick's_approximation
float fresnel(vec3 ray, vec3 norm, float n2)
{
   float n1 = 1.; // air
   float angle = acos(-dot(ray, norm));
   float r0 = dot((n1-n2)/(n1+n2), (n1-n2)/(n1+n2));
   float r = r0 + (1. - r0)*pow(1. - cos(angle), 5.);
   return clamp(r, 0., 0.8);
}

// From https://www.shadertoy.com/view/lsSXzD, modified
vec3 GetCameraRayDir(vec2 vWindow, vec3 vCameraDir, float fov)
{
    vec3 vForward = normalize(vCameraDir);
    vec3 vRight = normalize(cross(vec3(0.0, 1.0, 0.0), vForward));
    vec3 vUp = normalize(cross(vForward, vRight));
    
    vec3 vDir = normalize(vWindow.x * vRight + vWindow.y * vUp + vForward * fov);

    return vDir;
}

// Sets the position of the camera with the mouse and calculates its direction
void setCamera()
{
   /*vec2 iMouse2;
   if (iMouse.x==0. && iMouse.y==0.)
      iMouse2 = iResolution.xy*vec2(0.52, 0.65);
   else
      iMouse2 = iMouse.xy;
    
   campos = vec3(3.5 + 10.*cos(1.3 + 1.3*iMouse2.x/iResolution.x)*(1. - 0.0*iMouse2.y/iResolution.y),
                 -13. + 12.*(iMouse2.y/iResolution.y),
                 -5.5 + 10.*sin(1.3 + 1.3*iMouse2.x/iResolution.x)*(1. + 0.6*iMouse2.y/iResolution.y));
   camtarget = vec3(0., -3.2 + 2.8*iMouse2.y/iResolution.y + 0.1, 1.);*/
    
   camdir = camtarget - campos;
}

RenderData trace0(vec3 tpos, vec3 ray, bool inside)
{
  vec2 t = trace(tpos, ray, maxdist, inside);
  float tx = t.x;
  vec3 col;
  float objnr = t.y;
    
  vec3 pos = tpos + tx*ray;
  vec3 norm;
  if (tx<maxdist*0.65)
  {
      norm = getNormal(pos, normdelta, inside);

      // Coloring
      col = obj_color(norm, pos, objnr, ray);
      
      // Shading
      col = ambientColor*ambientint + lampsShading(norm, pos, col, objnr);
  }
  else
  {
      col = sky_color(ray);
      objnr = 0.;
  }
  return RenderData(col, pos, norm, objnr);
}

vec3 render_glass(vec3 ray, RenderData traceinf)
{
     bool inside = true;
     vec3 col = traceinf.col;
    
     vec3 refray = reflect(ray, traceinf.norm);
     float rf = fresnel(ray, traceinf.norm, glass_ior);
      
     vec3 pos1 = traceinf.pos;
      
     vec3 ray_r = refract(ray, traceinf.norm, 1./glass_ior);
     traceinf = trace0(traceinf.pos, ray_r, inside);
      
     vec3 pos2 = traceinf.pos;
     ray = ray_r;
      
     ray_r = refract(ray_r, traceinf.norm, glass_ior);
     inside = !inside;
     if (length(ray_r)==0.)
       ray_r = reflect(ray, traceinf.norm);
     traceinf = trace0(traceinf.pos, ray_r, inside);
     col+= traceinf.col;
      
     col*= getGlassAbsColor(0.6*distance(pos1, pos2), glass_vol_col);
     col = mix(col, sky_color(refray), rf);
    
     return col;
}

vec4 render(vec2 fragCoord)
{
  lamps[0] = Lamp(vec3(2, -2., 23.5), vec3(1.0, 1.0, 1.0), 12.4, 0.1);
  lamps[1] = Lamp(vec3(-14., 2.5, 29.), vec3(0.7, 0.82, 1.0), 5.3, 0.1);
  lamps[2] = Lamp(vec3(1., 0., 6.2), vec3(1.0, 0.6, 0.4), 0.22, 0.1);
    
  vec2 uv = fragCoord.xy / iResolution.xy;
  uv = uv*2.0 - 1.0;
  uv.x*= iResolution.x / iResolution.y;

  vec3 ray = GetCameraRayDir(uv, camdir, fov*get_zoom());
    
  RenderData traceinf = trace0(campos, ray, false);
  vec3 col = traceinf.col;
  float obj = traceinf.objnr;
    
  #ifdef lens_refraction
  if (int(traceinf.objnr)==LENS_OBJ)
  {
     col = render_glass(ray, traceinf);
  }
  #endif
  if (int(traceinf.objnr)==RIM_OBJ || int(traceinf.objnr)==HANDLE_OBJ)
  {
     float rf = fresnel(ray, traceinf.norm, 1.6);
     vec3 ray_r = reflect(ray, traceinf.norm);
     traceinf = trace0(traceinf.pos, ray_r, false);
     //if (int(traceinf.objnr)==PAPER_OBJ)
        //col+= render_glass(ray_r, traceinf);
        //col = vec3(1., 0., 0.);
     //else
     
     if (int(obj)==RIM_OBJ)
        col+= rim_color*traceinf.col;
     else
        col = mix(col, traceinf.col, rf*0.7);
  }
  
    
  return vec4(col, 1.0);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    setCamera();
    
    // Antialiasing
    vec4 orv;
    vec4 vs = vec4(0.);
    for (int j=0;j<aasamples ;j++)
    {
       float oy = float(j)*aawidth/max(float(aasamples-1), 1.);
       for (int i=0;i<aasamples ;i++)
       {
          float ox = float(i)*aawidth/max(float(aasamples-1), 1.);
          vec4 rv = render(fragCoord + vec2(ox, oy));
          vs+= rv;
       }
    }
    fragColor = vs/vec4(aasamples*aasamples);
    
    //vec2 uv = fragCoord/iResolution.xx;
    //fragColor = texture(iChannel0, uv);
}
