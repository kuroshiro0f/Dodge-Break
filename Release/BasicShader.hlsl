Texture2D<float4> tex:register(t0);		//	0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(�x�[�X)
Texture2D<float4> sph:register(t1);		//	1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(��Z)
Texture2D<float4> spa:register(t2);		//	2�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(���Z)
Texture2D<float4> toon:register(t3);	//	3�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��(�g�D�[��)

SamplerState smp:register(s0);	//	0�ԃX���b�g�ɐݒ肳�ꂽ�T���v��
SamplerState smpToon:register(s1);	//	1�ԃX���b�g�ɐݒ肳�ꂽ�T���v��

//	�萔�o�b�t�@0
cbuffer SceneData : register(b0) {
	matrix view;
	matrix proj;	//	�r���[�v���W�F�N�V�����s��
	float3 eye;
};
cbuffer Transform : register(b1) {
	matrix world;	//	���[���h�ϊ��s��
}

//	�萔�o�b�t�@1
//	�}�e���A���p
cbuffer Material : register(b2) {
	float4 diffuse;		//	�f�B�t���[�Y�F
	float4 specular;	//	�X�y�L����
	float3 ambient;		//	�A���r�G���g
};

//	���_�V�F�[�_���s�N�Z���V�F�[�_�ւ̂����Ɏg�p����
//	�\����
struct Output {
	float4 svpos:SV_POSITION;	//	�V�X�e���p���_���W
	float4 pos:POSITION;		//	�V�X�e���p���_���W
	float4 normal:NORMAL0;		//	�@���x�N�g��
	float4 vnormal:NORMAL1;		//	�@���x�N�g��
	float2 uv:TEXCOORD;			//	UV�l
	float3 ray:VECTOR;			//	�x�N�g��
};

Output BasicVS(float4 pos : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD) {
	Output output;	//	�s�N�Z���V�F�[�_�֓n���l
	pos = mul(world, pos);
	output.svpos = mul(mul(proj, view), pos);	//	�V�F�[�_�ł͗�D��Ȃ̂Œ���
	output.pos = mul(view, pos);
	normal.w = 0;	//	�����d�v(���s�ړ������𖳌��ɂ���)
	output.normal = mul(world, normal);	//	�@���ɂ����[���h�ϊ����s��
	output.vnormal = mul(view, output.normal);
	output.uv = uv;
	output.ray = normalize(pos.xyz - mul(view, eye));	//	�����x�N�g��

	return output;
}

float4 BasicPS(Output input) : SV_TARGET{
	float3 light = normalize(float3(1,-1,1));	//	���̌������x�N�g��(���s����)
	float3 lightColor = float3(1,1,1);	//	���C�g�̃J���[(1,1,1�Ő^����)

	//	�f�B�t���[�Y�v�Z
	float diffuseB = saturate(dot(-light, input.normal));
	float4 toonDif = toon.Sample(smpToon, float2(0, 1.0 - diffuseB));

	//	���̔��˃x�N�g��
	float3 refLight = normalize(reflect(light, input.normal.xyz));
	float specularB = pow(saturate(dot(refLight, -input.ray)),specular.a);

	//	�X�t�B�A�}�b�v�pUV
	float2 sphereMapUV = input.vnormal.xy;
	sphereMapUV = (sphereMapUV + float2(1, -1)) * float2(0.5, -0.5);

	float4 texColor = tex.Sample(smp, input.uv);	//	�e�N�X�`���J���[

	return saturate(toonDif	//	�P�x(�g�D�[��)
		* diffuse	//	�f�B�t���[�Y�F
		* texColor	//	�e�N�X�`���J���[
		* sph.Sample(smp, sphereMapUV))	//	�X�t�B�A�}�b�v(��Z)
		+ saturate(spa.Sample(smp, sphereMapUV) * texColor	//	�X�t�B�A�}�b�v(���Z)
		+ float4(specularB * specular.rgb, 1))	//	�X�y�L�����[
		+ float4(texColor * ambient * 0.5,1);	//	�A���r�G���g(���邭�Ȃ肷����̂�0.5)
}