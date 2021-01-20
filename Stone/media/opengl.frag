float4 LightColor1;
float4 AmbientColor1;
float4 SpecularColor1;
float4 LightColor2;
float4 AmbientColor2;
float4 SpecularColor2;
float4x4 view_matrix;
float4 fogColor;
float near;
float far;
float density;

sampler2D baseMap;
sampler2D bumpMap;
sampler2D refMap;
sampler2D ShadowMap;

struct PS_INPUT 
{
   float2 Texcoord1      :   TEXCOORD0;
   float2 Texcoord2      :   TEXCOORD1;
   float3 ViewDirection  :   TEXCOORD2;
   float3 LightDirection1:   TEXCOORD3;
   float3 LightDirection2:   TEXCOORD4;
   float3 basis1         :   TEXCOORD4;
   float3 basis2         :   TEXCOORD5;
   float3 basis3         :   TEXCOORD6;
   float z               :   TEXCOORD7;
};

float4 ps_main( PS_INPUT IN ) : COLOR0
{ 
      float4 color=fogColor;
      if(IN.z < far){
      
      float ZDepth=min(max(near,IN.z),far);
      float fog=((far-near)-(ZDepth-near))/(far-near);
      fog=pow(fog,density);
      
      float4 base = tex2D(baseMap,IN.Texcoord1);
      float4 shadow = tex2D(ShadowMap,IN.Texcoord2);
      
      base*=shadow;
      
      float3 normal = tex2D(bumpMap,IN.Texcoord1);
      normal = normal*2-1;      

      float3 viewDir = normalize(IN.ViewDirection);
      float3 lightDir1 = normalize(IN.LightDirection1);
      float3 lightDir2 = normalize(IN.LightDirection2);
      
      normal = normalize(normal);
      float3 halfView1 = normalize(lightDir1+viewDir);
      float3 halfView2 = normalize(lightDir2+viewDir);
      
      float specPower = 128*base.w;      
      
      float diffuse1 = dot(normal,lightDir1);
      float specular1 = pow(saturate(dot(normal,halfView1)),specPower);

      float diffuse2 = dot(normal,lightDir2);
      float specular2 = pow(saturate(dot(normal,halfView2)),specPower);

      float3 reflection=normal+1;
      reflection.x= dot(normal,IN.basis1);
      reflection.y= dot(normal,IN.basis2);
      reflection.z= dot(normal,IN.basis3);
      float3 refVec = mul(view_matrix,reflection);
      refVec = normalize(refVec);
      refVec = refVec+1;
      refVec = normalize(refVec);
      
      float fresnel = 1-pow(dot(normal,viewDir),16);
      refVec.xy = refVec.xy*0.5 + 0.5;

      float4 Reflection = tex2D(refMap,refVec.xy)*fresnel*base.w;
      
      color = (saturate(AmbientColor1*base*0.25 + diffuse1*base*LightColor1 + specular1*base*SpecularColor1) +
               saturate(AmbientColor2*base*0.25 + diffuse2*base*LightColor2 + specular2*base*SpecularColor2) +
               Reflection)*fog + (1-fog)*color;
      }
      return ( color );
}