//
//  AgoraMetalShader.metal
//  Agora-Custom-Media-Device
//
//  Created by GongYuhua on 2017/11/15.
//  Copyright © 2017年 Agora. All rights reserved.
//

#include <metal_stdlib>

using namespace metal;

typedef struct {
    float4 renderedCoordinate [[position]];
    float2 textureCoordinate;
} TextureMappingVertex;

vertex TextureMappingVertex mapTexture(unsigned int vertex_id [[ vertex_id ]],
                                       const device packed_float4* vertex_array [[ buffer(0) ]]) {
    
    float4x4 renderedCoordinates = float4x4(vertex_array[0], vertex_array[1], vertex_array[2], vertex_array[3]);
    float4x2 textureCoordinates = float4x2(float2( 0.0, 1.0 ),
                                           float2( 1.0, 1.0 ),
                                           float2( 0.0, 0.0 ),
                                           float2( 1.0, 0.0 ));
    
    TextureMappingVertex outVertex;
    outVertex.renderedCoordinate = renderedCoordinates[vertex_id];
    outVertex.textureCoordinate = textureCoordinates[vertex_id];
    
    return outVertex;
}

fragment float4 displayNV12Texture(TextureMappingVertex mappingVertex [[stage_in]],
                                   texture2d<float, access::sample> textureY [[ texture(0) ]],
                                   texture2d<float, access::sample> textureUV [[ texture(1) ]]) {
    constexpr sampler colorSampler(mip_filter::linear,
                                   mag_filter::linear,
                                   min_filter::linear);
    
    const float4x4 ycbcrToRGBTransform = float4x4(float4(+1.0000f, +1.0000f, +1.0000f, +0.0000f),
                                                  float4(+0.0000f, -0.3441f, +1.7720f, +0.0000f),
                                                  float4(+1.4020f, -0.7141f, +0.0000f, +0.0000f),
                                                  float4(-0.7010f, +0.5291f, -0.8860f, +1.0000f));
    
    float4 ycbcr = float4(textureY.sample(colorSampler, mappingVertex.textureCoordinate).r,
                          textureUV.sample(colorSampler, mappingVertex.textureCoordinate).rg, 1.0);
    return ycbcrToRGBTransform * ycbcr;
}

kernel void writeRGBFromYUV(texture2d<uint, access::read> yTexture [[texture(0)]],
                            texture2d<uint, access::read> uTexture [[texture(1)]],
                            texture2d<uint, access::read> vTexture [[texture(2)]],
                            texture2d<float, access::write> rgbTexture [[texture(3)]],
                            uint2 yPosition [[thread_position_in_grid]])
{
    float3x3 yuvToRGBTransform = float3x3(float3(+1.0000f, +1.0000f, +1.0000f),
                                          float3(+0.0000f, -0.3441f, +1.7720f),
                                          float3(+1.4020f, -0.7141f, +0.0000f));

    uint2 uvPosition = uint2(yPosition.x / 2, yPosition.y / 2);

    float3 yuvMatrix = float3(yTexture.read(yPosition).r / 255.0,
                              uTexture.read(uvPosition).r / 255.0 - 0.5,
                              vTexture.read(uvPosition).r / 255.0 - 0.5);

    float3 rgbMatrix = yuvToRGBTransform * yuvMatrix;

    rgbTexture.write(float4(float3(rgbMatrix), 1.0), yPosition);
}
