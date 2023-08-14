#include "EnemyStrong.h"

#include "GameScene.h"
#include "Player.h"

void EnemyStrong::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULL�|�C���^�`�F�b�N
	assert(model);
	model_ = model;
	// �e�N�X�`���ǂݍ���
	textureHandle_ = TextureManager::Load("model/enemy.png");

	// ���[���h�ϊ��̏�����
	worldTransform_.Initialize();
	// �����Ŏ󂯎�����������W���Z�b�g
	worldTransform_.translation_ = position;

	// ���x
	velocity_ = velocity;

	// �ڋ߃t�F�[�Y������
	ApproachInitialize();
}

void EnemyStrong::Update() {
	// �t�F�[�Y
	switch (phase_) {
	case Phase::Approach:
	default:
		// �ړ�
		ApproachUpdate();
		break;
	case Phase::Leave:
		LeaveUpdate();
		break;
	}

	// HP��0�ɂȂ�����f�X
	if (HP <= 0) {
		isDead_ = true;
	}

	worldTransform_.UpdateMatrix();
}

void EnemyStrong::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

void EnemyStrong::ApproachInitialize() {
	// ���˃^�C�}�[��������
	kFireTimer = kFireInterval;
}

void EnemyStrong::ApproachUpdate() {
	// ���W���ړ�������
	worldTransform_.translation_.z += velocity_.z;

	// �K��̈ʒu�ɓ��B�����痣�E
	if (worldTransform_.translation_.z < 10.0f) {
		phase_ = Phase::ATTACK;
	}

	// ���˃^�C�}�[�J�E���g�_�E��
	--kFireTimer;
	if (kFireTimer <= 0) {
		// �e�𔭎�
		Fire();
		// ���˃^�C�}�[��������
		kFireTimer = kFireInterval;
	}
}

void EnemyStrong::AttackUpdate() {
	// ���˃^�C�}�[�J�E���g�_�E��
	--kFireTimer;
	if (kFireTimer <= 0) {
		// �e�𔭎�
		Fire();
		// ���˃^�C�}�[��������
		kFireTimer = kFireInterval;
	}

	// ���E�t�F�[�Y��
	if (--toLeaveTimer <= 0) {
		phase_ = Phase::Leave;
	}
}

void EnemyStrong::LeaveUpdate() {
	if (deathTimer_-- < 0) {
		isDead_ = true;
	}

	// ���W���ړ�������(1�t���[�����̈ړ��ʂ𑫂�����)
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;
	worldTransform_.translation_.z += velocity_.z;
}

void EnemyStrong::Fire() {
	assert(player_);

	// �e�̑��x
	const float kBulletSpeed = -0.5f;

	Vector3 playerWorldPos = player_->GetWorldPositiopn(); // ���L�����̃��[���h���W
	Vector3 enemyWorldPos = GetWorldPositiopn();           // �G�L�����̃��[���h���W
	// �G�L�����Ǝ��L�����̍����x�N�g�������߂�
	Vector3 velocity;
	velocity.x = enemyWorldPos.x - playerWorldPos.x;
	velocity.y = enemyWorldPos.y - playerWorldPos.y;
	velocity.z = enemyWorldPos.z - playerWorldPos.z;
	// ���K��
	velocity = Normalize(velocity);
	// �����𑬂��ɍ��킹��
	velocity.x *= kBulletSpeed;
	velocity.y *= kBulletSpeed;
	velocity.z *= kBulletSpeed;

	// �e�𐶐���������
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	// �e��o�^����
	gameScene_->AddEnemyBullet(newBullet);
}

void EnemyStrong::OnColision() { --HP; }

Vector3 EnemyStrong::GetWorldPositiopn() {
	// ���[���h���W������ϐ�
	Vector3 worldPos;

	// ���[���h�s��̕��s�ړ��������擾(���[���h���W)
	worldPos.x = worldTransform_.translation_.x;
	worldPos.y = worldTransform_.translation_.y;
	worldPos.z = worldTransform_.translation_.z;

	return worldPos;
}
