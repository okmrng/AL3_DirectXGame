#pragma once

#include "EnemyBullet.h"
#include "Material.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <cassert>
#include <cmath>
#include <list>

// �O���錾
class Player;
class GameScene;

/// <summary>
/// �����G
/// </summary>
class EnemyMove {
public:
	/// <summary>
	/// ������
	/// </summary>
	/// <param name="model">���f��</param>
	/// <param name="position">�������W</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// �X�V
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	/// <param name="viewProjection">�r���[�v���W�F�N�V����</param>
	void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// �ڋ߃t�F�[�Y�̏������֐�
	/// </summary>
	void ApproachInitialize();

	/// <summary>
	/// �ڋ߃t�F�[�Y�̍X�V�֐�
	/// </summary>
	void ApproachUpdate();

	/// <summary>
	/// ���E�t�F�[�Y�̍X�V�֐�
	/// </summary>
	void LeaveUpdate();

	/// <summary>
	/// �U��
	/// </summary>
	void Fire();

	/// <summary>
	/// �Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	/// </summary>
	void OnColision();

	/// <summary>
	/// �{���Ƃ̏Փˏ���
	/// </summary>
	void OnBombCollision();

	// �Z�b�^�[
	void SetPlayer(Player* player) { player_ = player; }

	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

	// �Q�b�^�[
	/// <summary>
	/// ���[���h���W���擾
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPositiopn();

	/// <summary>
	/// ���a�擾
	/// </summary>
	/// <returns></returns>
	const float GetRadius() { return radius_; }

	bool GetIsDead() { return isDead_; }

	static const int kFireInterval = 60; // ���ˊԊu

private:
	int32_t kFireTimer = 0; // ���˃^�C�}�[

	WorldTransform worldTransform_; // ���[���h�ϊ��f�[�^

	// ���f��
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u; // �e�N�X�`���n���h��

	Vector3 velocity_; // ���x

	// �t�F�[�Y
	// �s���t�F�[�Y
	enum class Phase {
		Approach, // �ڋ߂���
		Leave     // ���E����
	};
	Phase phase_ = Phase::Approach;
	int32_t toLeaveTimer = 60 * 3; // ���E�t�F�[�Y�Ɉڂ�܂ł̎���

	Player* player_ = nullptr; // ���L����

	const float radius_ = 2.0f; // ���a

	GameScene* gameScene_ = nullptr; // �Q�[���V�[��

	// �f�X
	int32_t deathTimer_ = 300; // �f�X�^�C�}�[
	bool isDead_ = false;      // �f�X�t���O

	int32_t HP = 5;
};
