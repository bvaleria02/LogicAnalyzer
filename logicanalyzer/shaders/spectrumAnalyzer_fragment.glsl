#version 330 core
in vec2 texPos;

out vec4 FragColor;

uniform sampler1D fftBins;

const vec4 bgColor   	 = vec4(0.0f,   0.0f, 	0.0f,   1.0f);
const vec4 bgColor2  	 = vec4(0.076f, 0.04f, 	0.111f, 1.0f);
const vec4 mainColor 	 = vec4(0.416f, 0.0f, 	0.188f, 1.0f);
const vec4 mainColor2 	 = vec4(0.271f, 0.0f, 	0.408f, 1.0f);
const vec4 mainColorH 	 = vec4(0.624f, 0.134f,	0.205f, 1.0f);
const vec4 mainColor2H  = vec4(0.086f, 0.112f,	0.314f, 1.0f);
const vec4 accentColor  = vec4(1.0f,   0.0f, 	0.0f,   1.0f);
const vec4 accentColor2 = vec4(1.0f,   1.0f, 	1.0f,   1.0f);
const vec4 gridColor	 = vec4(0.3f,   0.3f, 	0.3f,   1.0f);

const float accentThres    = 0.02;
const float accent2Thres   = 0.002;
const float accentThres2   = 0.03;
const float accent3ThresX  = 0.0025;
const float accent3ThresY  = 0.01;
const float mulX  			= 32;
const float mulY  			= 8;

void main(){
	float val = texture(fftBins, texPos.x).r;
	float mainMag = smoothstep(val + 0.1, val - 0.1, texPos.y);

	float accentMag  = smoothstep(val - accentThres2, val + accentThres2, texPos.y + accentThres) - step(val, texPos.y);
	float accent2Mag = step(val - accent2Thres, texPos.y) - step(val + accent2Thres, texPos.y);

	float normX = texPos.x * mulX;
	float normY = texPos.y * mulY;
	float fracX = normX - floor(normX);
	float fracY = normY - floor(normY);
	float gridXmag = step(fracX - accent3ThresX, texPos.x) - step(fracX + accent3ThresX, texPos.x);
	float gridYmag = step(fracY - accent3ThresY, texPos.y) - step(fracY + accent3ThresY, texPos.y);

	vec4 background = mix(
							mix(bgColor2, bgColor, texPos.y),
							gridColor,
							max(gridXmag, gridYmag)
						);

	vec4 foreground = mix(
							mix(mainColor2, mainColor2H, val),
							mix(mainColor, mainColorH, val),
							min(0.8, 1.25 * texPos.y)
						);
    FragColor = mix(
					mix(
						mix(background, foreground, mainMag),
						accentColor,
						accentMag
					),
					accentColor2,
					accent2Mag
				);
}
