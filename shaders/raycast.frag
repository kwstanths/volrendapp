#version 330

smooth in vec3 tex_coords;
smooth in vec3 position;
in vec3 camera_position_world;

uniform sampler3D volume;
/* Transfer function has four channels, one for each rgba component */ 
uniform sampler1D transfer_function;
/* Light position */
uniform vec3 LPOS;
/* Light color */
uniform vec3 LCOL;
/* Calculate shadow */
uniform bool calc_shadow = true;
/* Calculate phong shading */
uniform bool calc_phong = true;

out vec4 frag_color;

vec4 TF(float density) {
   /* Sample color from the transfer function */
   return vec4(texture(transfer_function, density));
}

/* Compose color, front to back, color is the input color, alpha is the opacity accumulation */
vec3 ComposeColor(vec4 color, float alpha){
  return (1 - alpha) * (color.a) * color.xyz;
}
/* Compose alpha front to back, color is the input color, alpha is the opacity accumulation */
float ComposeAlpha(vec4 color, float alpha){
  return (1 - alpha) * color.a;
}

/* Calculate the gradient of a texel, given a small delta */
vec3 CalculateNormal(vec3 texel_pos, float delta) {
  float x = texture(volume, texel_pos + vec3(delta, 0, 0)).r - texture(volume, texel_pos - vec3(delta, 0, 0)).r;
  float y = texture(volume, texel_pos + vec3(0, delta, 0)).r - texture(volume, texel_pos - vec3(0, delta, 0)).r;
  float z = texture(volume, texel_pos + vec3(0, 0, delta)).r - texture(volume, texel_pos - vec3(0, 0, delta)).r;

  /* Normalize and inverse */
  /* Inversion is applied because in the volume that I was testing, ligthning was inversed */
  return -normalize(vec3(x, y, z));
}

vec3 ComputePhongShading(vec3 light_position, vec3 light_color, vec3 fragment_position, vec3 fragment_normal, vec3 fragment_color){
    /* Calculate ambient component */
    vec3 light_ambient = 0.3 * light_color * fragment_color;
    
    /* Calculate diffuse component */
    vec3 light_direction_inv = normalize(light_position - fragment_position); 
    float light_diffuse_strength = max(dot(fragment_normal, light_direction_inv), 0.0);
    vec3 light_diffuse = light_color * light_diffuse_strength * fragment_color;
    
    /* Calculate specular component */
    vec3 view_direction = normalize(camera_position_world - fragment_position);
    vec3 light_reflect_vector = reflect(-light_direction_inv, fragment_normal);
    float light_specular_strength = pow(max(dot(view_direction, light_reflect_vector), 0.0), 16.0);
    vec3 light_specular = light_color * light_specular_strength * 0.1;
		
    return clamp(light_ambient + light_diffuse + light_specular, vec3(0,0,0), vec3(1,1,1));
}

/* Calculate the shadow for a texel */
float CalculateShadow(vec3 texel_world_position, vec3 fragment_tex_coords, vec3 light_position) {
  /* Accumulate alpha from the texel position to the light */
  float alpha_acc = 0.0f;

  /* This is used again to estimate the number of steps */
  float max_texture_size = max(max(textureSize(volume, 0).x, textureSize(volume, 0).y), textureSize(volume, 0).z);

  /* Calculate direction from the texel to light */
  vec3 current_position = fragment_tex_coords;
  vec3 ray =  normalize(LPOS - texel_world_position);
  /* Advance roughly 10 texel per step */
  vec3 step = 10 * ray / max_texture_size;

  for(int i=0; i < 2*max_texture_size; i++) {
    /* Sample texel density */
    float density = texture(volume, current_position).r;
    /* Calculate color */
    vec4 color = TF(density);
    /* Compse alpha */
    alpha_acc += (1 - alpha_acc) * color.a;

    /* Advance ray */
    current_position = current_position + step;

    /* Stop if accumulation is high enough, or we if exited the volume */
    if (alpha_acc >= 0.95) break;
    if (current_position.x > 1 || current_position.x < 0) break;
    if (current_position.y > 1 || current_position.y < 0) break;
    if (current_position.z > 1 || current_position.z < 0) break;
  }
  return alpha_acc;
}


void main (void) {
  
  /* Initial color */
  frag_color = vec4(0, 0, 0, 0);
  /* Calculate maximum texture size, to be used to estimate the 
     number of ray tracing steps
  */
  float max_texture_size = max(max(textureSize(volume, 0).x, textureSize(volume, 0).y), textureSize(volume, 0).z);

  /* Calculate ray direction from cameta to fragment */
  vec3 current_position = tex_coords;
  vec3 ray =  normalize(position - camera_position_world);
  /* March roughly one texel per step, if the angle is perpendicular 
     to the volume
  */
  vec3 step = 1 * ray / max_texture_size;

  for(int i=0; i < 2*max_texture_size; i++) {
    /* Sample texel density from the volume */
    float density = texture(volume, current_position).r;
    /* Calculate color based on the transfer function */
    vec4 color = TF(density);
   
    /* If the texel is highly transparent, then skip it */
    if (color.a <= 0.001) {
       /* Advance ray */
       current_position = current_position + step;
       continue;
    }

    /* Calculate shadow for this texel */
    float shadow = 0.0f;
    if (calc_shadow){
       /* The world position of that texel is current_position - 0.5 
          since the texels are shifted in the vertex shader 
       */
       shadow = CalculateShadow(current_position - vec3(0.5), current_position, LPOS);
    }
    
    /* Calculate phong color for texel */
    vec4 phong_color = color;
    if (calc_phong){
        /* Calculate the gradient of this texel, for the normal, delta is 0.01 */
        /* Shift again for the same reason, we could shift current_position as well */
        phong_color = vec4(ComputePhongShading(LPOS + vec3(0.5), LCOL, current_position, CalculateNormal(current_position, 0.01), color.xyz), color.a);
    }
    
    /* Compose color and alpha, front to back, multiply color with shadow */
    frag_color.xyz += (1-shadow) * ComposeColor(phong_color, frag_color.a);
    frag_color.a += ComposeAlpha(phong_color, frag_color.a);

    /* Advance ray */
    current_position = current_position + step;

    /* Exit if opacity is big enough, or if we ray exited the volume */
    if (frag_color.a >= 0.95) break;
    if (current_position.x > 1 || current_position.x < 0) break;
    if (current_position.y > 1 || current_position.y < 0) break;
    if (current_position.z > 1 || current_position.z < 0) break;
  }

}
