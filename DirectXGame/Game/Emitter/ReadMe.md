# Emitter�ɂ���

## 1.�T�v
��������Emitter�Ƃ̓Q�[�����ƂɕK�v�ȓ��삪�قȂ邽�߁A�Ƃ肠�����ėp�I�Ȃ��̂��쐬���܂����B
 
 ����𔭓W������Particle�𔭐������Ă��������A�Ǝ��ɍ쐬���Ă����Ȃ��ł��B

 ## 2.�g����
 Initialie���āAUpdate��Draw���Ăяo�������ł��B

 ������ύX�������ꍇ��SetJobs���Ăяo���āAParticleJobs��or���Z�q�őg�ݍ��킹�ēn���Ă��������B

 �ȉ��́A�@�\��ǉ��������ꍇ�̐����ł��B

 ### 2-1.enum�ɋ@�\��ǉ�
 ���̂܂�܂ł��B
 ### 2-2.������ǉ�
 Update�֐����ɁA�C�ӂ̏ꏊ�ɏ����������Ă��������B�K�v�͏ꍇ�͊֐���ǉ����Ă��������B�N���X�����l
 ### 2-3.�ۑ�
 �ق��̐ݒ�Ɠ������ۑ����ڂɉ��������ꍇ�́A���̏ꏊ��ҏW���Ă��������B
 - DefaultEmitter::Save() ... �ۑ��������l��BinaryManager�ɓo�^����B���O�𑼂Ɣ��Ȃ��悤�ɂ���
	- DefaultEmitter::Load() ... Save�œo�^�������O���ǂݍ��܂�Ă����ꍇ��if��ǉ����āA���Ɠ����悤�ɒl��}������

---
�ȉ��́AParticleResource�ɂ��Ăł��B

# ParticleResource�ɂ���
## 1.�T�v
 �����������̃G���W���ɂ́A���f����e�N�X�`���Ȃǂ̃��\�[�X�ƁAID3D12Resource�ɂ��Ẵ��\�[�X�̓������܂��B
 
 Engine/Resource���O�ҁAEngine/Render/Resource����҂ł��B

 ParticleResource�́AEngine/Render/Resource�ɑ����Ă��܂��B

 ParticleResource�́A�s��ƃe�N�X�`�������ꂼ��z���GPU�ɑ��M����ړI�ō쐬����܂����B

 ## 2.�g����
 ParticleResource��Initialize���čő吔��o�^���ADraw�̎���Render::Draw�̈����ɓn�������ł��B

 �ȉ��́A�l�̍X�V���@�̐����ł��B

 ### 2-1.�s��̍X�V
 position_, rotate_, scale_���o�^�����ő吔�������݂���̂ŁA�����ɃC���X�^���X���Ƃ̒l�����Ă��������B
 
 �s��v�Z��DrawReady�ōs���܂����ARender::Draw���Ŏ����ōs���邽�ߕK�v����܂���B

 ### 2-n.���̑��̍X�V
 ��قǂ̐����ŉ��ƂȂ��@���������ł����ADrawReady���ł��ׂĂ̒l��GPU���M�p�Ƀp�b�P�[�W���Ă��܂��B

 public���ɂ���ϐ������������ɂ������Ă��������B

 ## 3.����
 �����ύX���ڂ�ǉ��������ꍇ�́AShader/Particle.[][].hlsl�̍\���̂��ύX���Ă��������B

 �����Ƒ傫���ύX�������ꍇ�́A�쐬���ɕ����Ă��������B���ؒ��J�ɗD���������܂��B
