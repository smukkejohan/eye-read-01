#version 150

// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;
in vec4 position;
in vec2 texcoord;

uniform float mouseRange;

uniform vec2 magnificationArea;

uniform vec2 mousePos;
uniform vec4 mouseColor;

out vec2 texCoordVarying;

void main()
{
    // copy position so we can work with it.
    vec4 pos = position;
    
    // direction vector from mouse position to vertex position.
	vec2 dir = pos.xy - mousePos;
    float xD = pos.x - mousePos.x;
    float yD = pos.y - mousePos.y;

    // distance between the mouse position and vertex position.
	float dist =  sqrt(dir.x * dir.x + dir.y * dir.y);
    
    // ball under
    
    // check vertex is within mouse range.
	/*if(dist > 0.0 && dist < mouseRange) {
		
		// normalise distance between 0 and 1.
		float distNorm = dist / mouseRange;
        
		// flip it so the closer we are the greater the repulsion.
		distNorm = 1.0 - distNorm;
		
        // make the direction vector magnitude fade out the further it gets from mouse position.
        dir *= distNorm;
        
		// add the direction vector to the vertex position.
		pos.x += dir.x;
		pos.y += dir.y;
	}*/
    
    
    // move rectangle closer
    /*if(abs(xD) < magnificationArea.x && abs(xD) > 0 && abs(yD) < magnificationArea.y && abs(yD) > 0) {
        
        // normalise distance between 0 and 1.
            float distNormx = xD / magnificationArea.x;
            float distNormy = yD / magnificationArea.y;

            // flip it so the closer we are the greater the repulsion.
            distNormx = 1.0 - distNormx;
            distNormy = 1.0 - distNormy;

            // make the direction vector magnitude fade out the further it gets from mouse position.
            dir.x *= distNormx;
            dir.y *= distNormy;

            // add the direction vector to the vertex position.
        
        pos.x *= 2;// dir.x; // TODO move towards camera
        pos.y *= 2;// dir.y;
            
        
        //pos.z += 400; // move towards camera instead of on z axis
    }*/

    texCoordVarying = texcoord; 
    
    // finally set the pos to be that actual position rendered
	gl_Position = modelViewProjectionMatrix * pos;
    
}


