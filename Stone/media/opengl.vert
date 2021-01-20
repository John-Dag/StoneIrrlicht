float3 fvLightPosition1;
float3 fvLightPosition2;
float3 fvEyePosition;
float4x4 matWorld;
float4x4 matWorldViewProjection;
float4x4 matWorldInverse;

struct VS_INPUT 
{
   float4 Position : POSITION0;
   float2 Texcoord1 : TEXCOORD0;
   float2 Texcoord2 : TEXCOORD1;
   float3 Normal :   NORMAL0;
   float3 Binormal : BINORMAL0;
   float3 Tangent :  TANGENT0;
};

struct VS_OUTPUT 
{
   float4 Position         :  POSITION0;
   float2 Texcoord1        :  TEXCOORD0;
   float2 Texcoord2        :  TEXCOORD1;
   float3 ViewDirection    :  TEXCOORD2;
   float3 LightDirection1  :  TEXCOORD3;
   float3 LightDirection2  :  TEXCOORD4;
   float3 basis1           :  TEXCOORD5;
   float3 basis2           :  TEXCOORD6;
   float3 basis3           :  TEXCOORD7;
   float  z                :  TEXCOORD8;
};

VS_OUTPUT vs_main( VS_INPUT IN )
{
   VS_OUTPUT OUT;
   OUT.Position = mul( IN.Position , matWorldViewProjection );
   OUT.Texcoord1 = IN.Texcoord1;
   OUT.Texcoord2 = IN.Texcoord2;
   OUT.z=OUT.Position.z;
   
   float3 RealPos = mul( IN.Position , matWorld);
   
   float3 viewVec = fvEyePosition - RealPos;
   float3 lightVec1 = fvLightPosition1 - RealPos;
   float3 lightVec2 = fvLightPosition2 - RealPos;

   float3 tViewVec;
   float3 tLightVec1;
   float3 tLightVec2;
   
   float3 OTangent =  normalize(IN.Tangent);
   float3 ONormal = normalize(IN.Normal);
   float3 OBinormal = cross( ONormal , OTangent );
   OTangent = cross( OBinormal,ONormal );
   
   float3 RTangent = mul( matWorldInverse , OTangent );
   float3 RBinormal = mul( matWorldInverse , OBinormal );
   float3 RNormal = mul( matWorldInverse , ONormal );

   tViewVec.x = dot(viewVec,RTangent);
   tViewVec.y = dot(viewVec,RBinormal);
   tViewVec.z = dot(viewVec,RNormal);
   
   tLightVec1.x = dot(lightVec1,RTangent);
   tLightVec1.y = dot(lightVec1,RBinormal);
   tLightVec1.z = dot(lightVec1,RNormal);

   tLightVec2.x = dot(lightVec2,RTangent);
   tLightVec2.y = dot(lightVec2,RBinormal);
   tLightVec2.z = dot(lightVec2,RNormal);
   
   OUT.ViewDirection = tViewVec;
   OUT.LightDirection1 = tLightVec1;
   OUT.LightDirection2 = tLightVec2;

   OUT.basis1.x = OTangent.x;
   OUT.basis1.y = OBinormal.x;
   OUT.basis1.z = ONormal.x;
   OUT.basis2.x = OTangent.y;
   OUT.basis2.y = OBinormal.y;
   OUT.basis2.z = ONormal.y;
   OUT.basis3.x = OTangent.z;
   OUT.basis3.y = OBinormal.z;
   OUT.basis3.z = ONormal.z;
      
   return( OUT );
   
}