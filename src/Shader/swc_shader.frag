#version 460

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main(){
    float r = 1;
    vec2 coord = gl_PointCoord * 2.0 - vec2(1.0); // 将坐标转换到[-1,1]区间
    if (length(coord) > r) {
        discard; // 如果距离大于半径，丢弃这个片段
    }
    outColor = vec4(fragColor, 1.0);
}
